#include "common.h"
#include "functions.h"

/* data 1c24 */ sSRAMNodePtr gSRAMFreeListHead = NULL;
/* data 1c28 */ SInt32 gSRAMFreeNodesInUse = 0;
/* bss 80e0 */ sSRAMNode gSRAMFreeNodeStorage[129];

void snd_SRAMInitMalloc() {
    SInt32 i;

    gSRAMFreeListHead = NULL;
    for (i = 0; i < 129; ++i) {
        gSRAMFreeNodeStorage[i].in_use = 0;
    }

    snd_SRAMFree(0x5040, 0x1FAFBFu);
}

#ifndef NON_MATCHING
INCLUDE_ASM("asm/nonmatchings/sram", snd_SRAMMalloc);
#elif
UInt32 snd_SRAMMalloc(UInt32 size) {
    sSRAMNodePtr p;
    sSRAMNodePtr candidate;
    UInt32 ret_loc;

    p = gSRAMFreeListHead;
    ret_loc = 0;

    if (!size) {
        return 0;
    }

    if (size % 64) {
        size = (size + 64) - (size % 64);
    }

    candidate = snd_SRAMSearch(p, size);
    if (!candidate) {
        snd_ShowError(32, size, 0, 0, 0);
        return 0;
    }

    while ((p = snd_SRAMSearch(candidate->smaller, size))) {
        candidate = p;
    }

    ret_loc = candidate->loc;
    snd_SRAMRemoveNode(candidate);
    candidate->size -= size;
    if (candidate->size) {
        candidate->loc += size;
        snd_SRAMAddNode(candidate);
    }

    return ret_loc;
}
#endif

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
