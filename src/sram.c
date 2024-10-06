#include "common.h"
#include "functions.h"
#include "stdio.h"

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
        size = size + (64 - (size % 64));
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

UInt32 snd_SRAMMallocRev(UInt32 size) {
    SInt32 i;
    sSRAMNodePtr candidate;
    sSRAMNodePtr storage;
    UInt32 loc;
    UInt32 end;

    candidate = NULL;
    storage = gSRAMFreeNodeStorage;

    if (size % 64) {
        size = size + (64 - (size % 64));
    }

    for (i = 0; i < 129; i++) {
        if (storage[i].in_use) {
            if (storage[i].size >= size) {
                end = storage[i].loc + storage[i].size;
                if (end - (end % 0x20000) > storage[i].loc &&
                    end - (end % 0x20000) - storage[i].loc >= size) {
                    if (candidate) {
                        if (candidate->size > storage[i].size) {
                            candidate = &storage[i];
                        }
                    } else {
                        candidate = &storage[i];
                    }
                }
            }
        }
    }

    if (!candidate) {
        if (snd_SRAMGetFreeMem() >= size) {
            snd_ShowError(88, size, 0, 0, 0);
        }

        return 0;
    }

    end = candidate->loc + candidate->size;
    loc = end - ((end) & 0x1FFFF) - size;
    return snd_SRAMMarkUsed(loc, size);
}

void snd_SRAMFree(UInt32 loc, UInt32 size) {
    SInt32 i;
    SInt32 free_node;
    sSRAMNodePtr storage;

    free_node = 129;
    storage = gSRAMFreeNodeStorage;

    if (!size) {
        snd_ShowError(93, loc, 0, 0, 0);
        return;
    }

    if (size % 64) {
        size = size + (64 - (size % 64));
    }

    for (i = 0; i < 129; ++i) {
        if (storage[i].in_use) {
            if (storage[i].loc + storage[i].size == loc) {
                snd_SRAMRemoveNode(&storage[i]);
                size += storage[i].size;
                loc = storage[i].loc;
                if (i < free_node) {
                    free_node = i;
                }
            } else if (storage[i].loc == loc + size) {
                snd_SRAMRemoveNode(&storage[i]);
                size += storage[i].size;
                if (i < free_node) {
                    free_node = i;
                }
            }
        } else if (i < free_node) {
            free_node = i;
        }
    }

    if (free_node < 129) {
        storage[free_node].size = size;
        storage[free_node].loc = loc;
        storage[free_node].root = 0;
        storage[free_node].smaller = 0;
        storage[free_node].bigger = 0;
        snd_SRAMAddNode(&storage[free_node]);
    } else {
        snd_ShowError(33, 0, 0, 0, 0);
    }
}

UInt32 snd_SRAMMarkUsed(UInt32 loc, UInt32 size) {
    SInt32 i;
    SInt32 nn_size;
    SInt32 nn_loc;
    SInt32 free_node;
    sSRAMNodePtr storage;

    nn_size = 0;
    nn_loc = 0;
    free_node = 129;
    storage = gSRAMFreeNodeStorage;

    if (size % 64) {
        size = size + (64 - (size % 64));
    }

    for (i = 0; i < 129; i++) {
        if (storage[i].in_use) {
            if (loc >= storage[i].loc &&
                loc < storage[i].loc + storage[i].size) {
                snd_SRAMRemoveNode(&storage[i]);

                if (loc == storage[i].loc) {
                    storage[i].loc = loc + size;
                    storage[i].size -= size;
                    if (storage[i].size) {
                        snd_SRAMAddNode(&storage[i]);
                    } else {
                        if (0) {
                            snd_ShowError(34, 0, 0, 0, 0);
                        }
                    }

                    return loc;

                } else {
                    if (loc + size == storage[i].loc + storage[i].size) {
                        storage[i].size -= size;
                        if (storage[i].size) {
                            snd_SRAMAddNode(&storage[i]);
                        } else {
                            if (0) {
                                snd_ShowError(34, 0, 0, 0, 0);
                            }
                        }
                        return loc;
                    }
                }

                nn_size = storage[i].size;
                storage[i].size = loc - storage[i].loc;
                snd_SRAMAddNode(&storage[i]);
                nn_size = nn_size - storage[i].size;
                nn_size -= size;
                nn_loc = loc + size;
            }
        } else {
            if (i < free_node) {
                free_node = i;
            }
        }
    }

    if (nn_size && free_node < 129) {
        if (nn_size < 0) {
            snd_ShowError(34, 0, 0, 0, 0);
        } else if (nn_size > 0) {
            storage[free_node].size = nn_size;
            storage[free_node].loc = nn_loc;
            storage[free_node].root = 0;
            storage[free_node].smaller = 0;
            storage[free_node].bigger = 0;
            snd_SRAMAddNode(&storage[free_node]);
        }

        return loc;
    }

    snd_ShowError(35, 0, 0, 0, 0);
    return 0;
}

void snd_SRAMRemoveNode(sSRAMNodePtr node) {
    if (!node) {
        return;
    }

    if (node == gSRAMFreeListHead) {
        gSRAMFreeListHead = node->bigger;
    } else {
        if (node->root->smaller == node) {
            node->root->smaller = node->bigger;
        } else {
            node->root->bigger = node->bigger;
        }
    }

    if (node->bigger) {
        node->bigger->root = node->root;
    }

    snd_SRAMAddNode(node->smaller);

    node->in_use = 0;
    node->smaller = 0;
    node->bigger = 0;
    gSRAMFreeNodesInUse--;
}

void snd_SRAMAddNode(sSRAMNodePtr node) {
    sSRAMNodePtr work;

    if (!node) {
        return;
    }

    node->in_use = 1;
    gSRAMFreeNodesInUse++;
    work = gSRAMFreeListHead;

    if (!work) {
        gSRAMFreeListHead = node;
        node->root = NULL;
        return;
    }

    while (1) {
        if (node->size < work->size) {
            if (!work->smaller) {
                work->smaller = node;
                node->root = work;
                return;
            }

            work = work->smaller;
        } else {
            if (!work->bigger) {
                work->bigger = node;
                node->root = work;
                return;
            }

            work = work->bigger;
        }
    }
}

sSRAMNodePtr snd_SRAMSearch(sSRAMNodePtr root, UInt32 size) {
    if (!root) {
        return 0;
    }

    if (root->size >= size) {
        return root;
    }

    return snd_SRAMSearch(root->bigger, size);
}

SInt32 snd_SRAMGetFreeMem() {
    SInt32 i;
    sSRAMNodePtr storage;
    SInt32 free_size;

    storage = gSRAMFreeNodeStorage;
    free_size = 0;

    for (i = 0; i < 129; ++i) {
        if (storage[i].in_use) {
            free_size += storage[i].size;
        }
    }

    return free_size;
}

SInt32 snd_SRAMMaxFreeBlockSize() {
    SInt32 i;
    sSRAMNodePtr storage;
    SInt32 largest;

    storage = gSRAMFreeNodeStorage;
    largest = 0;

    for (i = 0; i < 129; ++i) {
        if (storage[i].in_use) {
            if (storage[i].size > largest)
                largest = storage[i].size;
        }
    }

    return largest;
}

void snd_SRAMDumpFreeBlocks() {
    SInt32 i;
    sSRAMNodePtr storage;

    storage = gSRAMFreeNodeStorage;

    for (i = 0; i < 129; i++) {
        if (storage[i].in_use) {
            printf("\nFree SPU Block at -> %u (0x%x) size -> %d\n\n",
                   storage[i].loc, storage[i].loc, storage[i].size);
        }
    }
}

SInt32 snd_GetFreeBlockSize(SInt32 start, UInt32 *loc) {
    SInt32 i;
    sSRAMNodePtr storage;

    storage = gSRAMFreeNodeStorage;

    for (i = start; i < 129; i++) {
        if (storage[i].in_use) {
            *loc = storage[i].loc;
            return storage[i].size;
        }
    }

    return 0;
}
