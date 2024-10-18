#include "common.h"
#include "functions.h"
#include "globals.h"
#include "libsd.h"
#include "sdmacro.h"
#include "thread.h"
#include "timerman.h"

/* data 12e0 */ SInt32 gSTickSema = -1;
/* data 12e4 */ SInt32 gTimerThreadID = 0;
/* data 12e8 */ SInt32 gTimerID = 0;
/* data 12ec */ SInt32 gTickTrigger = 0;
/* data 12f0 */ SInt32 gTickMode = -1;
/* data 12f4 */ UInt32 gMicsPerTick = 0;
/* data 12f8 */ SInt32 gTickRate = 0;
/* data 12fc */ SInt32 gPercent = 0;
/* data 1300 */ BOOL gPrefs_ShowMissingSounds = 1;
/* data 1304 */ BOOL gPrefs_Silent = 0;
/* bss 0 */ timercommon gTimerInfo;

SInt32 snd_SystemRunning() { return gTickMode != -1; }

SInt32 snd_StartSoundSystemEx(UInt32 flags) {
    SInt32 ret;
    SInt32 i;
    UInt16 val;

    if (gTickMode != -1) {
        return -1;
    }

    if (flags & 1) {
        gPrefs_ShowMissingSounds = 0;
    } else {
        gPrefs_ShowMissingSounds = 1;
    }

    if (flags & 2) {
        gPrefs_Silent = 1;
    } else {
        gPrefs_Silent = 0;
    }

    snd_SRAMInitMalloc();
    snd_InitLoader();
    snd_InitVoiceAllocator();
    snd_InitSoundHandlers();
    snd_InitDuckers();

    if (!(flags & 4)) {
        sceSdInit(0);
        sceSdSetParam(SD_P_MMIX | SD_CORE_0, 0xF00u);
        sceSdSetParam(SD_P_MMIX | SD_CORE_1, 0xF0Cu);
        sceSdSetParam(SD_P_MVOLL, 0x3FFFu);
        sceSdSetParam(SD_P_MVOLR, 0x3FFFu);
        sceSdSetParam(SD_P_EVOLL | SD_CORE_0, 0);
        sceSdSetParam(SD_P_EVOLR | SD_CORE_0, 0);
        sceSdSetParam(SD_P_EVOLL | SD_CORE_1, 0);
        sceSdSetParam(SD_P_EVOLR | SD_CORE_1, 0);
        sceSdSetParam(SD_P_AVOLL | SD_CORE_1, 0x7FFFu);
        sceSdSetParam(SD_P_AVOLR | SD_CORE_1, 0x7FFFu);
        val = 0;
        if ((flags & 8) != 0) {
            val |= SD_SPDIF_MEDIA_DVD;
        } else {
            val |= 0;
        }

        if ((flags & 0x10) != 0) {
            val |= 0;
        } else {
            val |= SD_SPDIF_COPY_PROHIBIT;
        }
        sceSdSetCoreAttr(SD_C_SPDIF_MODE, val);
        sceSdSetCoreAttr(SD_C_EFFECT_ENABLE | SD_CORE_0, 0);
        sceSdSetCoreAttr(SD_C_EFFECT_ENABLE | SD_CORE_1, 0);
    }

    gTickMode = 1;
    gTickRate = snd_GetTickRate();
    gMicsPerTick = 100000000 / (999 * gTickRate / 10);

    ret = snd_InstallSoundTick();
    if (ret) {
        gTickMode = -1;
        return ret;
    }

    snd_SetPlaybackMode(0);
    for (i = 0; i < 15; i++) {
        snd_SetMasterVolume(i, 0x400);
    }
    snd_SetMasterVolume(16, 0x400);
    gLockMasterTick = 0;

    return 0;
}

SInt32 snd_InstallSoundTick() {
    int ret;
    struct SysClock clock;
    struct ThreadParam param;
    struct SemaParam sema;

    sema.attr = 1;
    sema.initCount = 0;
    sema.maxCount = 1;
    gSTickSema = CreateSema(&sema);

    param.attr = 0x2000000;
    param.entry = snd_TimerLLIRQLoop;
    param.initPriority = gThreadPriority_TICK;
    param.stackSize = 0x1000;
    param.option = 0;
    gTimerThreadID = CreateThread(&param);

    gKillTimer = 0;
    USec2SysClock(1000000u, &clock);
    gTimerInfo.thid = &gTimerThreadID;
    gTimerInfo.compare = clock.low / 0xF0;
    gTimerID = AllocHardTimer(1, 32, 1);
    ret = SetTimerHandler(gTimerID, gTimerInfo.compare, snd_TimerLLTick, &gTimerInfo);
    ret = SetupHardTimer(gTimerID, 1, 0, 1);
    ret = StartHardTimer(gTimerID);
    if (gTimerThreadID > 0) {
        StartThread(gTimerThreadID, 0);
    }

    return 0;
}

void snd_RemoveSoundTick() {
    SInt32 ret;

    TerminateThread(gTimerThreadID);
    DeleteThread(gTimerThreadID);
    gKillTimer = 1;
    while (gKillTimer) {
        DelayThread(33000);
    }
    ret = StopHardTimer(gTimerID);
    FreeHardTimer(gTimerID);
}

SInt32 snd_StopSoundSystem() {
    gLockMasterTick = 1;
    snd_UnloadAllBanks();
    snd_UnloadAllMMD();
    snd_CleanupVoiceAllocator();
    snd_SetReverbDepth(3, 0, 0);
    snd_SetReverbType(3, 0);
    snd_CloseMovieSound();
    snd_CloseVAGStreaming();
    gAwaitingKeyOff[0] = 0xFFFFFF;
    gAwaitingKeyOff[1] = 0xFFFFFF;
    snd_StopPendingVoices();
    snd_RemoveSoundTick();
    sceSdSetSwitch(SD_S_VMIXL | SD_CORE_0, 0);
    sceSdSetSwitch(SD_S_VMIXR | SD_CORE_0, 0);
    sceSdSetSwitch(SD_S_VMIXL | SD_CORE_1, 0);
    sceSdSetSwitch(SD_S_VMIXR | SD_CORE_1, 0);
    snd_SetMasterVolume(16, 0);
    snd_WaitDMADone();
    snd_ShutdownLoader();
    gLockMasterTick = 0;
    gTickMode = -1;

    return 0;
}

SInt32 snd_GetTickRate() { return 240; }

SInt32 snd_TimerLLIRQLoop() {
    while (1) {
        SleepThread();
        snd_SoundTickHandler();
        SignalSema(gSTickSema);
    }

    return 0;
}
