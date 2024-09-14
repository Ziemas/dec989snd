#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#include "common.h"

// 989snd.c
Extern989HandlerPtr snd_FindExternProcHandler(UInt32 id);
void snd_ShowError(SInt32 num, UInt32 a1, UInt32 a2, UInt32 a3, UInt32 a4);

// init.c
SInt32 snd_StartSoundSystemEx(UInt32 flags);
SInt32 snd_StopSoundSystem();

// loader.c
SoundBankPtr snd_BankLoadEx(SInt8 *name, SInt32 offset, UInt32 spu_mem_loc, UInt32 spu_mem_size);
SoundBankPtr snd_BankLoadByLocEx( SInt32 sect_loc, SInt32 file_offset, UInt32 spu_mem_loc, UInt32 spu_mem_size);
SoundBankPtr snd_BankLoadFromEEEx(UInt32 ee_loc, UInt32 spu_mem_loc, UInt32 spu_mem_size);
SoundBankPtr snd_BankLoadFromIOPEx(void *iop_loc, UInt32 spu_mem_loc, UInt32 spu_mem_size);

#endif