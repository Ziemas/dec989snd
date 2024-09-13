#include "common.h"

INCLUDE_ASM("asm/nonmatchings/sram", snd_SRAMInitMalloc);

INCLUDE_ASM("asm/nonmatchings/sram", snd_SRAMMalloc);

INCLUDE_ASM("asm/nonmatchings/sram", snd_SRAMMallocRev);

INCLUDE_ASM("asm/nonmatchings/sram", snd_SRAMFree);

INCLUDE_ASM("asm/nonmatchings/sram", snd_SRAMMarkUsed);

INCLUDE_ASM("asm/nonmatchings/sram", snd_SRAMRemoveNode);

INCLUDE_ASM("asm/nonmatchings/sram", snd_SRAMAddNode);

INCLUDE_ASM("asm/nonmatchings/sram", snd_SRAMSearch);

INCLUDE_ASM("asm/nonmatchings/sram", snd_SRAMGetFreeMem);

INCLUDE_ASM("asm/nonmatchings/sram", snd_SRAMMaxFreeBlockSize);

INCLUDE_ASM("asm/nonmatchings/sram", snd_SRAMDumpFreeBlocks);

INCLUDE_ASM("asm/nonmatchings/sram", snd_GetFreeBlockSize);
