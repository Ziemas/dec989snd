#include "common.h"
#include "functions.h"
#include "globals.h"
#include "intrman.h"
#include "stdio.h"
#include "thread.h"

/* data 1c1c */ GSoundHandlerPtr gActiveSoundListHead = NULL;
/* data 1c20 */ GSoundHandlerPtr gActiveSoundListTail = NULL;
/* bss 9d0 */ BlockSoundHandler gBlockSoundHandler[64];
/* bss 52d0 */ BasicEffect gBasicEffect[64];
/* bss 68e0 */ MIDIHandler gMIDIHandler[32];
/* bss 59d0 */ AMEHandler gAMEHandler[4];

void snd_InitHandlers(GSoundHandlerPtr handlers, SInt32 count, UInt32 type, SInt32 size) {
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

void snd_InitSoundHandlers() {
    SInt32 x;

    snd_InitHandlers(&gBlockSoundHandler[0].SH, 64, 5, sizeof(BlockSoundHandler));
    snd_InitHandlers(&gMIDIHandler[0].SH, 32, 1, sizeof(MIDIHandler));
    snd_InitHandlers(&gAMEHandler[0].SH, 4, 2, sizeof(AMEHandler));
    for (x = 0; x < 64; ++x) {
        gBasicEffect[x].ec.Flags = 0;
        gBasicEffect[x].ec.next = NULL;
    }
    gActiveSoundListTail = NULL;
    gActiveSoundListHead = NULL;
}

GSoundHandlerPtr snd_FindFreeHandler(GSoundHandlerPtr handlers, SInt32 count, UInt32 type, SInt32 size) {
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

BOOL snd_CheckInstanceLimit(SFX2 *sfx, SInt32 vol, BOOL parent, BlockSoundHandlerPtr *weakest_holder) {
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

    while (walk) {
        type = (walk->SH.OwnerID >> 24) & 0x1f;
        if (type == 5 && ((parent && walk->orig_sound == sfx) || walk->SH.Sound == sfx)) {
            inst++;

            if (!weakest || ((sfx->Flags & 0x10) != 0 && walk->App_Vol < weakest->App_Vol) ||
                ((sfx->Flags & 0x20) != 0 && walk->start_tick < weakest->start_tick)) {
                weakest = walk;
            }
        }

        walk = walk->SH.next;
    }

    if (sfx->InstanceLimit - 1 < inst) {
        if (weakest && (((sfx->Flags & 0x10) != 0 && weakest->App_Vol < vol) || (sfx->Flags & 0x20) != 0)) {
            *weakest_holder = weakest;
        } else {
            return false;
        }
    }

    return true;
}

BlockSoundHandlerPtr snd_GetFreeBlockSoundHandler(SFX2 *sfx, SInt32 vol, BOOL parent) {
    BlockSoundHandlerPtr weakest;

    if (sfx && (sfx->Flags & 8) != 0) {
        if (!snd_CheckInstanceLimit(sfx, vol, parent, &weakest)) {
            return 0;
        }

        if (weakest) {
            snd_StopHandlerPtr(&weakest->SH, 1, 0, 1);
        }
    }

    return snd_FindFreeHandler(&gBlockSoundHandler[0].SH, 64, 5, sizeof(BlockSoundHandler));
}

MIDIHandlerPtr snd_GetFreeMIDIHandler() { return snd_FindFreeHandler(&gMIDIHandler[0].SH, 32, 1, sizeof(MIDIHandler)); }

AMEHandlerPtr snd_GetFreeAMEHandler() { return snd_FindFreeHandler(&gAMEHandler[0].SH, 4, 2, sizeof(AMEHandler)); }

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

void snd_AttachSoundToHandlersChildList(GSoundHandlerPtr handler, UInt32 id) {
    GSoundHandlerPtr child_handle;
    GSoundHandlerPtr walk;

    snd_LockMasterTick(74);
    child_handle = snd_CheckHandlerStillActive(id);
    if (!child_handle) {
        snd_UnlockMasterTick();
    } else {
        if (handler == child_handle || child_handle->parent) {
            snd_ShowError(117, 0, 0, 0, 0);
            snd_UnlockMasterTick();
            return;
        }

        child_handle->parent = handler;
        if (!handler->first_child) {
            handler->first_child = child_handle;
        } else {
            walk = handler->first_child;
            while (walk->siblings) {
                walk = walk->siblings;
            }

            walk->siblings = child_handle;
        }

        snd_UnlockMasterTick();
    }
}

GSoundHandlerPtr snd_CheckHandlerStillActive(UInt32 handle) {
    SInt32 handler_index;
    SInt32 handler_type;

    handler_index = (handle >> 16) & 0xFF;
    handler_type = (handle >> 24) & 0x1F;
    if (!handle) {
        return NULL;
    }

    if (handle == -1) {
        snd_ShowError(100, 0, 0, 0, 0);
        return NULL;
    }

    if (handler_index < 0) {
        snd_ShowError(101, handle, 0, 0, 0);
        return NULL;
    }

    if (handler_type == 5) {
        if (handler_index >= 64) {
            snd_ShowError(101, handle, 0, 0, 0);
            return NULL;
        }

        if (gBlockSoundHandler[handler_index].SH.OwnerID == handle) {
            return &gBlockSoundHandler[handler_index].SH;
        }
    }

    if (handler_type == 1) {
        if (handler_index >= 32) {
            snd_ShowError(101, handle, 0, 0, 0);
            return NULL;
        }

        if (gMIDIHandler[handler_index].SH.OwnerID == handle) {
            return &gMIDIHandler[handler_index].SH;
        }
    }

    if (handler_type == 2) {
        if (handler_index >= 4) {
            snd_ShowError(101, handle, 0, 0, 0);
            return NULL;
        }

        if (gAMEHandler[handler_index].SH.OwnerID == handle) {
            return &gAMEHandler[handler_index].SH;
        }
    }

    if (handler_type == 4) {
        if (handler_index > gNumVAGStreams - 1) {
            snd_ShowError(101, handle, 0, 0, 0);
            return NULL;
        }

        if (gVAGStreamHandler[handler_index].SH.OwnerID == handle) {
            return &gVAGStreamHandler[handler_index].SH;
        }
    }

    return NULL;
}

void snd_StopHandler(UInt32 handle, SInt32 and_child, SInt32 silence, BOOL vlimit_stop) {
    SInt32 handler_index;
    SInt32 handler_type;
    GSoundHandlerPtr snd;

    handler_index = (handle >> 16) & 0xFF;
    handler_type = (handle >> 24) & 0x1F;

    switch (handler_type) {
    case 5:
        snd = &gBlockSoundHandler[handler_index].SH;
        break;
    case 1:
        snd = &gMIDIHandler[handler_index].SH;
        break;
    case 2:
        snd = &gAMEHandler[handler_index].SH;
        break;
    case 4:
        snd = &gVAGStreamHandler[handler_index].SH;
        break;
    default:
        return;
        break;
    }

    snd_StopHandlerPtr(snd, and_child, silence, vlimit_stop);
}

void snd_PauseHandlerPtr(GSoundHandlerPtr snd, SInt32 and_child) {
    GSoundHandlerPtr walk;

    if (and_child && snd->first_child) {
        for (walk = snd->first_child; walk; walk = walk->siblings) {
            snd_PauseHandlerPtr(walk, 1);
        }
    }
    snd->flags |= 2u;
    snd_PauseVoicesOwnedWithOwner(snd);
}

void snd_ContinueHandlerPtr(GSoundHandlerPtr snd, SInt32 and_child) {
    GSoundHandlerPtr walk;

    if (and_child && snd->first_child) {
        for (walk = snd->first_child; walk; walk = walk->siblings) {
            snd_ContinueHandlerPtr(walk, 1);
        }
    }
    snd_UnPauseVoicesOwnedWithOwner(snd);
    snd->flags &= ~2u;
}

void snd_PauseAllSoundsInGroup(UInt32 groups) {
    GSoundHandlerPtr walk;
    SInt32 type;

    walk = gActiveSoundListHead;
    snd_LockMasterTick(75);

    while (walk) {
        if ((groups & (1 << walk->VolGroup)) && (walk->flags & 2) == 0) {
            type = (walk->OwnerID >> 24) & 0x1f;
            if (type == 4) {
                snd_PauseVAGStream(walk->OwnerID);
            } else {
                snd_PauseHandlerPtr(walk, 1);
            }
        }
        walk = walk->next;
    }

    snd_UnlockMasterTick();
}

void snd_StopAllSoundsInGroup(UInt32 groups) {
    GSoundHandlerPtr walk;

    walk = gActiveSoundListHead;
    snd_LockMasterTick(76);
    while (walk) {
        if ((groups & (1 << walk->VolGroup)) && (walk->OwnerID & 0x80000000)) {
            snd_StopSound(walk->OwnerID);
        }
        walk = walk->next;
    }
    snd_UnlockMasterTick();
}

void snd_ContinueAllSoundsInGroup(UInt32 groups) {
    GSoundHandlerPtr walk;
    SInt32 type;

    walk = gActiveSoundListHead;
    snd_LockMasterTick(77);
    while (walk) {
        if ((groups & (1 << walk->VolGroup)) && (walk->flags & 2)) {
            type = (walk->OwnerID >> 24) & 0x1f;
            if (type == 4) {
                snd_ContinueVAGStream(walk->OwnerID);
            } else {
                snd_ContinueHandlerPtr(walk, 1);
            }
        }
        walk = walk->next;
    }
    snd_UnlockMasterTick();
}

void snd_StopAllSounds() {
    GSoundHandlerPtr walk;
    VoiceFlags voices;
    walk = gActiveSoundListHead;
    snd_LockMasterTick(78);

    while (walk) {
        if ((walk->OwnerID & 0x80000000) == 0) {
            printf("testing deactivated handler on the active list!\n");
        }
        if (!walk->parent && (walk->flags & 4) == 0) {
            snd_StopHandlerPtr(walk, 1, 1, 0);
        }
        walk = walk->next;
    }

    voices.core[0] = -1;
    voices.core[1] = -1;

    snd_SilenceVoicesEx(&voices, 1);
    snd_UnlockMasterTick();

    if (gActiveSoundListHead) {
        while (gActiveSoundListHead) {
            DelayThread(33000);
        }
    }
}

void snd_StopHandlerPtr(GSoundHandlerPtr snd, SInt32 and_child, SInt32 silence, BOOL vlimit_stop) {
    SInt32 do_voice_and_deactivate;
    BOOL kill_block_sound;
    GSoundHandlerPtr walk;
    MultiMIDIBlockHeaderPtr ame_master;

    do_voice_and_deactivate = 1;
    kill_block_sound = 0;
    snd_LockMasterTick(79);

    if (!(snd->OwnerID & 0x80000000)) {
        snd_UnlockMasterTick();
        return;
    }

    if (snd->flags & 4) {
        kill_block_sound = 1;
        and_child = 1;
    }

    snd->flags |= 4u;
    if (and_child && snd->first_child) {
        for (walk = snd->first_child; walk; walk = walk->siblings) {
            snd_StopHandlerPtr(walk, and_child, silence, vlimit_stop);
        }
    }

    switch ((snd->OwnerID >> 24) & 0x1f) {
    case 5:
        if (kill_block_sound) {
            snd_DoBlockSoundStop((BlockSoundHandler *)snd, 1, vlimit_stop);
        } else if (snd_DoBlockSoundStop((BlockSoundHandler *)snd, silence, vlimit_stop)) {
            do_voice_and_deactivate = 0;
        }
        break;
    case 2:
        ame_master = ((MIDISoundPtr)snd->Sound)->MIDIBlock;
        ame_master->Flags &= ~2;
        break;
    case 4:
        snd_StopVAGStream(snd->OwnerID);
        do_voice_and_deactivate = 0;
        break;
    }

    if (do_voice_and_deactivate) {
        snd_LockVoiceAllocatorEx(1, 74);

        if (silence || (snd->flags & 2)) {
            snd_SilenceVoicesEx(&snd->Voices, 1);
        } else {
            snd_KeyOffVoicesEx(&snd->Voices, 1);
        }

        snd_UnlockVoiceAllocator();

        if (snd->OwnerID & 0x80000000) {
            snd_DeactivateHandler(snd, 0);
        }
    }

    snd_UnlockMasterTick();
}

void snd_StopAllHandlersForSound(SoundPtr snd, SInt32 silence, BOOL vlimit_stop) {
    GSoundHandlerPtr walk;

    for (walk = gActiveSoundListHead; walk; walk = walk->next) {
        if (walk->Sound == snd) {
            snd_StopHandler(walk->OwnerID, 1, silence, vlimit_stop);
        }
    }
}

BOOL snd_KillChildrenWithSound(GSoundHandlerPtr handler, void *sfx) {
    GSoundHandlerPtr walk;

    if (!handler->first_child)
        return false;

    for (walk = handler->first_child; walk; walk = walk->siblings) {
        if (walk->Sound == sfx) {
            snd_StopHandlerPtr(walk, 1, 0, 0);
            return true;
        }
    }
    return false;
}

SInt32 snd_GetNextHandlerVoice(GSoundHandlerPtr snd, SInt32 start_v) {
    SInt32 x;
    SInt32 core;
    SInt32 c_v;

    for (x = start_v; x < 48; x++) {
        core = x / 24;
        c_v = x % 24;

        if (snd->Voices.core[core] & (1 << c_v)) {
            return x;
        }
    }

    return -1;
}

void snd_UpdateHandlers() {
    GSoundHandlerPtr walk;
    SInt32 type;
    SInt32 stop_current_handler;

    walk = gActiveSoundListHead;
    stop_current_handler = 0;
    while (walk) {
        type = (walk->OwnerID >> 24) & 0x1f;
        switch (type) {
        case 5:
            if (!(walk->flags & 2)) {
                stop_current_handler = snd_ProcessBlockSoundTick((BlockSoundHandlerPtr)walk);
            }
            break;
        case 0:
            break;
        case 1:
        case 3:
            if (!(walk->flags & 2)) {
                stop_current_handler = snd_ProcessMIDITick((MIDIHandlerPtr)walk);
            }
            break;
        case 2:
            if (!(walk->first_child)) {
                stop_current_handler = 1;
            }
            break;
        case 4:
            snd_ProcessVAGStreamTick((VAGStreamHandlerPtr)walk);
            break;
        default:
            break;
        }

        if (!stop_current_handler && walk->Effects && (walk->flags & 2) == 0) {
            stop_current_handler = snd_UpdateEffect(walk->Effects, walk);
        }
        if (stop_current_handler) {
            snd_StopHandlerPtr(walk, 1, 0, 0);
            stop_current_handler = 0;
        }

        walk = walk->next;
    }
}

SInt32 snd_UpdateEffect(EffectChainPtr effect, GSoundHandlerPtr owner) {
    if (effect->next && snd_UpdateEffect(effect->next, owner) == 1) {
        return 1;
    }

    if ((effect->Flags & 0x80000000) != 0) {
        return 0;
    }

    if (effect->delta_type == 1) {
        effect->delta_counter--;
    }

    if (!effect->delta_counter) {
        return effect->proc(effect, owner);
    }

    return 0;
}

void snd_FreeEffectChain(EffectChainPtr effect) {
    if (effect->next) {
        snd_FreeEffectChain(effect->next);
    }

    effect->Flags = 0;
}

void snd_RemoveEffect(GSoundHandlerPtr handler, SInt32 type_flag) {
    EffectChainPtr found;

    found = snd_FindEffect(handler, type_flag);
    if (found) {
        snd_RemoveEffectFromHandler(handler, found);
    }
}

EffectChainPtr snd_FindEffect(GSoundHandlerPtr handler, SInt32 type_flag) {
    EffectChainPtr walk;
    walk = handler->Effects;

    while (walk && (walk->Flags & type_flag) == 0) {
        walk = walk->next;
    }

    return walk;
}

BasicEffectPtr snd_GetFreeBasicEffect() {
    SInt32 x;

    for (x = 0; x < 64; ++x) {
        if ((gBasicEffect[x].ec.Flags & 1) == 0) {
            gBasicEffect[x].ec.Flags = 1;
            gBasicEffect[x].ec.next = 0;
            return &gBasicEffect[x];
        }
    }

    return NULL;
}

void snd_AddEffectToHandler(GSoundHandlerPtr handler, EffectChainPtr effect) {
    EffectChainPtr walk;

    if (!handler->Effects) {
        handler->Effects = effect;
    } else {
        walk = handler->Effects;
        while (walk->next) {
            walk = walk->next;
        }

        walk->next = effect;
    }
}

void snd_RemoveEffectFromHandler(GSoundHandlerPtr handler, EffectChainPtr effect) {
    SInt32 intr_state;
    SInt32 res;
    EffectChainPtr walk;

    if (!effect) {
        return;
    }

    res = CpuSuspendIntr(&intr_state);
    effect->Flags &= ~1u;
    if (effect == handler->Effects) {
        handler->Effects = effect->next;
    } else {
        walk = handler->Effects;
        while (walk) {
            if (walk->next == effect) {
                walk->next = effect->next;
                walk = 0;
            } else {
                walk = walk->next;
            }
        }
    }

    if (!res) {
        CpuResumeIntr(intr_state);
    }
}