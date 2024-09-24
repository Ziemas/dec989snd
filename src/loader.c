#include "common.h"
#include "functions.h"
#include "globals.h"
#include "sifrpc.h"
#include "stdio.h"
#include "thread.h"
#include <string.h>

/* data 1344 */ BOOL gLimit2Meg = 0;
/* data 1348 */ SInt32 gFileHandle = -1;
/* data 134c */ SInt32 gFileStartSector = 0;
/* data 1350 */ SInt32 gFileStartOffset = 0;
/* data 1354 */ SInt32 gFileNextReadSector = 0;
/* data 1358 */ SInt32 gFileNextReadOffset = 0;
/* data 135c */ SInt32 gFileMarkSector = 0;
/* data 1360 */ SInt32 gFileMarkOffset = 0;
/* data 1364 */ SInt32 gLastLoadError = 0;
/* data 1368 */ UInt32 gEEDataLocStart = 0;
/* data 136c */ UInt32 gEEDataLocNext = 0;
/* data 1370 */ UInt32 gEEDataMark = 0;
/* data 1374 */ void *gIOPDataLocStart = NULL;
/* data 1378 */ ExternSndIOPAlloc gAllocProc = snd_IOPMemAlloc;
/* data 137c */ ExternSndIOPFree gFreeProc = snd_IOPMemFree;
/* data 1380 */ char *gEEStatusAddr = NULL;
/* data 1384 */ SoundBankPtr gBankListHead = NULL;
/* data 1388 */ SFXBlock2Ptr gBlockListHead = NULL;
/* data 138c */ MIDIBlockHeaderPtr gMIDIListHead = NULL;
/* data 1390 */ SInt32 gReadBufferHasSector = 0;
/* bss 1d0 */ SInt8 gFileLoadBuffer[2048];
/* bss 180 */ SndSystemStatus gStats;
/* bss 1c0 */ char gEEAlignBuffer[16];

void snd_InitLoader() {
    struct SemaParam sp;

    sp.attr = 0;
    sp.initCount = 0;
    sp.maxCount = 1;
    sp.option = 0;

    gSPURAMTransSema = CreateSema(&sp);
    if (gSPURAMTransSema < 0) {
        snd_ShowError(115, 0, 0, 0, 0);
        while (1)
            ;
    }

    gTransfering = 0;
    gTransferDoneCallback = 0;
    gBankListHead = 0;
    gBlockListHead = 0;
    gMIDIListHead = 0;
    gAllocProc = snd_IOPMemAlloc;
    gFreeProc = snd_IOPMemFree;
}

void snd_ShutdownLoader() {
    DeleteSema(gSPURAMTransSema);
    gSPURAMTransSema = -1;
}

void snd_RegisterIOPMemAllocator(ExternSndIOPAlloc AllocProc,
                                 ExternSndIOPFree FreeProc) {
    if (AllocProc) {
        gAllocProc = AllocProc;
    } else {
        gAllocProc = snd_IOPMemAlloc;
    }

    if (FreeProc) {
        gFreeProc = FreeProc;
    } else {
        gFreeProc = snd_IOPMemFree;
    }
}

BOOL snd_OpenDataSourceByName(char *name, SInt32 offset) {
    gFileHandle = open(name, 1);
    if (gFileHandle < 0) {
        return 0;
    }

    gFileStartOffset = offset;
    lseek(gFileHandle, gFileStartOffset, 0);

    return 1;
}

BOOL snd_OpenDataSourceByLoc(SInt32 sector, SInt32 offset) {
    gFileStartSector = sector;
    gFileStartOffset = offset;
    gFileNextReadSector = sector;
    gFileNextReadOffset = offset;
    gFileMarkSector = sector;
    gFileMarkOffset = offset;
    gFileHandle = 1;

    return 1;
}

BOOL snd_OpenDataSourceFromEE(UInt32 ee_loc) {
    gEEDataLocStart = ee_loc;
    gEEDataLocNext = ee_loc;
    gEEDataMark = ee_loc;
    gFileHandle = 2;

    return 1;
}

// INCLUDE_ASM("asm/nonmatchings/loader", snd_CloseDataSource);
void snd_CloseDataSource() {
    if (gFileHandle == -1) {
        return;
    }

    if (!gFileStartSector) {
        if (!gEEDataLocStart) {
            close(gFileHandle);
        } else {
            gEEDataLocStart = 0;
        }
    } else {
        gFileStartSector = 0;
        gFileStartOffset = 0;
        gFileNextReadSector = 0;
        gFileNextReadOffset = 0;
    }

    gFileHandle = -1;
}

SInt32 snd_SeekDataSource(SInt32 where, SInt32 from_where) {
    if (gFileHandle == -1) {
        return 0;
    }

    if (!gFileStartSector) {
        if (!gEEDataLocStart) {
            if (!from_where) {
                return lseek(gFileHandle, where + gFileStartOffset, 0);
            } else {
                return lseek(gFileHandle, where + gFileMarkOffset, 0);
            }
        } else {
            if (!from_where) {
                gEEDataLocNext = gEEDataLocStart + where;
            } else {
                gEEDataLocNext = gEEDataMark + where;
            }
            return gEEDataLocNext;
        }
    } else {
        if (!from_where) {
            gFileNextReadSector = gFileStartSector;
            gFileNextReadOffset = gFileStartOffset + where;
        } else {
            gFileNextReadSector = gFileMarkSector;
            gFileNextReadOffset = gFileMarkOffset + where;
        }
        if (gFileNextReadOffset >= 2048) {
            gFileNextReadSector += gFileNextReadOffset / 2048;
            gFileNextReadOffset %= 2048;
        }

        return where;
    }

    return 0;
}

void snd_SetDataSourceMark() {
    if (gFileHandle == -1) {
        return;
    }

    if (!gFileStartSector) {
        if (!gEEDataLocStart) {
            gFileMarkOffset = lseek(gFileHandle, 0, 1);
        } else {
            gEEDataMark = gEEDataLocNext;
        }
    } else {
        gFileMarkSector = gFileNextReadSector;
        gFileMarkOffset = gFileNextReadOffset;
    }
}

SInt32 snd_GetLastLoadError() { return gLastLoadError; }

SInt32 snd_ReadBytesFromEE(UInt32 ee_loc, void *iop_loc, SInt32 bytes) {
    SInt32 ret;
    sceSifReceiveData data_track;
    SInt32 full_blocks;
    SInt32 get_blocks;
    SInt32 read_bytes;
    SInt32 part_size;

    read_bytes = bytes;
    if ((ee_loc % 16)) {
        part_size = 16 - (ee_loc % 16);
        ee_loc -= ee_loc % 16;
        ret = sceSifGetOtherData(&data_track, (void *)ee_loc, gEEAlignBuffer,
                                 16, 0);
        if (ret < 0) {
            return 0;
        }

        memcpy(iop_loc, &gEEAlignBuffer[16 - part_size], part_size);

        bytes -= part_size;
        iop_loc += part_size;
        ee_loc += 16;
    }

    for (full_blocks = bytes / 16; full_blocks; full_blocks -= get_blocks) {
        get_blocks = (full_blocks > 0x3fff) ? 0x3fff : full_blocks;
        ret = sceSifGetOtherData(&data_track, (void *)ee_loc, iop_loc,
                                 16 * get_blocks, 0);
        if (ret < 0) {
            return 0;
        }

        bytes -= 16 * get_blocks;
        iop_loc += 16 * get_blocks;
        ee_loc += 16 * get_blocks;
    }

    if (bytes) {
        ret = sceSifGetOtherData(&data_track, (void *)ee_loc, gEEAlignBuffer,
                                 16, 0);
        if (ret < 0) {
            return 0;
        }

        memcpy(iop_loc, gEEAlignBuffer, bytes);
    }

    return read_bytes;
}

SInt32 snd_ReadBytes(void *buffer, SInt32 count) {
    SInt32 ret;

    if (gFileHandle == -1) {
        return 0;
    }

    if (!gFileStartSector) {
        if (!gEEDataLocStart) {
            ret = read(gFileHandle, buffer, count);
            if (ret != count) {
                gLastLoadError = 257;
            }
            return ret;
        } else {
            ret = snd_ReadBytesFromEE(gEEDataLocNext, buffer, count);
            if (ret != count) {
                gLastLoadError = 257;
                return 0;
            }

            gEEDataLocNext += count;
            return ret;
        }
    } else {
        if (snd_FileRead(gFileNextReadSector, gFileNextReadOffset, count,
                         buffer) == 0) {
            return 0;
        }

        gFileNextReadOffset += count;
        if (gFileNextReadOffset >= 2048) {
            gFileNextReadSector += gFileNextReadOffset / 2048;
            gFileNextReadOffset %= 2048;
        }
        return count;
    }

    return 0;
}

SoundBankPtr snd_BankLoadEx(SInt8 *name, SInt32 offset, UInt32 spu_mem_loc,
                            UInt32 spu_mem_size) {
    SoundBankPtr ret;

    if (!snd_OpenDataSourceByName(name, offset)) {
        gLastLoadError = 256;
        return NULL;
    }

    gLastLoadError = 0;
    ret = snd_ReadBank(spu_mem_loc, spu_mem_size);
    snd_CloseDataSource();
    if (ret && g989Monitor) {
        if (g989Monitor->bnk_load) {
            g989Monitor->bnk_load(ret, 1);
        }
    }

    return ret;
}

void snd_IterateBlockListToMonitor() {
    SFXBlock2Ptr block;

    if (!g989Monitor || !g989Monitor->bnk_load) {
        return;
    }

    for (block = gBlockListHead; block; block = block->NextBlock) {
        g989Monitor->bnk_load(block, 1);
    }
}

SoundBankPtr snd_ParseIOPBank(SInt8 *iop_loc, UInt32 spu_mem_loc,
                              UInt32 spu_mem_size) {
    SInt32 result;
    SoundBankPtr bank_head;
    SFXBlock2Ptr block;
    SInt8 *bank_head_ram;
    FileAttributesPtr fa;
    SInt32 size;
    SInt8 *data_loc;

    bank_head = NULL;
    fa = (FileAttributesPtr)iop_loc;
    if (fa->type == 1 || fa->type == 3) {
        // If we have a midi chunk we want to be able to store its ptr, so add
        // space for it
        size =
            (fa->num_chunks >= 3) ? (fa->where[0].size + 4) : fa->where[0].size;
        bank_head_ram = gAllocProc(size, 1, 0);
        if (!bank_head_ram) {
            snd_ShowError(15, size, 0, 0, 0);
            gLastLoadError = 258;
            return 0;
        }

        if (fa->num_chunks == 3) {
            // first 4 bytes will host the midi chunk ptr
            bank_head = (SoundBankPtr)(bank_head_ram + 4);
        } else {
            bank_head = (SoundBankPtr)(bank_head_ram);
        }

        memcpy(bank_head, iop_loc + fa->where[0].offset, fa->where[0].size);

        if (fa->where[1].size) {
            data_loc = iop_loc + fa->where[1].offset;
            result = snd_BankTransfer(bank_head, data_loc, fa->where[1].size, 0,
                                      0, spu_mem_loc, spu_mem_size, 0);
            snd_IsCurrentTransferComplete(1);
            snd_EndBankTransfer(bank_head);
        } else {
            if (bank_head->DataID == 0x32764253) {
                bank_head->FirstSound =
                    (SoundPtr)((UInt32)bank_head->FirstSound +
                               (UInt32)bank_head);
                bank_head->FirstProg =
                    (ProgPtr)((UInt32)bank_head->FirstProg + (UInt32)bank_head);
                bank_head->FirstTone =
                    (TonePtr)((UInt32)bank_head->FirstTone + (UInt32)bank_head);
                bank_head->Flags |= 1;
            } else {
                block = (SFXBlock2Ptr)bank_head;
                block->FirstSound =
                    (SFX2Ptr)((UInt32)block->FirstSound + (UInt32)block);
                block->FirstGrain =
                    (SFX2Ptr)((UInt32)block->FirstGrain + (UInt32)block);
                block->Flags |= 1;
            }
            snd_EndBankTransfer(bank_head);
        }

        if (fa->num_chunks == 3) {
            *((MultiMIDIBlockHeaderPtr *)bank_head_ram) =
                snd_MMDLoadFromIOPLoc(iop_loc + fa->where[2].offset);
            if ((*(MultiMIDIBlockHeaderPtr *)bank_head_ram) == NULL) {
                snd_RemoveBank(bank_head);
                gFreeProc(bank_head_ram);
                snd_ShowError(19, 0, 0, 0, 0);
                return 0;
            } else {
                bank_head->Flags |= 8;
            }
        }
    } else {
        snd_ShowError(84, 0, 0, 0, 0);
        gLastLoadError = 263;
        bank_head = 0;
    }

    return bank_head;
}

SoundBankPtr snd_ReadBank(UInt32 spu_mem_loc, UInt32 spu_mem_size) {
    SInt32 result;
    SInt32 max_length;
    SoundBankPtr bank_head;
    SFXBlock2Ptr block_head;
    SInt8 *bank_head_ram;
    SInt8 *bank_body;
    FileAttributesPtr fa;
    SInt32 block;
    SInt32 size;
    SInt32 sram_offset;
    SInt8 buff[32];

    bank_head = NULL;
    bank_body = NULL;

    result = snd_ReadBytes(buff, sizeof(buff));
    if (result != 32) {
        gReadBufferHasSector = 0;
        snd_ShowError(14, gLastLoadError, 0, 0, 0);
        return NULL;
    }

    fa = (FileAttributesPtr)buff;
    if (fa->type == 1 || fa->type == 3) {
        // If we have a midi chunk we want to be able to store its ptr, so add
        // space for it
        size =
            (fa->num_chunks >= 3) ? (fa->where[0].size + 4) : fa->where[0].size;

        bank_head_ram = gAllocProc(size, 1, 0);
        if (!bank_head_ram) {
            snd_ShowError(15, size, 0, 0, 0);
            gLastLoadError = 258;
            return 0;
        }

        if (fa->num_chunks == 3) {
            // first 4 bytes will host the midi chunk ptr
            bank_head = (SoundBankPtr)(bank_head_ram + 4);
        } else {
            bank_head = (SoundBankPtr)(bank_head_ram);
        }

        if (fa->where[1].size) {
            bank_body = gAllocProc(fa->where[1].size, 8, &max_length);
            if (max_length > fa->where[1].size) {
                max_length = fa->where[1].size;
            }
            if (!bank_body) {
                snd_ShowError(16, 0, 0, 0, 0);
                gFreeProc(bank_head_ram);
                gLastLoadError = 258;
                return 0;
            }
        }

        snd_SeekDataSource(fa->where[0].offset, 0);
        result = snd_ReadBytes((char *)bank_head, fa->where[0].size);
        if (result != fa->where[0].size) {
            gReadBufferHasSector = 0;
            snd_ShowError(17, gLastLoadError, 0, 0, 0);
            gFreeProc(bank_head_ram);
            gFreeProc(bank_body);
            return 0;
        }
        if (bank_head->DataID == 0x6B6C4253 && bank_head->Version < 2) {
            snd_ShowError(113, 0, 0, 0, 0);
            gFreeProc(bank_head_ram);
            gFreeProc(bank_body);
            return 0;
        }
        if (fa->where[1].size) {
            snd_SeekDataSource(fa->where[1].offset, 0);
            block = 0;
            sram_offset = 0;
            while (fa->where[1].size) {
                result = snd_ReadBytes(bank_body, max_length);
                if (result != max_length) {
                    gReadBufferHasSector = 0;
                    snd_ShowError(14, gLastLoadError, 0, 0, 0);
                    gFreeProc(bank_head_ram);
                    gFreeProc(bank_body);
                    return 0;
                }

                if (!block) {
                    result =
                        snd_BankTransfer(bank_head, bank_body, max_length, 0, 0,
                                         spu_mem_loc, spu_mem_size, 0);
                } else {
                    result = snd_BankTransfer(bank_head, bank_body, max_length,
                                              sram_offset, 1, spu_mem_loc,
                                              spu_mem_size, 0);
                }
                if (result < 0) {
                    snd_ShowError(18, 0, 0, 0, 0);
                    gFreeProc(bank_head_ram);
                    gFreeProc(bank_body);
                    return 0;
                }
                snd_IsCurrentTransferComplete(1);
                sram_offset += max_length;
                ++block;
                fa->where[1].size -= max_length;
                if (fa->where[1].size < max_length) {
                    max_length = fa->where[1].size;
                }
            }

            snd_EndBankTransfer((SFXBlock2Ptr)bank_head);
            gFreeProc(bank_body);
        } else {
            if (bank_head->DataID == 0x32764253) {
                bank_head->FirstSound =
                    (SoundPtr)((UInt32)bank_head->FirstSound +
                               (UInt32)bank_head);
                bank_head->FirstProg =
                    (ProgPtr)((UInt32)bank_head->FirstProg + (UInt32)bank_head);
                bank_head->FirstTone =
                    (TonePtr)((UInt32)bank_head->FirstTone + (UInt32)bank_head);
                bank_head->Flags |= 1;
            } else {
                block_head = (SFXBlock2Ptr)bank_head;
                block_head->FirstSound =
                    (SFX2Ptr)((UInt32)block_head->FirstSound +
                              (UInt32)block_head);
                block_head->FirstGrain =
                    (SFX2Ptr)((UInt32)block_head->FirstGrain +
                              (UInt32)block_head);
                block_head->Flags |= 1;
            }
            snd_EndBankTransfer(bank_head);
        }
        if (fa->num_chunks == 3) {
            snd_SeekDataSource(fa->where[2].offset, 0);
            *(MultiMIDIBlockHeaderPtr *)bank_head_ram =
                snd_MMDLoadFromDataSource();
            if (!*(MultiMIDIBlockHeaderPtr *)bank_head_ram) {
                snd_RemoveBank(bank_head);
                gFreeProc(bank_head_ram);
                snd_ShowError(19, 0, 0, 0, 0);
                return 0;
            } else {
                bank_head->Flags |= 8u;
            }
        }
    } else {
        snd_ShowError(84, (UInt32)buff, gFileStartSector, gReadBufferHasSector,
                      0);
        gReadBufferHasSector = -1;
        gLastLoadError = 263;
        bank_head = NULL;
    }

    return bank_head;
}

INCLUDE_ASM("asm/nonmatchings/loader", snd_FileRead);

INCLUDE_ASM("asm/nonmatchings/loader", snd_BankLoadByLocEx);

INCLUDE_ASM("asm/nonmatchings/loader", snd_BankLoadFromEEEx);

INCLUDE_ASM("asm/nonmatchings/loader", snd_GetSRAMUsedByBank);

INCLUDE_ASM("asm/nonmatchings/loader", snd_BankLoadFromIOPEx);

INCLUDE_ASM("asm/nonmatchings/loader", snd_UnloadBank);

INCLUDE_ASM("asm/nonmatchings/loader", snd_BankLoaded);

INCLUDE_ASM("asm/nonmatchings/loader", snd_UnloadBlock);

INCLUDE_ASM("asm/nonmatchings/loader", snd_BlockLoaded);

INCLUDE_ASM("asm/nonmatchings/loader", snd_MMDLoad);

INCLUDE_ASM("asm/nonmatchings/loader", snd_MMDLoadFromIOPLoc);

INCLUDE_ASM("asm/nonmatchings/loader", snd_MMDLoadFromDataSource);

INCLUDE_ASM("asm/nonmatchings/loader", snd_MMDLoadByLoc);

INCLUDE_ASM("asm/nonmatchings/loader", snd_UnloadMMD);

INCLUDE_ASM("asm/nonmatchings/loader", snd_UnloadAllMMD);

INCLUDE_ASM("asm/nonmatchings/loader", snd_BankTransfer);

INCLUDE_ASM("asm/nonmatchings/loader", snd_ClearTransSema);

INCLUDE_ASM("asm/nonmatchings/loader", snd_IsCurrentTransferComplete);

INCLUDE_ASM("asm/nonmatchings/loader", snd_EndBankTransfer);

INCLUDE_ASM("asm/nonmatchings/loader", snd_RemoveBank);

INCLUDE_ASM("asm/nonmatchings/loader", snd_RemoveBlock);

INCLUDE_ASM("asm/nonmatchings/loader", snd_UnloadAllBanks);

INCLUDE_ASM("asm/nonmatchings/loader", snd_ResolveBankXREFS);

INCLUDE_ASM("asm/nonmatchings/loader", snd_FindBankByID);

INCLUDE_ASM("asm/nonmatchings/loader", snd_FindBlockByName);

INCLUDE_ASM("asm/nonmatchings/loader", snd_FindBlockByID);

INCLUDE_ASM("asm/nonmatchings/loader", snd_FindBankByNum);

INCLUDE_ASM("asm/nonmatchings/loader", snd_FindBlockByNum);

INCLUDE_ASM("asm/nonmatchings/loader", snd_RegisterMIDI);

INCLUDE_ASM("asm/nonmatchings/loader", snd_UnregisterMIDI);

INCLUDE_ASM("asm/nonmatchings/loader", snd_FindMIDIBlock);

INCLUDE_ASM("asm/nonmatchings/loader", snd_InsertMIDIBlockInList);

INCLUDE_ASM("asm/nonmatchings/loader", snd_MIDIStreamGone);

INCLUDE_ASM("asm/nonmatchings/loader", snd_SetCDSifReg);

INCLUDE_ASM("asm/nonmatchings/loader", snd_IOPMemAlloc);

INCLUDE_ASM("asm/nonmatchings/loader", snd_IOPMemFree);

INCLUDE_ASM("asm/nonmatchings/loader", snd_GetFreeSPUDMA);

INCLUDE_ASM("asm/nonmatchings/loader", snd_FreeSPUDMA);

INCLUDE_ASM("asm/nonmatchings/loader", snd_WaitDMADone);
