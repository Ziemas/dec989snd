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

#ifndef NON_MATCHING
INCLUDE_ASM("asm/nonmatchings/loader", snd_ReadBytesFromEE);
#else
SInt32 snd_ReadBytesFromEE(UInt32 ee_loc, void *iop_loc, SInt32 bytes) {
    SInt32 ret;
    sceSifReceiveData data_track;
    SInt32 full_blocks;
    SInt32 get_blocks;
    SInt32 read_bytes;
    SInt32 part_size;

    read_bytes = bytes;
    if ((ee_loc & 0xf)) {
        part_size = 16 - (ee_loc & 0xf);
        ee_loc -= ee_loc & 0xf;
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
#endif

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

INCLUDE_ASM("asm/nonmatchings/loader", snd_ParseIOPBank);

INCLUDE_ASM("asm/nonmatchings/loader", snd_ReadBank);

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
