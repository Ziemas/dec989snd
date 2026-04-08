#ifndef SRAM_H_
#define SRAM_H_

#include "common.h"

struct sSRAMNode_tag { // 0x18
    /* 0x00 */ UInt32 loc;
    /* 0x04 */ UInt32 size;
    /* 0x08 */ UInt32 in_use;
    /* 0x0c */ struct sSRAMNode_tag *root;
    /* 0x10 */ struct sSRAMNode_tag *smaller;
    /* 0x14 */ struct sSRAMNode_tag *bigger;
};

typedef struct sSRAMNode_tag sSRAMNode;
typedef struct sSRAMNode_tag *sSRAMNodePtr;

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
