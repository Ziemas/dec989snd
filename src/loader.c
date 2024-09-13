#include "common.h"

INCLUDE_ASM("asm/nonmatchings/loader", snd_InitLoader);

INCLUDE_ASM("asm/nonmatchings/loader", snd_ShutdownLoader);

INCLUDE_ASM("asm/nonmatchings/loader", snd_RegisterIOPMemAllocator);

INCLUDE_ASM("asm/nonmatchings/loader", snd_OpenDataSourceByName);

INCLUDE_ASM("asm/nonmatchings/loader", snd_OpenDataSourceByLoc);

INCLUDE_ASM("asm/nonmatchings/loader", snd_OpenDataSourceFromEE);

INCLUDE_ASM("asm/nonmatchings/loader", snd_CloseDataSource);

INCLUDE_ASM("asm/nonmatchings/loader", snd_SeekDataSource);

INCLUDE_ASM("asm/nonmatchings/loader", snd_SetDataSourceMark);

INCLUDE_ASM("asm/nonmatchings/loader", snd_GetLastLoadError);

INCLUDE_ASM("asm/nonmatchings/loader", snd_ReadBytesFromEE);

INCLUDE_ASM("asm/nonmatchings/loader", snd_ReadBytes);

INCLUDE_ASM("asm/nonmatchings/loader", snd_BankLoadEx);

INCLUDE_ASM("asm/nonmatchings/loader", snd_IterateBlockListToMonitor);

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
