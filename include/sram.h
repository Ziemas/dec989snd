#ifndef SRAM_H_
#define SRAM_H_

#include "common.h"

void snd_SRAMInitMalloc();
UInt32 snd_SRAMMalloc(UInt32 size);
UInt32 snd_SRAMMallocRev(UInt32 size);
void snd_SRAMFree(UInt32 loc, UInt32 size);
UInt32 snd_SRAMMarkUsed(UInt32 loc, UInt32 size);
void snd_SRAMRemoveNode(sSRAMNodePtr node);
void snd_SRAMAddNode(sSRAMNodePtr node);
sSRAMNodePtr snd_SRAMSearch(sSRAMNodePtr root, UInt32 size);
SInt32 snd_SRAMGetFreeMem();
SInt32 snd_SRAMMaxFreeBlockSize();
void snd_SRAMDumpFreeBlocks();
SInt32 snd_GetFreeBlockSize(SInt32 start, UInt32 *loc);

#endif // SRAM_H_
