#include "common.h"
#include "functions.h"
#include "intrman.h"
#include "stdio.h"

/* data 1c1c */ GSoundHandlerPtr gActiveSoundListHead = NULL;
/* data 1c20 */ GSoundHandlerPtr gActiveSoundListTail = NULL;
/* bss 9d0 */ BlockSoundHandler gBlockSoundHandler[64];
/* bss 52d0 */ BasicEffect gBasicEffect[64];
/* bss 68e0 */ MIDIHandler gMIDIHandler[32];
/* bss 59d0 */ AMEHandler gAMEHandler[4];

void snd_InitHandlers(GSoundHandlerPtr handlers, SInt32 count, UInt32 type,
                      SInt32 size) {
    int x;

    for (x = 0; x < count; ++x) {
        handlers->OwnerID = 1;
        handlers->OwnerID |= (x << 16) | (type << 24);
        handlers->Sound = 0;
        handlers->Effects = 0;
        handlers->flags = 0;
        handlers->VolGroup = 0;
        handlers->Voices.core[0] = handlers->Voices.core[1] = 0;
        handlers = (GSoundHandlerPtr)((char *)handlers + size);
    }
}

// INCLUDE_ASM("asm/nonmatchings/sndhand", snd_InitSoundHandlers);
void snd_InitSoundHandlers() {
    SInt32 x;

    snd_InitHandlers(&gBlockSoundHandler[0].SH, 64, 5,
                     sizeof(BlockSoundHandler));
    snd_InitHandlers(&gMIDIHandler[0].SH, 32, 1, sizeof(MIDIHandler));
    snd_InitHandlers(&gAMEHandler[0].SH, 4, 2, sizeof(AMEHandler));
    for (x = 0; x < 64; ++x) {
        gBasicEffect[x].ec.Flags = 0;
        gBasicEffect[x].ec.next = NULL;
    }
    gActiveSoundListTail = NULL;
    gActiveSoundListHead = NULL;
}

GSoundHandlerPtr snd_FindFreeHandler(GSoundHandlerPtr handlers, SInt32 count,
                                     UInt32 type, SInt32 size) {
    SInt32 x;
    SInt32 owner_num;
    SInt32 intr_state;
    SInt32 dis;

    dis = CpuSuspendIntr(&intr_state);
    for (x = 0; x < count; ++x) {
        if (!handlers->Sound) {
            owner_num = (handlers->OwnerID & 0xffff) + 1;
            handlers->OwnerID = (owner_num & 0xffff) | (x << 16) | (type << 24);
            handlers->next = 0;
            handlers->prev = 0;
            handlers->parent = 0;
            handlers->first_child = 0;
            handlers->siblings = 0;
            handlers->flags = 0;
            if (!dis) {
                CpuResumeIntr(intr_state);
            }
            return handlers;
        }

        handlers = (GSoundHandlerPtr)((char *)handlers + size);
    }

    if (!dis) {
        CpuResumeIntr(intr_state);
    }

    return 0;
}

// INCLUDE_ASM("asm/nonmatchings/sndhand", snd_CheckInstanceLimit);
BOOL snd_CheckInstanceLimit(SFX2 *sfx, SInt32 vol, BOOL parent,
                            BlockSoundHandlerPtr *weakest_holder) {
    SInt32 inst;
    SInt32 type;
    BlockSoundHandlerPtr weakest;
    BlockSoundHandlerPtr walk;

    inst = 0;
    weakest = 0;
    walk = gActiveSoundListHead;

    if (!sfx->InstanceLimit) {
        return false;
    }

    if (weakest_holder) {
        *weakest_holder = NULL;
    }

    for (; walk; walk = walk->SH.next) {
        type = (walk->SH.OwnerID >> 24) & 0x1f;
        if (type == 5 &&
            ((parent && walk->orig_sound == sfx) || walk->SH.Sound == sfx)) {
            inst++;

            if (!weakest ||
                ((sfx->Flags & 0x10) != 0 &&
                 walk->App_Vol < weakest->App_Vol) ||
                ((sfx->Flags & 0x20) != 0 &&
                 walk->start_tick < weakest->start_tick)) {
                weakest = walk;
            }
        }
    }

    if (sfx->InstanceLimit - 1 < inst) {
        if (weakest && (((sfx->Flags & 0x10) != 0 && weakest->App_Vol < vol) ||
                        (sfx->Flags & 0x20) != 0)) {
            *weakest_holder = weakest;
        } else {
            return false;
        }
    }

    return true;
}

BlockSoundHandlerPtr snd_GetFreeBlockSoundHandler(SFX2 *sfx, SInt32 vol,
                                                  BOOL parent) {
    BlockSoundHandlerPtr weakest;

    if (sfx && (sfx->Flags & 8) != 0) {
        if (!snd_CheckInstanceLimit(sfx, vol, parent, &weakest)) {
            return 0;
        }

        if (weakest) {
            snd_StopHandlerPtr(&weakest->SH, 1, 0, 1);
        }
    }

    return snd_FindFreeHandler(&gBlockSoundHandler[0].SH, 64, 5,
                               sizeof(BlockSoundHandler));
}

MIDIHandlerPtr snd_GetFreeMIDIHandler() {
    return snd_FindFreeHandler(&gMIDIHandler[0].SH, 32, 1, sizeof(MIDIHandler));
}

AMEHandlerPtr snd_GetFreeAMEHandler() {
    return snd_FindFreeHandler(&gAMEHandler[0].SH, 4, 2, sizeof(AMEHandler));
}

void snd_ActivateHandler(GSoundHandlerPtr snd) {
    SInt32 intr_state;
    SInt32 dis;

    dis = CpuSuspendIntr(&intr_state);
    snd->prev = 0;
    snd->next = 0;
    snd->OwnerID |= 0x80000000;

    if (!gActiveSoundListTail) {
        gActiveSoundListTail = gActiveSoundListHead = snd;
    } else {
        if (gActiveSoundListTail != snd) {
            gActiveSoundListTail->next = snd;
            snd->prev = gActiveSoundListTail;
            gActiveSoundListTail = snd;
        }
    }

    if (!dis) {
        CpuResumeIntr(intr_state);
    }
}

// INCLUDE_ASM("asm/nonmatchings/sndhand", snd_DeactivateHandler);
void snd_DeactivateHandler(GSoundHandlerPtr snd, SInt32 and_child) {
    GSoundHandlerPtr walk;
    SInt32 intr_state;
    SInt32 dis;

    if (!and_child && !snd->parent && snd->first_child) {
        snd_ShowError(120, 0, 0, 0, 0);
        and_child = 1;
    }

    if (and_child) {
        while (snd->first_child) {
            snd_DeactivateHandler(snd->first_child, and_child);
        }
    }

    dis = CpuSuspendIntr(&intr_state);
    if (snd->Effects) {
        snd_FreeEffectChain(snd->Effects);
        snd->Effects = NULL;
    }

    if (snd->prev) {
        if (snd->prev->next != snd) {
            printf("list problem 1!\n");
        } else {
            snd->prev->next = snd->next;
        }
    } else {
        gActiveSoundListHead = snd->next;
    }

    if (snd->next) {
        if (snd->next->prev != snd) {
            printf("list problem 2!\n");
        } else {
            snd->next->prev = snd->prev;
        }
    } else {
        gActiveSoundListTail = snd->prev;
    }

    if (snd->parent) {
        if (snd->parent->first_child == snd) {
            snd->parent->first_child = snd->siblings;
        } else {
            walk = snd->parent->first_child;
            while (walk->siblings != snd) {
                walk = walk->siblings;
            }

            if (!walk) {
                if (!dis) {
                    CpuResumeIntr(intr_state);
                }
                snd_ShowError(116, 0, 0, 0, 0);
                return;
            }
            walk->siblings = snd->siblings;
        }

        walk = snd->first_child;
        while (walk) {
            walk->parent = snd->parent;
            walk = walk->siblings;
        }

        if (!snd->parent->first_child) {
            snd->parent->first_child = snd->first_child;
        } else {
            walk = snd->parent->first_child;
            while (walk->siblings) {
                walk = walk->siblings;
            }

            walk->siblings = snd->first_child;
        }
    }

    snd->Sound = NULL;
    snd->OwnerID &= ~0x80000000;
    if (!dis) {
        CpuResumeIntr(intr_state);
    }
}

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_AttachSoundToHandlersChildList);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_CheckHandlerStillActive);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_StopHandler);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_PauseHandlerPtr);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_ContinueHandlerPtr);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_PauseAllSoundsInGroup);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_StopAllSoundsInGroup);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_ContinueAllSoundsInGroup);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_StopAllSounds);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_StopHandlerPtr);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_StopAllHandlersForSound);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_KillChildrenWithSound);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_GetNextHandlerVoice);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_UpdateHandlers);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_UpdateEffect);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_FreeEffectChain);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_RemoveEffect);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_FindEffect);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_GetFreeBasicEffect);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_AddEffectToHandler);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_RemoveEffectFromHandler);
