#include "common.h"
#include "functions.h"
#include "intrman.h"
#include "libsd.h"
#include "sdmacro.h"
#include "thread.h"

/* data 1bc0 */ SInt32 gReverbType[2] = {-1, -1};
/* data 1bc8 */ UInt32 gRevRamStart[2] = {0, 0};
/* data 1bd0 */ SInt32 gRevRamPreAllocType[2] = {-1, -1};
/* data 1bd8 */ SInt16 gReverbDepthL[2] = {0, 0};
/* data 1bdc */ SInt16 gReverbDepthR[2] = {0, 0};
/* data 1be0 */ SInt32 gAwaitingReverbChange[2] = {0, 0};
/* data 1be8 */ SInt32 gAwaitingReverbChangeTick[2] = {0, 0};
/* data 1bf0 */ SInt32 gReverbMode = 0;
/* data 1bf4 */ UInt32 gRevRamUse[] = {
    0x00000080, 0x000026C0, 0x00001F40, 0x00004840, 0x00006FE0,
    0x0000ADE0, 0x0000F6C0, 0x00018040, 0x00018040, 0x00003C00,
};

void snd_SetReverbMode(SInt32 reverb_mode) {
    UInt16 core1mode;

    gReverbMode = reverb_mode;

    core1mode = sceSdGetParam(SD_P_MMIX | SD_CORE_1);
    if (!gReverbMode) {
        core1mode &= ~3u;
    } else {
        core1mode |= 3u;
    }
    sceSdSetParam(SD_P_MMIX | SD_CORE_1, core1mode);
}

void snd_PreAllocReverbWorkArea(UInt32 core, SInt32 type) {
    if (core & 1) {
        snd_PreAllocReverbWorkArea_L(0, type);
    }

    if (core & 2) {
        snd_PreAllocReverbWorkArea_L(1, type);
    }
}

void snd_PreAllocReverbWorkArea_L(SInt32 core, SInt32 type) {
    SInt32 dis;
    SInt32 oldstat;

    if (gReverbType[core] > 0) {
        snd_SetReverbType_L(core, 0, 0, 0);
    }

    if (gRevRamPreAllocType[core] > 0) {
        dis = CpuSuspendIntr(&oldstat);
        snd_SRAMFree(gRevRamStart[core], gRevRamUse[gRevRamPreAllocType[core]]);
        if (!dis) {
            CpuResumeIntr(oldstat);
        }
    }
    gRevRamPreAllocType[core] = type;

    if (type > 0) {
        dis = CpuSuspendIntr(&oldstat);
        if (!(gRevRamStart[core] = snd_SRAMMallocRev(gRevRamUse[type]))) {
            if (!dis) {
                CpuResumeIntr(oldstat);
            }
            gReverbType[core] = -1;
            gRevRamPreAllocType[core] = -1;
            snd_ShowError(85, 0, 0, 0, 0);
            return;
        }

        if (!dis) {
            CpuResumeIntr(oldstat);
        }
    }
}

void snd_SetReverbEx(UInt32 core, SInt32 type, SInt32 depth, SInt32 delay, SInt32 feedback) {
    if (core & 1) {
        snd_SetReverbType_L(0, type, delay, feedback);
        snd_SetReverbDepth_L(0, depth, depth);
    }

    if (core & 2) {
        snd_SetReverbType_L(1, type, delay, feedback);
        snd_SetReverbDepth_L(1, depth, depth);
    }
}

void snd_SetReverbType(SInt32 core, SInt32 type) {
    SInt32 ret1;
    SInt32 ret2;

    ret1 = 0;
    ret2 = 0;

    if ((core & 1) != 0) {
        ret1 = snd_SetReverbType_L(0, type, 0, 0);
    }

    if ((core & 2) != 0) {
        ret2 = snd_SetReverbType_L(1, type, 0, 0);
    }
}

// INCLUDE_ASM("asm/nonmatchings/reverb", snd_SetReverbType_L);
SInt32 snd_SetReverbType_L(SInt32 core, SInt32 type, SInt32 delay, SInt32 feedback) {
    sceSdEffectAttr ef;
    SInt32 ch;
    SInt32 dis;
    SInt32 oldstat;

    if (gReverbType[core] == type) {
        ef.core = core;
        ef.mode = type;
        ef.depth_L = 0;
        ef.depth_R = 0;
        ef.delay = delay;
        ef.feedback = feedback;
        sceSdSetEffectAttr(core, &ef);
        return 0;
    }

    if (gRevRamPreAllocType[core] > 0 && gRevRamUse[gRevRamPreAllocType[core]] < gRevRamUse[type]) {
        snd_ShowError(87, 0, 0, 0, 0);
        return -13;
    }

    if (gReverbType[core] > 0) {
        dis = CpuSuspendIntr(&oldstat);
        sceSdSetCoreAttr(core | SD_C_EFFECT_ENABLE, 0);
        sceSdSetParam(core | SD_P_EVOLL, 0);
        sceSdSetParam(core | SD_P_EVOLR, 0);
        if (!dis) {
            CpuResumeIntr(oldstat);
        }
    }

    if (gReverbType[core] > 0 && gRevRamPreAllocType[core] <= 0) {
        dis = CpuSuspendIntr(&oldstat);
        snd_SRAMFree(gRevRamStart[core], gRevRamUse[gReverbType[core]]);
        if (!dis) {
            CpuResumeIntr(oldstat);
        }
    }

    if (gRevRamPreAllocType[core] <= 0) {
        dis = CpuSuspendIntr(&oldstat);
        if (type && !(gRevRamStart[core] = snd_SRAMMallocRev(gRevRamUse[type]))) {
            if (!dis) {
                CpuResumeIntr(oldstat);
            }
            gReverbType[core] = -1;
            snd_ShowError(86, 0, 0, 0, 0);
            return -11;
        }

        if (!dis) {
            CpuResumeIntr(oldstat);
        }
    }

    snd_KillAutoVerbForCore(core);
    if (type) {
        sceSdSetAddr(core | SD_A_EEA, gRevRamStart[core] - (1 - gRevRamUse[type]));
    }

    ef.core = core;
    ef.mode = type;
    ef.depth_L = 0;
    ef.depth_R = 0;
    ef.delay = delay;
    ef.feedback = feedback;
    sceSdSetEffectAttr(core, &ef);

    if (type) {
        while ((ch = snd_GetFreeSPUDMA()) == -1) {
            DelayThread(33333);
        }
        gAwaitingReverbChange[core] = ch + 1;
        gAwaitingReverbChangeTick[core] = 10;
        sceSdClearEffectWorkArea(core, ch, type);
    }

    gReverbType[core] = type;

    return 0;
}

void snd_SetReverbDepth(SInt32 core, SInt32 left, SInt32 right) {
    if (core & 1) {
        snd_KillAutoVerbForCore(0);
        snd_SetReverbDepth_L(0, left, right);
    }

    if (core & 2) {
        snd_KillAutoVerbForCore(1);
        snd_SetReverbDepth_L(1, left, right);
    }
}

void snd_SetReverbDepth_L(SInt32 core, SInt32 left, SInt32 right) {
    SInt32 intr_state;
    SInt32 dis;

    gReverbDepthL[core] = left;
    gReverbDepthR[core] = right;
    if (!gAwaitingReverbChange[core]) {
        dis = CpuSuspendIntr(&intr_state);
        sceSdSetParam(core | SD_P_EVOLL, gReverbDepthL[core]);
        sceSdSetParam(core | SD_P_EVOLR, gReverbDepthR[core]);
        if (!dis) {
            CpuResumeIntr(intr_state);
        }
    }
}

void snd_CheckVerb() {
    SInt32 intr_state;
    SInt32 dis;

    if (gAwaitingReverbChangeTick[0]) {
        gAwaitingReverbChangeTick[0]--;
        if (!gAwaitingReverbChangeTick[0]) {
            dis = CpuSuspendIntr(&intr_state);
            sceSdSetParam(SD_P_EVOLL | SD_CORE_0, gReverbDepthL[0]);
            sceSdSetParam(SD_P_EVOLR | SD_CORE_0, gReverbDepthR[0]);
            sceSdSetCoreAttr(SD_C_EFFECT_ENABLE | SD_CORE_0, 1);
            if (!dis) {
                CpuResumeIntr(intr_state);
            }
            snd_FreeSPUDMA(gAwaitingReverbChange[0] - 1);
            gAwaitingReverbChange[0] = 0;
        }
    }

    if (gAwaitingReverbChangeTick[1]) {
        gAwaitingReverbChangeTick[1]--;
        if (!gAwaitingReverbChangeTick[1]) {
            dis = CpuSuspendIntr(&intr_state);
            sceSdSetParam(SD_P_EVOLL | SD_CORE_1, gReverbDepthL[1]);
            sceSdSetParam(SD_P_EVOLR | SD_CORE_1, gReverbDepthR[1]);
            sceSdSetCoreAttr(SD_C_EFFECT_ENABLE | SD_CORE_1, 1);
            if (!dis) {
                CpuResumeIntr(intr_state);
            }
            snd_FreeSPUDMA(gAwaitingReverbChange[1] - 1);
            gAwaitingReverbChange[1] = 0;
        }
    }
    snd_AutoVerbHandler();
}
