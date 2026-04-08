#ifndef LOADER_H_
#define LOADER_H_

#include "common.h"

extern BOOL gLimit2Meg;
extern char *gEEStatusAddr;
extern ExternSndIOPAlloc gAllocProc;
extern ExternSndIOPFree gFreeProc;
extern SoundBankPtr gBankListHead;
extern SFXBlock2Ptr gBlockListHead;
extern SInt8 gFileLoadBuffer[2048];

void snd_InitLoader();
void snd_ShutdownLoader();
void snd_RegisterIOPMemAllocator(ExternSndIOPAlloc AllocProc, ExternSndIOPFree FreeProc);
BOOL snd_OpenDataSourceByName(char *name, SInt32 offset);
BOOL snd_OpenDataSourceByLoc(SInt32 sector, SInt32 offset);
BOOL snd_OpenDataSourceFromEE(UInt32 ee_loc);
void snd_CloseDataSource();
SInt32 snd_SeekDataSource(SInt32 where, SInt32 from_where);
void snd_SetDataSourceMark();
SInt32 snd_GetLastLoadError();
SInt32 snd_ReadBytesFromEE(UInt32 ee_loc, void *iop_loc, SInt32 bytes);
SInt32 snd_ReadBytes(void *buffer, SInt32 count);
SoundBankPtr snd_BankLoadEx(SInt8 *name, SInt32 offset, UInt32 spu_mem_loc, UInt32 spu_mem_size);
void snd_IterateBlockListToMonitor();
SoundBankPtr snd_ParseIOPBank(SInt8 *iop_loc, UInt32 spu_mem_loc, UInt32 spu_mem_size);
SoundBankPtr snd_ReadBank(UInt32 spu_mem_loc, UInt32 spu_mem_size);
SInt32 snd_FileRead(SInt32 sect_loc, SInt32 offset, SInt32 size, SInt8 *buffer);
SoundBankPtr snd_BankLoadByLocEx(SInt32 sect_loc, SInt32 file_offset, UInt32 spu_mem_loc, UInt32 spu_mem_size);
SoundBankPtr snd_BankLoadFromEEEx(UInt32 ee_loc, UInt32 spu_mem_loc, UInt32 spu_mem_size);
UInt32 snd_GetSRAMUsedByBank(SoundBankPtr bank);
SoundBankPtr snd_BankLoadFromIOPEx(void *iop_loc, UInt32 spu_mem_loc, UInt32 spu_mem_size);
void snd_UnloadBank(SoundBankPtr bank);
BOOL snd_BankLoaded(SoundBankPtr bank);
void snd_UnloadBlock(SFXBlock2Ptr block);
BOOL snd_BlockLoaded(SFXBlock2Ptr block);
MultiMIDIBlockHeaderPtr snd_MMDLoad(SInt8 *name, SInt32 offset);
MultiMIDIBlockHeaderPtr snd_MMDLoadFromIOPLoc(SInt8 *iop_loc);
MultiMIDIBlockHeaderPtr snd_MMDLoadFromDataSource();
MultiMIDIBlockHeaderPtr snd_MMDLoadByLoc(SInt32 sect_loc, SInt32 file_offset);
void snd_UnloadMMD(MultiMIDIBlockHeaderPtr mmd);
void snd_UnloadAllMMD();
SInt32 snd_BankTransfer(SoundBankPtr bank, SInt8 *data, UInt32 data_size, SInt32 offset, SInt32 state,
                        UInt32 spu_mem_loc, UInt32 spu_mem_size, SpuTransferCallbackProc callback);
void snd_ClearTransSema();
SInt32 snd_IsCurrentTransferComplete(BOOL wait);
SInt32 snd_EndBankTransfer(SoundBankPtr bank);
void snd_RemoveBank(SoundBankPtr bank);
void snd_RemoveBlock(SFXBlock2Ptr block);
void snd_UnloadAllBanks();
void snd_ResolveBankXREFS();
SoundBankPtr snd_FindBankByID(UInt32 id);
SFXBlock2Ptr snd_FindBlockByName(UInt32 *name);
SFXBlock2Ptr snd_FindBlockByID(UInt32 id);
SoundBankPtr snd_FindBankByNum(SInt8 num);
SFXBlock2Ptr snd_FindBlockByNum(SInt8 num);
SInt32 snd_RegisterMIDI(MIDIBlockHeaderPtr midi);
void snd_UnregisterMIDI(MIDIBlockHeaderPtr midi);
MIDIBlockHeaderPtr snd_FindMIDIBlock(UInt32 id);
void snd_InsertMIDIBlockInList(MIDIBlockHeaderPtr midi);
void snd_MIDIStreamGone(MIDIBlockHeaderPtr midi);
void snd_SetCDSifReg(SInt32 reg8, SInt32 reg9);
void *snd_IOPMemAlloc(SInt32 size, SInt32 use, SInt32 *act_size);
void snd_IOPMemFree(void *mem);
SInt32 snd_GetFreeSPUDMA();
void snd_FreeSPUDMA(SInt32 ch);
void snd_WaitDMADone();

#endif // LOADER_H_
