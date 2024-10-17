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

INCLUDE_ASM("asm/nonmatchings/valloc", snd_StartVoices);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_StartPendingVoices);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_StopPendingVoices);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_GetKeyedOnVoices);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_GetKeyedOffVoices);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_VoiceIsDone);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_GetVoiceAge);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_GetVoiceStatus);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_GetVoicePriority);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_KeyOffVoice);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_KeyOffVoicesEx);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_SilenceVoicesEx);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_PauseVoices);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_PauseVoicesOwnedWithOwner);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_UnPauseVoices);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_UnPauseVoicesOwnedWithOwner);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_GetVoiceStatusPtr);

INCLUDE_ASM("asm/nonmatchings/valloc", PS1Note2Pitch);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_PitchBendTone);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_MarkVoicePlaying);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_MarkVoiceFree);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_HardFreeVoice);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_KeyOnVoiceRaw);

INCLUDE_ASM("asm/nonmatchings/valloc", snd_KeyOffVoiceRaw);
