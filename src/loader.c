#include "common.h"
#include "functions.h"
#include "globals.h"
#include "intrman.h"
#include "kerror.h"
#include "libsd.h"
#include "sif.h"
#include "sifrpc.h"
#include "stdio.h"
#include "sysmem.h"
#include "thread.h"

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

SInt32 snd_FileRead(SInt32 sect_loc, SInt32 offset, SInt32 size,
                    SInt8 *buffer) {
    sceCdRMode mode;
    SInt32 bytes_needed;
    SInt32 move_bytes;
    SInt32 sector;
    SInt32 get_whole_sectors;
    SInt32 err;

    move_bytes = 0;
    WaitSema(gFileReadMutex);
    snd_StreamSafeCdSync(0);
    snd_StreamSafeCdSync(16);
    snd_SetCDSifReg(1, 0);
    mode.trycount = 0x64;
    mode.spindlctrl = 0;
    mode.datapattern = 0;
    bytes_needed = size;
    sector = sect_loc + offset / 2048;
    offset %= 2048;

    if (bytes_needed < 2048 || offset) {
        if (sector != gReadBufferHasSector) {
            err = snd_StreamSafeCdRead(sector, 1, gFileLoadBuffer);
            if (!err && !gPrefs_Silent) {
                printf("Error Kicking Off Read!\n");
            }
            snd_StreamSafeCdSync(0);
            if (!err || (err = snd_StreamSafeCdGetError()) != 0) {
                gReadBufferHasSector = 0;
                if (!err) {
                    err = 48;
                }
                snd_SetCDSifReg(0, err);
                gLastLoadError = err;
                SignalSema(gFileReadMutex);
                return 0;
            }
            gReadBufferHasSector = sector;
        }

        move_bytes = 2048 - offset % 2048;

        if (move_bytes > bytes_needed) {
            move_bytes = bytes_needed;
        }
        memcpy(buffer, gFileLoadBuffer + offset % 2048, move_bytes);
        bytes_needed = bytes_needed - move_bytes;

        if (bytes_needed == 0) {
            snd_SetCDSifReg(0, 0);
            SignalSema(gFileReadMutex);
            return 1;
        }

        sector++;
    }

    get_whole_sectors = bytes_needed / 2048;
    if (get_whole_sectors) {
        err = snd_StreamSafeCdRead(sector, get_whole_sectors,
                                   &buffer[move_bytes]);
        snd_StreamSafeCdSync(0);
        if (!err || (err = snd_StreamSafeCdGetError()) != 0) {
            if (!err) {
                err = 48;
            }
            snd_SetCDSifReg(0, err);
            gLastLoadError = err;
            SignalSema(gFileReadMutex);
            return 0;
        }
    }

    sector += get_whole_sectors;
    bytes_needed -= get_whole_sectors * 2048;

    if (bytes_needed) {
        if (sector != gReadBufferHasSector) {
            err = snd_StreamSafeCdRead(sector, 1, gFileLoadBuffer);
            snd_StreamSafeCdSync(0);
            if (!err || (err = snd_StreamSafeCdGetError()) != 0) {
                gReadBufferHasSector = 0;
                if (!err) {
                    err = 48;
                }
                snd_SetCDSifReg(0, err);
                gLastLoadError = err;
                SignalSema(gFileReadMutex);
                return 0;
            }
            gReadBufferHasSector = sector;
        }

        memcpy(&buffer[size - bytes_needed], &gFileLoadBuffer, bytes_needed);
    }

    snd_SetCDSifReg(0, 0);
    SignalSema(gFileReadMutex);
    return 1;
}

SoundBankPtr snd_BankLoadByLocEx(SInt32 sect_loc, SInt32 file_offset,
                                 UInt32 spu_mem_loc, UInt32 spu_mem_size) {
    SoundBankPtr ret;

    if (!snd_OpenDataSourceByLoc(sect_loc, file_offset))
        return 0;

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

SoundBankPtr snd_BankLoadFromEEEx(UInt32 ee_loc, UInt32 spu_mem_loc,
                                  UInt32 spu_mem_size) {
    SoundBankPtr ret;

    if (!snd_OpenDataSourceFromEE(ee_loc))
        return 0;

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

UInt32 snd_GetSRAMUsedByBank(SoundBankPtr bank) {
    SFXBlock2Ptr block;

    block = (SFXBlock2Ptr)bank;

    if (bank->DataID == 0x6B6C4253) {
        return block->SRAMAllocSize;
    }

    return bank->VagDataSize;
}

SoundBankPtr snd_BankLoadFromIOPEx(void *iop_loc, UInt32 spu_mem_loc,
                                   UInt32 spu_mem_size) {
    SoundBankPtr ret;

    gLastLoadError = 0;
    ret = snd_ParseIOPBank(iop_loc, spu_mem_loc, spu_mem_size);

    if (ret && g989Monitor) {
        if (g989Monitor->bnk_load) {
            g989Monitor->bnk_load(ret, 1);
        }
    }

    return ret;
}

void snd_UnloadBank(SoundBankPtr bank) {
    SoundBankPtr mem_to_free;

    mem_to_free = bank;
    if (!bank || bank == (SoundBankPtr)-1) {
        snd_ShowError(20, 0, 0, 0, 0);
        return;
    }

    if (bank->DataID == 0x6B6C4253) {
        snd_UnloadBlock(bank);
        return;
    }

    if (!snd_BankLoaded(bank)) {
        snd_ShowError(21, 0, 0, 0, 0);
        return;
    }

    if (g989Monitor && g989Monitor->bnk_load) {
        g989Monitor->bnk_load(bank, 0);
    }

    if (bank->Flags & 8) {
        mem_to_free = (SoundBank *)((char *)bank - 4);
        snd_UnloadMMD((MultiMIDIBlockHeaderPtr)mem_to_free->DataID);
    }

    snd_RemoveBank(bank);
    gFreeProc(mem_to_free);
}

BOOL snd_BankLoaded(SoundBankPtr bank) {
    SoundBankPtr walker;

    walker = gBankListHead;
    if (!bank || bank == (SoundBankPtr)-1) {
        return false;
    }

    while (walker) {
        if (bank == walker) {
            return true;
        }
        walker = walker->NextBank;
    }

    return false;
}

void snd_UnloadBlock(SFXBlock2Ptr block) {
    if (!snd_BlockLoaded(block)) {
        snd_ShowError(22, 0, 0, 0, 0);
        return;
    }

    if (g989Monitor && g989Monitor->bnk_load) {
        g989Monitor->bnk_load(block, 0);
    }

    snd_RemoveBlock(block);
    gFreeProc(block);
}

BOOL snd_BlockLoaded(SFXBlock2Ptr block) {
    SFXBlock2Ptr walker;

    walker = gBlockListHead;
    if (!block || block == (SFXBlock2Ptr)-1) {
        return false;
    }

    while (walker) {
        if (block == walker) {
            return true;
        }

        walker = walker->NextBlock;
    }

    return false;
}

MultiMIDIBlockHeaderPtr snd_MMDLoad(SInt8 *name, SInt32 offset) {
    MultiMIDIBlockHeaderPtr ret;

    if (!snd_OpenDataSourceByName(name, offset)) {
        return 0;
    }

    ret = snd_MMDLoadFromDataSource();
    snd_CloseDataSource();

    return ret;
}

MultiMIDIBlockHeaderPtr snd_MMDLoadFromIOPLoc(SInt8 *iop_loc) {
    SInt32 length;
    MultiMIDIBlockHeaderPtr mmd;
    FileAttributesPtr fa;

    fa = (FileAttributesPtr)iop_loc;
    length = fa->where[0].size;
    mmd = gAllocProc(length, 3, 0);
    if (!mmd) {
        snd_ShowError(23, length, 0, 0, 0);
        gLastLoadError = 258;
        return 0;
    }

    memcpy(mmd, iop_loc + fa->where[0].offset, length);
    if (!snd_RegisterMIDI((MIDIBlockHeaderPtr)mmd)) {
        gFreeProc(mmd);
        snd_ShowError(121, 0, 0, 0, 0);
        gLastLoadError = 263;
        return 0;
    }

    return mmd;
}

MultiMIDIBlockHeaderPtr snd_MMDLoadFromDataSource() {
    SInt32 result;
    SInt32 length;
    MultiMIDIBlockHeaderPtr mmd;
    FileAttributesPtr fa;
    SInt8 buff[16];

    snd_SetDataSourceMark();
    result = snd_ReadBytes(buff, sizeof(buff));
    if (result != sizeof(buff)) {
        return NULL;
    }

    fa = (FileAttributesPtr)buff;
    length = fa->where[0].size;

    snd_SeekDataSource(fa->where[0].offset, 1);
    mmd = gAllocProc(length, 3, NULL);
    if (!mmd) {
        snd_ShowError(23, length, 0, 0, 0);
        gLastLoadError = 258;
        return NULL;
    }

    result = snd_ReadBytes(mmd, length);
    if (result != length) {
        gFreeProc(mmd);
        return NULL;
    }

    if (!snd_RegisterMIDI((MIDIBlockHeaderPtr)mmd)) {
        gFreeProc(mmd);
        snd_ShowError(121, 0, 0, 0, 0);
        gLastLoadError = 263;
        return 0;
    }

    return mmd;
}

MultiMIDIBlockHeaderPtr snd_MMDLoadByLoc(SInt32 sect_loc, SInt32 file_offset) {
    MultiMIDIBlockHeaderPtr ret;

    if (!snd_OpenDataSourceByLoc(sect_loc, file_offset)) {
        return NULL;
    }

    ret = snd_MMDLoadFromDataSource();
    snd_CloseDataSource();

    return ret;
}

void snd_UnloadMMD(MultiMIDIBlockHeaderPtr mmd) {
    snd_UnregisterMIDI(mmd);
    gFreeProc(mmd);
}

void snd_UnloadAllMMD() {
    MIDIBlockHeaderPtr walk;

    walk = gMIDIListHead;
    while (walk) {
        if (walk->DataID == 0x44494D4D) {
            snd_UnloadMMD(walk);
        }
        walk = walk->NextMIDIBlock;
    }

    while (gMIDIListHead) {
        snd_UnloadMMD(gMIDIListHead);
    }
}

SInt32 snd_BankTransfer(SoundBankPtr bank, SInt8 *data, UInt32 data_size,
                        SInt32 offset, SInt32 state, UInt32 spu_mem_loc,
                        UInt32 spu_mem_size, SpuTransferCallbackProc callback) {
    UInt32 size;
    SInt32 ch;
    SInt32 msg;
    void *sram_loc;
    UInt32 sram_size;
    SFXBlock2Ptr block;
    SInt32 dis;
    SInt32 oldstat;

    msg = 0;
    block = (SFXBlock2Ptr)bank;

    // BUG: checking function address instead of calling
    if (!snd_SystemRunning) {
        snd_ShowError(24, 0, 0, 0, 0);
        return -2;
    }

    if (gTransfering) {
        snd_ShowError(25, 0, 0, 0, 0);
        gLastLoadError = 260;
        return -10;
    }

    if (bank->DataID == 0x32764253) {
        if (spu_mem_size) {
            if (bank->VagDataSize > spu_mem_size) {
                snd_ShowError(105, spu_mem_size, bank->VagDataSize, 0, 0);
            }
            bank->VagDataSize = spu_mem_size;
        }
        if (spu_mem_loc) {
            bank->Flags &= ~4u;
            bank->VagsInSR = (void *)spu_mem_loc;
        }
        sram_size = bank->VagDataSize;
        sram_loc = (void *)bank->VagsInSR;
    } else {
        if (spu_mem_size) {
            if (block->VagDataSize > spu_mem_size) {
                snd_ShowError(105, spu_mem_size, block->VagDataSize, 0, 0);
            }
            block->SRAMAllocSize = spu_mem_size;
        }
        if (spu_mem_loc) {
            bank->Flags &= ~4u;
            block->VagsInSR = (void *)spu_mem_loc;
        }
        sram_size = block->SRAMAllocSize;
        sram_loc = block->VagsInSR;
    }

    if (!state) {
        if (bank->Flags & 4) {
            dis = CpuSuspendIntr(&oldstat);
            sram_loc = (void *)snd_SRAMMalloc(sram_size);
            if (!dis) {
                CpuResumeIntr(oldstat);
            }
            if (!sram_loc) {
                snd_ShowError(26, sram_size, 0, 0, 0);
                gLastLoadError = 259;
                return -11;
            }

            if (bank->DataID == 0x32764253) {
                bank->VagsInSR = sram_loc;
            } else {
                block->VagsInSR = sram_loc;
            }
        } else {
            dis = CpuSuspendIntr(&oldstat);
            if (!snd_SRAMMarkUsed(sram_loc, sram_size)) {
                if (!dis) {
                    CpuResumeIntr(oldstat);
                }
                snd_ShowError(27, sram_size, (UInt32)sram_loc, 0, 0);
                gLastLoadError = 259;
                return -11;
            }

            if (!dis) {
                CpuResumeIntr(oldstat);
            }
        }

        if ((bank->Flags & 1) == 0) {
            if (bank->DataID == 0x32764253) {
                bank->FirstSound =
                    (SoundPtr)((UInt32)bank->FirstSound + (UInt32)bank);
                bank->FirstProg =
                    (ProgPtr)((UInt32)bank->FirstProg + (UInt32)bank);
                bank->FirstTone =
                    (TonePtr)((UInt32)bank->FirstTone + (UInt32)bank);
                bank->Flags |= 1;
            } else {
                block->FirstSound =
                    (SFX2Ptr)((UInt32)block->FirstSound + (UInt32)block);
                block->FirstGrain =
                    (SFX2Ptr)((UInt32)block->FirstGrain + (UInt32)block);
                block->Flags |= 1;
            }
        }
    }

    gTransferDoneCallback = callback;
    while ((ch = snd_GetFreeSPUDMA()) == -1) {
        if (!msg) {
            msg = 1;
        }
        while (ch < 1000) {
            ++ch;
        }

        ch = 0;
    }
    gTransfering = ch + 1;
    snd_ClearTransSema();
    sceSdSetTransIntrHandler(ch, snd_TransCallback, NULL);
    size = sceSdVoiceTrans(ch, 0, data, sram_loc + offset, data_size);
    if (size < data_size) {
        snd_FreeSPUDMA(gTransfering - 1);
        gTransfering = 0;
        gLastLoadError = 261;
        return -12;
    }

    return 0;
}

void snd_ClearTransSema() {
    while (PollSema(gSPURAMTransSema) != KE_SEMA_ZERO)
        ;
}

SInt32 snd_IsCurrentTransferComplete(BOOL wait) {
    if (wait) {
        WaitSema(gSPURAMTransSema);
    }

    if (gTransfering) {
        return 0;
    }

    return 1;
}

SInt32 snd_EndBankTransfer(SoundBankPtr bank) {
    SInt32 x;
    SFXBlock2Ptr block;

    block = (SFXBlock2Ptr)bank;

    if (bank->DataID == 0x32764253) {
        SoundBankPtr walker;
        for (x = 0; x < bank->NumTones; ++x) {
            bank->FirstTone[x].VAGInSR =
                (void *)((UInt32)bank->FirstTone[x].VAGInSR +
                         (UInt32)bank->VagsInSR);
        }
        for (x = 0; x < bank->NumProgs; ++x) {
            bank->FirstProg[x].FirstTone =
                (TonePtr)((UInt32)bank + (UInt32)bank->FirstProg[x].FirstTone);
        }

        if (!gBankListHead) {
            gBankListHead = bank;
        } else {
            walker = gBankListHead;
            while (walker->NextBank) {
                walker = walker->NextBank;
            }
            walker->NextBank = bank;
        }
    } else {
        block->GrainData = (char *)((UInt32)block->GrainData + (UInt32)block);
        for (x = 0; x < block->NumGrains; ++x) {
            TonePtr tp;
            if (block->FirstGrain[x].OpcodeData.MicroOp.Type == 1) {
                tp = (TonePtr)((char *)block->GrainData +
                               (block->FirstGrain[x].OpcodeData.Opcode &
                                0xFFFFFF));
                tp->VAGInSR = (void *)(tp->VAGInSR + (UInt32)block->VagsInSR);
            }
        }
        for (x = 0; x < block->NumSounds; ++x) {
            block->FirstSound[x].FirstGrain =
                (SFXGrain2Ptr)((UInt32)block->FirstGrain +
                               (UInt32)block->FirstSound[x].FirstGrain);
            block->FirstSound[x].Flags &= ~0x4000u;
        }

        if ((block->Flags & 0x100) != 0) {
            block->BlockNames =
                (SFXBlockNames *)((UInt32)block->BlockNames + (UInt32)block);
            block->BlockNames->SFXNameTableOffset +=
                (UInt32)block->BlockNames->BlockName;
            block->BlockNames->VAGNameTableOffset +=
                (UInt32)block->BlockNames->BlockName;
            block->BlockNames->VAGImportsTableOffset +=
                (UInt32)block->BlockNames->BlockName;
            block->BlockNames->VAGExportsTableOffset +=
                (UInt32)block->BlockNames->BlockName;
        }
        if ((block->Flags & 0x200) != 0) {
            block->SFXUD =
                (SFXUserData *)((UInt32)block + (UInt32)block->SFXUD);
        }

        if (!gBlockListHead) {
            gBlockListHead = block;
        } else {
            SFXBlock2Ptr walker;
            walker = gBlockListHead;
            while (walker->NextBlock) {
                walker = walker->NextBlock;
            }
            walker->NextBlock = block;
        }
    }

    return 0;
}

void snd_RemoveBank(SoundBankPtr bank) {
    SoundBankPtr walker;
    SoundBankPtr prev;
    SInt32 dis;
    SInt32 oldstat;

    walker = gBankListHead;
    prev = NULL;
    if (bank->DataID == 0x6B6C4253) {
        snd_RemoveBlock((SFXBlock2Ptr)bank);
        return;
    }

    if (!bank) {
        return;
    }

    while (walker && walker != bank) {
        prev = walker;
        walker = walker->NextBank;
    }

    if (walker == bank) {
        snd_StopAllSoundsInBank(bank, 1);
        if (prev) {
            prev->NextBank = walker->NextBank;
        } else {
            gBankListHead = walker->NextBank;
        }

        dis = CpuSuspendIntr(&oldstat);
        if (bank->VagDataSize) {
            snd_SRAMFree(bank->VagsInSR, bank->VagDataSize);
        }
        if (!dis) {
            CpuResumeIntr(oldstat);
        }
    }
}

void snd_RemoveBlock(SFXBlock2Ptr block) {
    SFXBlock2Ptr walker;
    SFXBlock2Ptr prev;
    SInt32 dis;
    SInt32 oldstat;

    walker = gBlockListHead;
    prev = NULL;

    if (!block) {
        return;
    }

    while (walker && walker != block) {
        prev = walker;
        walker = walker->NextBlock;
    }

    if (walker == block) {
        snd_StopAllSoundsInBank(block, 1);
        if (prev) {
            prev->NextBlock = walker->NextBlock;
        } else {
            gBlockListHead = walker->NextBlock;
        }

        dis = CpuSuspendIntr(&oldstat);
        if (block->SRAMAllocSize) {
            snd_SRAMFree(block->VagsInSR, block->SRAMAllocSize);
        }
        if (!dis) {
            CpuResumeIntr(oldstat);
        }
    }
}

void snd_UnloadAllBanks() {
    while (gBankListHead) {
        snd_UnloadBank(gBankListHead);
    }

    while (gBlockListHead) {
        snd_UnloadBlock(gBlockListHead);
    }
}

void snd_ResolveBankXREFS() {
    SoundBankPtr bank;
    SFXBlock2Ptr block;
    SInt32 x;
    MIDISoundPtr midisound;

    bank = gBankListHead;
    while (bank) {
        for (x = 0; x < bank->NumSounds; ++x) {
            bank->FirstSound[x].Bank = bank;
            if (bank->FirstSound[x].Type == 4 ||
                bank->FirstSound[x].Type == 5) {
                midisound = (MIDISound *)&bank->FirstSound[x];
                midisound->MIDIBlock =
                    (MultiMIDIBlockHeaderPtr)snd_FindMIDIBlock(
                        midisound->MIDIID);
                if (!midisound->MIDIBlock)
                    snd_ShowError(90, midisound->MIDIID, 0, 0, 0);
            }
        }

        bank->Flags |= 2u;
        bank = bank->NextBank;
    }

    block = gBlockListHead;
    while (block) {
        block->Flags |= 2u;
        block = block->NextBlock;
    }
}

SoundBankPtr snd_FindBankByID(UInt32 id) {
    SoundBankPtr bank;

    bank = gBankListHead;
    while (bank) {
        if (bank->BankID == id) {
            return bank;
        }

        bank = bank->NextBank;
    }

    if (!bank) {
        return snd_FindBlockByID(id);
    }

    return bank;
}

SFXBlock2Ptr snd_FindBlockByName(UInt32 *name) {
    SFXBlock2Ptr block;
    SFXBlockNames *names_header;
    UInt32 *buffer;

    block = gBlockListHead;
    buffer = name;
    while (block) {
        if ((block->Flags & 0x100) != 0) {
            names_header = block->BlockNames;
            if (names_header->BlockName[0] == buffer[0] &&
                names_header->BlockName[1] == buffer[1])
                return block;
        }

        block = block->NextBlock;
    }

    return block;
}

SFXBlock2Ptr snd_FindBlockByID(UInt32 id) {
    SFXBlock2Ptr block;

    block = gBlockListHead;
    while (block) {
        if (block->BlockID == id) {
            return block;
        }
        block = block->NextBlock;
    }

    return block;
}

SoundBankPtr snd_FindBankByNum(SInt8 num) {
    SoundBankPtr bank;

    bank = gBankListHead;
    while (bank) {
        if (bank->BankNum == num) {
            return bank;
        }

        bank = bank->NextBank;
    }

    if (!bank) {
        return snd_FindBlockByNum(num);
    }

    return bank;
}

SFXBlock2Ptr snd_FindBlockByNum(SInt8 num) {
    SFXBlock2Ptr block;

    block = gBlockListHead;
    while (block) {
        if (block->BlockNum == num) {
            return block;
        }

        block = block->NextBlock;
    }

    return block;
}

SInt32 snd_RegisterMIDI(MIDIBlockHeaderPtr midi) {
    MultiMIDIBlockHeaderPtr mmid;
    MIDIBlockHeaderPtr hold;
    SInt32 x;

    switch (midi->DataID) {
    case 0x2044494D:
        midi->BankPtr = (SoundBankPtr)snd_FindBankByID(midi->BankID);
        if (!midi->BankPtr) {
            snd_ShowError(91, midi->BankID, 0, 0, 0);
        }
        midi->DataStart += (UInt32)midi;
        midi->NextMIDIBlock = 0;
        midi->MultiMIDIParent = 0;
        snd_InsertMIDIBlockInList(midi);
        break;
    case 0x44494D4D:
        mmid = (MultiMIDIBlockHeaderPtr)midi;
        mmid->NextMIDIBlock = NULL;
        if (mmid->Version < 2) {
            return 0;
        }
        snd_InsertMIDIBlockInList(mmid);
        for (x = 0; x < mmid->NumMIDIBlocks; x++) {
            mmid->BlockPtr[x] =
                (SInt8 *)((UInt32)mmid->BlockPtr[x] + (UInt32)mmid);
            hold = mmid->BlockPtr[x];
            snd_RegisterMIDI(hold);
            hold->MultiMIDIParent = mmid;
        }
        break;
    }

    return 1;
}

void snd_UnregisterMIDI(MIDIBlockHeaderPtr midi) {
    MultiMIDIBlockHeaderPtr mmid;
    MIDIBlockHeaderPtr hold;
    SInt32 x;
    MIDIBlockHeaderPtr walk;
    static SInt32 working_mmd;

    if (!working_mmd) {
        snd_MIDIStreamGone(midi);
    }

    if (midi->DataID == 0x44494D4D) {
        mmid = (MultiMIDIBlockHeaderPtr)midi;
        working_mmd = 1;

        for (x = 0; x < mmid->NumMIDIBlocks; ++x) {
            hold = (MIDIBlockHeaderPtr)mmid->BlockPtr[x];
            snd_UnregisterMIDI(hold);
        }
        working_mmd = 0;
    }

    if (!gMIDIListHead) {
        return;
    }

    if (gMIDIListHead == midi) {
        gMIDIListHead = midi->NextMIDIBlock;
    } else {
        walk = gMIDIListHead;
        while (walk->NextMIDIBlock != midi) {
            walk = walk->NextMIDIBlock;
        }

        if (walk->NextMIDIBlock != midi) {
            return;
        }

        walk->NextMIDIBlock = midi->NextMIDIBlock;
    }
}

MIDIBlockHeaderPtr snd_FindMIDIBlock(UInt32 id) {
    MIDIBlockHeaderPtr walk;

    walk = gMIDIListHead;
    while (walk && walk->ID != id) {
        walk = walk->NextMIDIBlock;
    }

    return walk;
}

void snd_InsertMIDIBlockInList(MIDIBlockHeaderPtr midi) {
    MIDIBlockHeaderPtr walk;

    if (!gMIDIListHead) {
        gMIDIListHead = midi;
    } else {
        walk = gMIDIListHead;
        while (walk->NextMIDIBlock) {
            walk = walk->NextMIDIBlock;
        }

        walk->NextMIDIBlock = midi;
    }
}

void snd_MIDIStreamGone(MIDIBlockHeaderPtr midi) {
    SoundBankPtr bank;
    SInt32 x;
    MIDISoundPtr midisound;

    bank = gBankListHead;
    while (bank) {
        for (x = 0; x < bank->NumSounds; ++x) {
            if (bank->FirstSound[x].Type == 4 ||
                bank->FirstSound[x].Type == 5) {

                midisound = (MIDISoundPtr)&bank->FirstSound[x];
                if (midisound->MIDIBlock == midi) {
                    snd_LockMasterTick(512);
                    snd_StopAllHandlersForSound(
                        (MIDISound *)&bank->FirstSound[x], 0, 0);
                    snd_UnlockMasterTick();
                    midisound->MIDIBlock = 0;
                }
            }
        }

        bank = bank->NextBank;
    }
}

void snd_SetCDSifReg(SInt32 reg8, SInt32 reg9) {
    SInt32 did;
    SInt32 dis;
    SInt32 intr_state;
    sceSifDmaData transData;

    gStats.cd_busy = reg8;
    gStats.cd_error = reg9;

    if (!gEEStatusAddr) {
        return;
    }
    transData.size = 16;
    transData.mode = 0;

    if (gStats.cd_busy) {
        transData.data = (UInt32)&gStats;
        transData.addr = (UInt32)gEEStatusAddr;
        dis = CpuSuspendIntr(&intr_state);

        did = sceSifSetDmaIntr(&transData, 1, snd_EEDMADone, &gEEDMADoneSema);

        if (!dis) {
            CpuResumeIntr(intr_state);
        }

        while (sceSifDmaStat(did) > -1) {
            WaitSema(gEEDMADoneSema);
        }
    }

    transData.data = (UInt32)&gStats.cd_error;
    transData.addr = (UInt32)(gEEStatusAddr + 16);
    dis = CpuSuspendIntr(&intr_state);
    did = sceSifSetDmaIntr(&transData, 1, snd_EEDMADone, &gEEDMADoneSema);
    if (!dis) {
        CpuResumeIntr(intr_state);
    }
    while (sceSifDmaStat(did) > -1) {
        WaitSema(gEEDMADoneSema);
    }

    if (!reg8) {
        transData.data = (UInt32)&gStats;
        transData.addr = (UInt32)gEEStatusAddr;
        dis = CpuSuspendIntr(&intr_state);
        did = sceSifSetDmaIntr(&transData, 1, snd_EEDMADone, &gEEDMADoneSema);
        if (!dis) {
            CpuResumeIntr(intr_state);
        }
        while (sceSifDmaStat(did) > -1) {
            WaitSema(gEEDMADoneSema);
        }
    }
}

void *snd_IOPMemAlloc(SInt32 size, SInt32 use, SInt32 *act_size) {
    void *mem;
    SInt32 max_avail;
    SInt32 from_where;
    SInt32 dis;
    SInt32 oldstat;

    mem = NULL;
    from_where = 0;

    dis = CpuSuspendIntr(&oldstat);

    if (use == 8) {
        max_avail = QueryMaxFreeMemSize();
        if (gLimit2Meg && max_avail > 0x200000) {
            if (!gPrefs_Silent) {
                printf("989snd: detected more then 2 meg free (%d free).\n",
                       max_avail);
            }
            max_avail -= 0x600000;
            if (max_avail < 0) {
                max_avail = 0;
            }
            if (!gPrefs_Silent) {
                printf("        subtracting 6 meg (%d)\n", 0x600000);
                printf("        max avail then is %d\n", max_avail);
            }
            if (!max_avail) {
                max_avail = 10240;
                if (!gPrefs_Silent) {
                    printf("        allowing a 10k buffer!\n");
                }
            }
        }

        if (max_avail % 16) {
            max_avail = max_avail - (max_avail % 16);
        }

        if (max_avail < size) {
            size = max_avail;
        }

        if (!size) {
            if (!dis) {
                CpuResumeIntr(oldstat);
            }

            return NULL;
        }
    }

    switch (use) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 7:
        from_where = 1;
        break;

    case 8:
    default:
        from_where = 0;
        break;
    }

    mem = AllocSysMemory(from_where, size, 0);

    if (mem && act_size)
        *act_size = size;

    if (!dis) {
        CpuResumeIntr(oldstat);
    }

    return mem;
}

void snd_IOPMemFree(void *mem) {
    SInt32 dis;
    SInt32 oldstat;

    dis = CpuSuspendIntr(&oldstat);

    FreeSysMemory(mem);

    if (!dis) {
        CpuResumeIntr(oldstat);
    }
}

SInt32 snd_GetFreeSPUDMA() {
    SInt32 intr_state;
    SInt32 dis;

    dis = CpuSuspendIntr(&intr_state);

    if (!gDMAInUse[0]) {
        gDMAInUse[0] = 1;

        if (!dis) {
            CpuResumeIntr(intr_state);
        }

        sceSdSetTransIntrHandler(0, NULL, NULL);
        return 0;
    }

    if (!gDMAInUse[1]) {
        gDMAInUse[1] = 1;

        if (!dis) {
            CpuResumeIntr(intr_state);
        }

        sceSdSetTransIntrHandler(1, NULL, NULL);
        return 1;
    }

    if (!dis) {
        CpuResumeIntr(intr_state);
    }

    return -1;
}

void snd_FreeSPUDMA(SInt32 ch) {
    gDMAInUse[ch] = 0;
    if (VAGStreamDMAList) {
        SignalSema(gStartDMASema);
    }
}

void snd_WaitDMADone() {
    while (gDMAInUse[0] || gDMAInUse[1]) {
        gWaitingDMAComplete = GetThreadId();
        SleepThread();
    }

    gWaitingDMAComplete = 0;
}
