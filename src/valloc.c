#include "common.h"
#include "functions.h"
#include "globals.h"
#include "intrman.h"
#include "libsd.h"
#include "sdmacro.h"
#include "thread.h"

/* data 1dfc */ SInt32 gLockBlockerSema = -1;
/* data 1e00 */ SInt32 gTickAttentionNeeded = 0;
/* data 1e04 */ SInt32 gVoiceAllocationBlocked = 0;
/* data 1e08 */ SInt32 gVoiceAllocatorInUse = 0;
/* data 1e0c */ UInt32 gVAllocOwnerID = 0;
/* data 1e10 */ SInt32 gChannelMode = 0;
/* data 1e14 */ VoiceRange gVoiceRanges[16] = {
    {0, 47}, {0, 47}, {0, 47}, {0, 47}, {0, 47}, {0, 47}, {0, 47}, {0, 47},
    {0, 47}, {0, 47}, {0, 47}, {0, 47}, {0, 47}, {0, 47}, {0, 47}, {0, 47},
};
/* data 1e54 */ VoiceAttributes *gPlayingListHead = NULL;
/* data 1e58 */ SInt32 gNoiseOwnerPriority[2] = {-1, -1};
/* data 1e60 */ SInt32 gNoiseOwnerVoice[2] = {-1, -1};
/* data 1e68 */ SInt32 gThreadWaitingForVoiceAllocator = -1;
/* bss 92d0 */ VoiceAttributes gChannelStatus[48];
/* bss 20 */ UInt32 gAwaitingKeyOn[2];
/* bss 28 */ UInt32 gAwaitingKeyOff[2];
/* bss 30 */ UInt32 gKeyedOnVoices[2];
/* bss 38 */ UInt32 gKeyedOffVoices[2];
/* bss 18 */ UInt32 gReverbVoices[2];
/* bss 10 */ UInt32 gExternVoices[2];

void snd_InitVoiceAllocator() {
    SInt32 count;
    struct SemaParam sema;

    gVoiceAllocationBlocked = 0;
    gTickAttentionNeeded = 0;
    for (count = 0; count < 48; ++count) {
        snd_MarkVoiceFree(count);
        gChannelStatus[count].voice = count;
        gAutoFreeState[count] = 0;
    }

    gAwaitingKeyOn[1] = 0;
    gAwaitingKeyOn[0] = 0;
    gAwaitingKeyOff[1] = 0;
    gAwaitingKeyOff[0] = 0;
    gKeyedOnVoices[1] = 0;
    gKeyedOnVoices[0] = 0;
    gKeyedOffVoices[1] = 0;
    gKeyedOffVoices[0] = 0;
    gReverbVoices[1] = 0;
    gReverbVoices[0] = 0;
    gExternVoices[1] = 0;
    gExternVoices[0] = 0;
    gChannelMode = 0;
    gReverbMode = 0;
    gPlayingListHead = 0;
    for (count = 0; count < 16; ++count) {
        gVoiceRanges[count].min = 0;
        gVoiceRanges[count].max = 47;
    }

    sema.attr = 1;
    sema.initCount = 0;
    sema.maxCount = 1;
    gLockBlockerSema = CreateSema(&sema);
    SignalSema(gLockBlockerSema);
}

void snd_CleanupVoiceAllocator() {
    DeleteSema(gLockBlockerSema);
    gLockBlockerSema = -1;
}

void snd_SetMixerMode(SInt32 channel_mode, SInt32 reverb_mode) {
    SInt32 count;

    snd_SetReverbMode(reverb_mode);
    gChannelMode = channel_mode;
    if (gChannelMode == 1) {
        for (count = 0; count < 16; ++count) {
            gVoiceRanges[count].min = 24;
            gVoiceRanges[count].max = 47;
        }

        gVoiceRanges[1].min = 0;
        gVoiceRanges[1].max = 23;
    } else {
        for (count = 0; count < 16; ++count) {
            gVoiceRanges[count].min = 0;
            gVoiceRanges[count].max = 47;
        }
    }
}

void snd_SetGroupVoiceRange(SInt32 group, SInt32 min, SInt32 max) {
    if (min < 0) {
        min = 0;
    }

    if (max > 47) {
        max = 47;
    }

    gVoiceRanges[group].min = min;
    gVoiceRanges[group].max = max;
}

SInt32 snd_LockVoiceAllocatorEx(BOOL block, UInt32 ownerID) {
    SInt32 ws_ret;

    ws_ret = 0;
    if (!block) {
        if (PollSema(gLockBlockerSema) == -419) {
            return false;
        }
    } else {
        ws_ret = WaitSema(gLockBlockerSema);
        if (ws_ret) {
            snd_ShowError(80, ws_ret, 0, 0, 0);
            return 0;
        }
    }

    gVAllocOwnerID = ownerID;
    gVoiceAllocatorInUse = 1;

    return 1;
}

SInt32 snd_IsVoiceAllocatorLocked() { return gVoiceAllocatorInUse; }

SInt32 snd_CheckAllocatorCueTick() {
    if (!gVoiceAllocatorInUse) {
        return 0;
    }

    gTickAttentionNeeded++;

    return 1;
}

void snd_UnlockVoiceAllocator() {
    gVoiceAllocatorInUse = 0;
    gVAllocOwnerID = 0;
    SignalSema(gLockBlockerSema);
}

SInt32 snd_ExternVoiceAlloc(SInt32 vol_group, SInt32 priority) {
    SInt32 voice;
    UInt32 core;
    UInt32 c_v;

    if (vol_group > 15) {
        vol_group = 15;
    }

    voice = snd_AllocateVoice(vol_group, priority);
    if (voice > -1) {
        core = voice / 24;
        c_v = voice % 24;
        gChannelStatus[voice].Status = 3;
        gExternVoices[core] |= 1 << (c_v);
    }

    return voice;
}

void snd_ExternVoiceFree(SInt32 voice) {
    UInt32 core;
    UInt32 c_v;

    core = voice / 24;
    c_v = voice % 24;
    snd_MarkVoiceFree(voice);
    gExternVoices[core] &= ~(1 << c_v);
}

void snd_SetVoiceNoiseOrWave(SInt32 voice, BOOL wave) {
    SInt32 intr_state;
    SInt32 dis;
    UInt32 core;
    UInt32 c_v;

    core = voice / 24;
    c_v = voice % 24;
    dis = CpuSuspendIntr(&intr_state);
    sceSdSetParam(core | SD_VOICE(c_v) | SD_VP_VOLL, 0);
    sceSdSetParam((core | SD_VOICE(c_v)) | SD_VP_VOLR, 0);

    if (wave) {
        sceSdSetSwitch(core | SD_S_NON, sceSdGetSwitch(core | SD_S_NON) & ~(1 << c_v));
    } else {
        sceSdSetSwitch(core | SD_S_NON, sceSdGetSwitch(core | SD_S_NON) | (1 << c_v));
    }

    if (!dis) {
        CpuResumeIntr(intr_state);
    }
}

SInt32 snd_AllocateNoiseVoice(SInt32 vol_group, SInt32 priority) {
    SInt32 range_temp;
    SInt32 voice;
    SInt32 intr_state;
    SInt32 dis;

    range_temp = -1;
    voice = -1;
    dis = CpuSuspendIntr(&intr_state);
    if (gNoiseOwnerPriority[0] < priority && gVoiceRanges[vol_group].min < 24) {
        if (gVoiceRanges[vol_group].max >= 24) {
            range_temp = gVoiceRanges[vol_group].max;
            gVoiceRanges[vol_group].max = 23;
        }

        voice = snd_AllocateVoice(vol_group, priority);
        if (range_temp > -1) {
            gVoiceRanges[vol_group].max = range_temp;
        }

        if (voice != -1) {
            snd_SetVoiceNoiseOrWave(voice, 0);
            gNoiseOwnerPriority[0] = priority;
            gNoiseOwnerVoice[0] = voice;
        }
    }

    if (voice == -1 && gNoiseOwnerPriority[1] < priority && gVoiceRanges[vol_group].max >= 24) {
        if (gVoiceRanges[vol_group].min < 24) {
            range_temp = gVoiceRanges[vol_group].min;
            gVoiceRanges[vol_group].min = 24;
        }

        voice = snd_AllocateVoice(vol_group, priority);
        if (range_temp > -1) {
            gVoiceRanges[vol_group].min = range_temp;
        }

        if (voice != -1) {
            snd_SetVoiceNoiseOrWave(voice, 0);
            gNoiseOwnerPriority[1] = priority;
            gNoiseOwnerVoice[1] = voice;
        }
    }

    if (!dis) {
        CpuResumeIntr(intr_state);
    }

    return voice;
}

SInt32 snd_AllocateVoice(SInt32 vol_group, SInt32 priority) {
    SInt32 count;
    SInt32 start;
    SInt32 end;
    SInt32 lowest_priority;

    if (vol_group > 15) {
        vol_group = 15;
    }

    start = gVoiceRanges[vol_group].min;
    end = gVoiceRanges[vol_group].max + 1;

    for (count = start; count < end; ++count) {
        if (!gChannelStatus[count].Status) {
            gChannelStatus[count].Status = 2;
            gChannelStatus[count].StartTick = snd_GetTick();
            gChannelStatus[count].Priority = priority;
            gChannelStatus[count].VolGroup = vol_group;
            gAutoFreeState[count] = 4;
            snd_SetVoiceNoiseOrWave(count, 1);
            return count;
        }
    }

    lowest_priority = start;
    for (count = start + 1; count < end; ++count) {
        if (gChannelStatus[count].Status != 3 &&
            (gChannelStatus[count].Priority < gChannelStatus[lowest_priority].Priority ||
             gChannelStatus[count].Priority == gChannelStatus[lowest_priority].Priority &&
                 gChannelStatus[count].StartTick < gChannelStatus[lowest_priority].StartTick)) {
            lowest_priority = count;
        }
    }

    if (priority >= gChannelStatus[lowest_priority].Priority && gChannelStatus[lowest_priority].Priority < 0x7Eu &&
        gChannelStatus[lowest_priority].Status != 3) {

        if (gChannelStatus[lowest_priority].OwnerProc) {
            gChannelStatus[lowest_priority].OwnerProc(lowest_priority, gChannelStatus[lowest_priority].Owner, 1);
        }

        snd_MarkVoiceFree(lowest_priority);
        gChannelStatus[lowest_priority].Status = 2;
        gChannelStatus[lowest_priority].StartTick = snd_GetTick();
        gChannelStatus[lowest_priority].Priority = priority;
        gChannelStatus[lowest_priority].VolGroup = vol_group;
        gChannelStatus[lowest_priority].OwnerProc = 0;
        gAutoFreeState[lowest_priority] = 4;
        snd_SetVoiceNoiseOrWave(lowest_priority, 1);

        return lowest_priority;
    }

    return -1;
}

// INCLUDE_ASM("asm/nonmatchings/valloc", snd_StartVAGVoice);
void snd_StartVAGVoice(SInt32 voice, BOOL noise) {
    SInt32 note;
    SInt32 fine;
    UInt32 pitch;
    SInt32 intr_state;
    SInt32 dis;
    UInt32 flags;
    UInt32 core;
    UInt32 c_v;
    SInt32 clock;

    core = voice / 24;
    c_v = voice % 24;
    gChannelStatus[voice].StartTick = snd_GetTick();
    dis = CpuSuspendIntr(&intr_state);

    sceSdSetParam(core | SD_VOICE(c_v) | SD_VP_VOLL,
                  (UInt16)snd_AdjustVolToGroup(gChannelStatus[voice].Volume.left, gChannelStatus[voice].VolGroup) >> 1);
    sceSdSetParam(core | SD_VOICE(c_v) | SD_VP_VOLR,
                  (UInt16)snd_AdjustVolToGroup(gChannelStatus[voice].Volume.right, gChannelStatus[voice].VolGroup) >>
                      1);

    if ((gChannelStatus[voice].Tone->Flags & 0x10) == 0) {
        sceSdSetSwitch(core | SD_S_VMIXL, sceSdGetSwitch(core | SD_S_VMIXL) | (1 << c_v));
        sceSdSetSwitch(core | SD_S_VMIXR, sceSdGetSwitch(core | SD_S_VMIXR) | (1 << c_v));
    } else {
        flags = ~(1 << c_v);
        sceSdSetSwitch(core | SD_S_VMIXL, sceSdGetSwitch(core | SD_S_VMIXL) & flags);
        sceSdSetSwitch(core | SD_S_VMIXR, sceSdGetSwitch(core | SD_S_VMIXR) & flags);
    }

    if (!noise) {
        sceSdSetSwitch(core | SD_S_NON, sceSdGetSwitch(core | SD_S_NON) & ~(1 << c_v));
        snd_PitchBendTone(gChannelStatus[voice].Tone, gChannelStatus[voice].Current_PB,
                          gChannelStatus[voice].Current_PM, gChannelStatus[voice].StartNote,
                          gChannelStatus[voice].StartFine, &note, &fine);
        pitch =
            PS1Note2Pitch(gChannelStatus[voice].Tone->CenterNote, gChannelStatus[voice].Tone->CenterFine, note, fine);
        sceSdSetParam(core | SD_VOICE(c_v) | SD_VP_PITCH, pitch);
        sceSdSetAddr(core | SD_VOICE(c_v) | SD_VA_SSA, (UInt32)gChannelStatus[voice].Tone->VAGInSR);
    } else {
        clock = gChannelStatus[voice].Tone->CenterNote + 32 * gChannelStatus[voice].Current_PB / 0x7FFF;
        if (clock < 0)
            clock = 0;
        if (clock >= 64)
            clock = 63;
        sceSdSetSwitch(core | SD_S_NON, sceSdGetSwitch(core | SD_S_NON) | (1 << c_v));
        sceSdSetCoreAttr(core | SD_C_NOISE_CLK, clock);
    }

    sceSdSetParam(core | SD_VOICE(c_v) | SD_VP_ADSR1, gChannelStatus[voice].Tone->ADSR1);
    sceSdSetParam(core | SD_VOICE(c_v) | SD_VP_ADSR2, gChannelStatus[voice].Tone->ADSR2);

    if (!dis) {
        CpuResumeIntr(intr_state);
    }

    if ((gChannelStatus[voice].Tone->Flags & 1) != 0) {
        gReverbVoices[core] |= 1 << c_v;
    } else {
        gReverbVoices[core] &= ~(1 << c_v);
    }

    gAwaitingKeyOn[core] |= 1 << c_v;
    snd_MarkVoicePlaying(voice);
}

void snd_StartVoices(VoiceFlags *voices) {
    SInt32 count;

    for (count = 0; count < 48; ++count) {
        if (count < 24 && (voices->core[0] & (1 << count)) != 0 ||
            count >= 24 && (voices->core[1] & (1 << (count - 24))) != 0) {
            snd_MarkVoicePlaying(count);
            gChannelStatus[count].StartTick = snd_GetTick();
        }
    }

    gAwaitingKeyOn[0] |= voices->core[0];
    gAwaitingKeyOn[1] |= voices->core[1];
}

void snd_StartPendingVoices() {
    SInt32 intr_state;
    SInt32 dis;

    if (gAwaitingKeyOn[0] || gAwaitingKeyOn[1]) {
        dis = CpuSuspendIntr(&intr_state);
        sceSdSetSwitch(SD_S_VMIXEL | SD_CORE_0, gReverbVoices[0]);
        sceSdSetSwitch(SD_S_VMIXER | SD_CORE_0, gReverbVoices[0]);
        sceSdSetSwitch(SD_S_VMIXEL | SD_CORE_1, gReverbVoices[1]);
        sceSdSetSwitch(SD_S_VMIXER | SD_CORE_1, gReverbVoices[1]);
        sceSdSetSwitch(SD_S_KON | SD_CORE_0, gAwaitingKeyOn[0]);
        sceSdSetSwitch(SD_S_KON | SD_CORE_1, gAwaitingKeyOn[1]);

        if (!dis) {
            CpuResumeIntr(intr_state);
        }

        gKeyedOffVoices[0] &= ~gAwaitingKeyOn[0];
        gKeyedOffVoices[1] &= ~gAwaitingKeyOn[1];
        gKeyedOnVoices[0] |= gAwaitingKeyOn[0];
        gKeyedOnVoices[1] |= gAwaitingKeyOn[1];
        gAwaitingKeyOn[1] = 0;
        gAwaitingKeyOn[0] = 0;
    }
}

void snd_StopPendingVoices() {
    SInt32 intr_state;
    SInt32 dis;

    if (gAwaitingKeyOff[0] || gAwaitingKeyOff[1]) {
        dis = CpuSuspendIntr(&intr_state);
        sceSdSetSwitch(SD_S_KOFF | SD_CORE_0, gAwaitingKeyOff[0]);
        sceSdSetSwitch(SD_S_KOFF | SD_CORE_1, gAwaitingKeyOff[1]);
        if (!dis) {
            CpuResumeIntr(intr_state);
        }

        gKeyedOnVoices[0] &= ~gAwaitingKeyOff[0];
        gKeyedOnVoices[1] &= ~gAwaitingKeyOff[1];
        gKeyedOffVoices[0] |= gAwaitingKeyOff[0];
        gKeyedOffVoices[1] |= gAwaitingKeyOff[1];
        gAwaitingKeyOff[1] = 0;
        gAwaitingKeyOff[0] = 0;
    }
}

void snd_GetKeyedOnVoices(VoiceFlags *voices) {
    voices->core[0] = gKeyedOnVoices[0];
    voices->core[1] = gKeyedOnVoices[1];
}

void snd_GetKeyedOffVoices(VoiceFlags *voices) {
    voices->core[0] = gKeyedOffVoices[0];
    voices->core[1] = gKeyedOffVoices[1];
}

void snd_VoiceIsDone(SInt32 voice) {
    if (gChannelStatus[voice].Priority != 127) {
        snd_MarkVoiceFree(voice);
    }

    if (gChannelStatus[voice].OwnerProc) {
        gChannelStatus[voice].OwnerProc(voice, gChannelStatus[voice].Owner, 2);
    }

    gChannelStatus[voice].OwnerProc = 0;
    gKeyedOffVoices[voice / 24] &= ~(1 << (voice % 24));
}

UInt32 snd_GetVoiceAge(SInt32 voice) {
    if (gChannelStatus[voice].Status == 1) {
        return snd_GetTick() - gChannelStatus[voice].StartTick;
    }

    return 0;
}

SInt32 snd_GetVoiceStatus(SInt32 voice) { return gChannelStatus[voice].Status; }

SInt32 snd_GetVoicePriority(SInt32 voice) { return gChannelStatus[voice].Priority; }

void snd_KeyOffVoice(SInt32 voice) {
    SInt32 core;
    SInt32 c_v;

    if (gChannelStatus[voice].Status == 1 && gChannelStatus[voice].OwnerProc) {
        gChannelStatus[voice].OwnerProc(voice, gChannelStatus[voice].Owner, 3);
    }

    gChannelStatus[voice].OwnerProc = 0;
    core = voice / 24;
    c_v = voice % 24;
    if ((gAwaitingKeyOn[core] & (1 << (c_v))) != 0) {
        gAwaitingKeyOn[core] &= ~(1 << c_v);
    }

    gAwaitingKeyOff[core] |= 1 << c_v;
}

void snd_KeyOffVoicesEx(VoiceFlags *voices, BOOL do_owner_proc) {
    SInt32 count;
    SInt32 core;
    SInt32 c_v;

    for (count = 0; count < 48; ++count) {
        core = count / 24;
        c_v = count % 24;
        if ((voices->core[core] & (1 << (c_v))) != 0 && gChannelStatus[count].Status == 1) {
            gAwaitingKeyOff[core] |= 1 << c_v;
            if (do_owner_proc && gChannelStatus[count].OwnerProc) {
                gChannelStatus[count].OwnerProc(count, gChannelStatus[count].Owner, 3);
            }

            gChannelStatus[count].OwnerProc = 0;
            if ((gAwaitingKeyOn[core] & (1 << c_v)) != 0) {
                gAwaitingKeyOn[core] &= ~(1 << c_v);
            }
        }
    }
}

void snd_SilenceVoicesEx(VoiceFlags *voices, BOOL do_owner_proc) {
    SInt32 count;
    SInt32 core;
    SInt32 c_v;
    SInt32 intr_state;
    SInt32 dis;

    for (count = 0; count < 48; ++count) {
        core = count / 24;
        c_v = count % 24;
        if ((voices->core[core] & (1 << (c_v))) != 0) {
            if (do_owner_proc && gChannelStatus[count].Status == 1 && gChannelStatus[count].OwnerProc) {
                gChannelStatus[count].OwnerProc(count, gChannelStatus[count].Owner, 3);
            }

            gChannelStatus[count].OwnerProc = 0;
            if (gChannelStatus[count].Priority != 127) {
                snd_MarkVoiceFree(count);
            }

            dis = CpuSuspendIntr(&intr_state);
            sceSdSetParam(core | SD_VOICE(c_v) | SD_VP_VOLL, 0);
            sceSdSetParam(core | SD_VOICE(c_v) | SD_VP_VOLR, 0);
            if (!dis) {
                CpuResumeIntr(intr_state);
            }

            snd_KeyOffVoiceRaw(core, c_v);
        }
    }
}

void snd_PauseVoices(VoiceFlags *voices) {
    SInt32 count;
    SInt32 core;
    SInt32 c_v;
    SInt32 intr_state;
    SInt32 dis;

    for (count = 0; count < 48; ++count) {
        core = count / 24;
        c_v = count % 24;
        if ((voices->core[core] & (1 << (c_v))) != 0) {
            dis = CpuSuspendIntr(&intr_state);
            sceSdSetParam(core | SD_VOICE(c_v) | SD_VP_VOLL, 0);
            sceSdSetParam(core | SD_VOICE(c_v) | SD_VP_VOLR, 0);
            sceSdSetParam(core | SD_VOICE(c_v) | SD_VP_PITCH, 0);
            if (!dis) {
                CpuResumeIntr(intr_state);
            }
        }
    }
}

void snd_PauseVoicesOwnedWithOwner(GSoundHandlerPtr owner) {
    SInt32 count;
    SInt32 core;
    SInt32 c_v;
    SInt32 intr_state;
    SInt32 dis;

    snd_LockVoiceAllocatorEx(1, 0x7Au);
    for (count = 0; count < 48; ++count) {
        if (gChannelStatus[count].Status && gChannelStatus[count].Owner == owner) {
            core = count / 24;
            c_v = count % 24;
            dis = CpuSuspendIntr(&intr_state);
            sceSdSetParam((core) | SD_VOICE(c_v) | SD_VP_VOLL, 0);
            sceSdSetParam((core) | SD_VOICE(c_v) | SD_VP_VOLR, 0);
            sceSdSetParam((core) | SD_VOICE(c_v) | SD_VP_PITCH, 0);
            if (!dis) {
                CpuResumeIntr(intr_state);
            }
        }
    }

    snd_UnlockVoiceAllocator();
}

void snd_UnPauseVoices(VoiceFlags *voices) {
    SInt32 note;
    SInt32 fine;
    SInt32 count;
    SInt32 core;
    SInt32 c_v;
    UInt32 pitch;
    SInt32 intr_state;
    SInt32 dis;

    for (count = 0; count < 48; ++count) {
        core = count / 24;
        c_v = count % 24;
        if ((voices->core[core] & (1 << (c_v))) != 0) {
            dis = CpuSuspendIntr(&intr_state);

            sceSdSetParam(
                core | SD_VOICE(c_v) | SD_VP_VOLL,
                (UInt16)snd_AdjustVolToGroup(gChannelStatus[count].Volume.left, gChannelStatus[count].VolGroup) >> 1);
            sceSdSetParam(
                core | SD_VOICE(c_v) | SD_VP_VOLR,
                (UInt16)snd_AdjustVolToGroup(gChannelStatus[count].Volume.right, gChannelStatus[count].VolGroup) >> 1);

            snd_PitchBendTone(gChannelStatus[count].Tone, gChannelStatus[count].Current_PB,
                              gChannelStatus[count].Current_PM, gChannelStatus[count].StartNote,
                              gChannelStatus[count].StartFine, &note, &fine);
            pitch = PS1Note2Pitch(gChannelStatus[count].Tone->CenterNote, gChannelStatus[count].Tone->CenterFine, note,
                                  fine);
            sceSdSetParam(core | SD_VOICE(c_v) | SD_VP_PITCH, pitch);
            if (!dis) {
                CpuResumeIntr(intr_state);
            }
        }
    }
}

void snd_UnPauseVoicesOwnedWithOwner(GSoundHandlerPtr owner) {
    SInt32 note;
    SInt32 fine;
    SInt32 count;
    SInt32 core;
    SInt32 c_v;
    UInt32 pitch;
    SInt32 intr_state;
    SInt32 dis;

    snd_LockVoiceAllocatorEx(1, 0x7Bu);
    for (count = 0; count < 48; ++count) {
        if (gChannelStatus[count].Status && gChannelStatus[count].Owner == owner) {
            core = count / 24;
            c_v = count % 24;
            dis = CpuSuspendIntr(&intr_state);

            sceSdSetParam(
                core | SD_VOICE(c_v) | SD_VP_VOLL,
                (UInt16)snd_AdjustVolToGroup(gChannelStatus[count].Volume.left, gChannelStatus[count].VolGroup) >> 1);
            sceSdSetParam(
                core | SD_VOICE(c_v) | SD_VP_VOLR,
                (UInt16)snd_AdjustVolToGroup(gChannelStatus[count].Volume.right, gChannelStatus[count].VolGroup) >> 1);
            snd_PitchBendTone(gChannelStatus[count].Tone, gChannelStatus[count].Current_PB,
                              gChannelStatus[count].Current_PM, gChannelStatus[count].StartNote,
                              gChannelStatus[count].StartFine, &note, &fine);
            pitch = PS1Note2Pitch(gChannelStatus[count].Tone->CenterNote, gChannelStatus[count].Tone->CenterFine, note,
                                  fine);
            sceSdSetParam(core | SD_VOICE(c_v) | SD_VP_PITCH, pitch);
            if (!dis) {
                CpuResumeIntr(intr_state);
            }
        }
    }

    snd_UnlockVoiceAllocator();
}

VoiceAttributes *snd_GetVoiceStatusPtr(SInt32 voice) { return &gChannelStatus[voice]; }

UInt16 PS1Note2Pitch(SInt8 center_note, SInt8 center_fine, UInt16 note, SInt16 fine) {
    UInt32 pitch;
    BOOL ps1;

    if (center_note < 0) {
        ps1 = 0;
        center_note = center_note * -1;
    } else {
        ps1 = 1;
    }

    pitch = sceSdNote2Pitch(center_note, center_fine, note, fine);
    if (ps1) {
        pitch = 44100 * pitch / 48000;
    }

    return pitch;
}

void snd_PitchBendTone(TonePtr tone, SInt32 pb, SInt32 pm, SInt32 note, SInt32 fine, SInt32 *new_note,
                       SInt32 *new_fine) {
    SInt32 pitch;

    pitch = (note * 128) + fine;
    pitch += pm;
    pitch += pb < 0 ? tone->PBLow * 128 * pb / -0x8000 * -1 : tone->PBHigh * 128 * pb / 0x7FFF;

    *new_note = pitch / 128;
    *new_fine = pitch % 128;
}

void snd_MarkVoicePlaying(SInt32 voice) {
    VoiceAttributes *va;
    VoiceAttributes *walk;

    va = &gChannelStatus[voice];
    va->Status = 1;
    va->playlist = 0;
    if (!gPlayingListHead) {
        gPlayingListHead = va;
    } else {
        walk = gPlayingListHead;
        if (va == gPlayingListHead) {
            snd_ShowError(82, 0, 0, 0, 0);
            return;
        }

        while (walk->playlist && walk->playlist != va) {
            walk = walk->playlist;
        }

        if (walk->playlist == va) {
            snd_ShowError(82, 0, 0, 0, 0);
            return;
        } else {
            walk->playlist = va;
        }
    }
}

void snd_MarkVoiceFree(SInt32 voice) {
    VoiceAttributes *va;
    UInt32 core;
    VoiceAttributes *walk;

    core = voice / 24;
    va = &gChannelStatus[voice];
    va->Status = 0;
    va->Owner = 0;
    if (va == gPlayingListHead) {
        gPlayingListHead = va->playlist;
    } else {
        walk = gPlayingListHead;
        while (walk && walk->playlist != va) {
            walk = walk->playlist;
        }

        if (walk) {
            walk->playlist = va->playlist;
        }
    }

    if (gNoiseOwnerVoice[core] == voice) {
        gNoiseOwnerVoice[core] = -1;
        gNoiseOwnerPriority[core] = -1;
    }
}

void snd_HardFreeVoice(SInt32 voice) { snd_MarkVoiceFree(voice); }

void snd_KeyOnVoiceRaw(int core, int voice, BOOL reverb) {
    gAwaitingKeyOn[core] |= 1 << voice;
    if (reverb) {
        gReverbVoices[core] |= 1 << voice;
    } else {
        gReverbVoices[core] &= ~(1 << voice);
    }
}

void snd_KeyOffVoiceRaw(int core, int voice) {
    gAwaitingKeyOn[core] &= ~(1 << voice);
    gAwaitingKeyOff[core] |= 1 << voice;
}