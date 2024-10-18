#include "common.h"
#include "functions.h"
#include "globals.h"
#include "intrman.h"
#include "libsd.h"
#include "sdmacro.h"

/* data 139c */ SInt32 gMovieDMAChannel = -1;
/* data 13a0 */ void *gMovieSoundBuffer = NULL;
/* data 13a4 */ UInt32 gMovieSoundSPUBuffer = 0;
/* data 13a8 */ UInt32 gMovieSPUBufferSize = 0;
/* data 13ac */ SInt32 gMovieSoundBufferSize = 0;
/* data 13b0 */ SInt32 gMovieVol = 0;
/* data 13b4 */ SInt32 gMoviePan = 0;
/* data 13b8 */ SInt32 gMovieVolGroup = 0;
/* data 13bc */ SInt32 gMovieSoundVoice[2] = {-1, -1};
/* data 13c4 */ BOOL gMovieSoundRunning = false;
/* data 13c8 */ SInt32 gMovieSoundType = 0;
/* data 13cc */ SInt32 gMovieSoundCh = 2;
/* data 13d0 */ UInt32 gMovieSoundSR = 48000;
/* data 13d4 */ UInt32 gMovieSoundPausePos = 0;
/* bss 8 */ SpuVolume gMovieSPUVol;

void *snd_InitMovieSoundEx(SInt32 buff_size, SInt32 spu_buff_size, SInt32 vol, SInt32 pan, SInt32 vol_group,
                           SInt32 type) {
    SInt32 msg;

    msg = 0;

    if (gMovieSoundBuffer) {
        snd_ShowError(112, 0, 0, 0, 0);
        return NULL;
    }

    if (buff_size % 4) {
        snd_ShowError(30, 0, 0, 0, 0);
        return NULL;
    }

    gMovieSoundBuffer = gAllocProc(buff_size, 6, 0);
    gMovieSoundBufferSize = buff_size;
    gMovieSoundType = type;
    if (!gMovieSoundBuffer) {
        snd_ShowError(31, buff_size, 0, 0, 0);
        return 0;
    }

    if (type == 3) {
        snd_LockVoiceAllocatorEx(1, 58);
        gMovieSoundVoice[0] = snd_AllocateVoice(vol_group, 127);
        gMovieSoundVoice[1] = snd_AllocateVoice(vol_group, 127);
        snd_UnlockVoiceAllocator();
        if (gMovieSoundVoice[0] == -1 || gMovieSoundVoice[1] == -1) {
            snd_ShowError(104, 0, 0, 0, 0);
            gFreeProc(gMovieSoundBuffer);
            if (gMovieSoundVoice[0] != -1) {
                snd_HardFreeVoice(gMovieSoundVoice[0]);
                gMovieSoundVoice[0] = -1;
            }
            if (gMovieSoundVoice[1] != -1) {
                snd_HardFreeVoice(gMovieSoundVoice[1]);
                gMovieSoundVoice[1] = -1;
            }
            return 0;
        }

        gMovieSPUBufferSize = spu_buff_size ? spu_buff_size : 4 * buff_size;
        gMovieSoundSPUBuffer = snd_SRAMMalloc(2 * gMovieSPUBufferSize);

        if (!gMovieSoundSPUBuffer) {
            snd_ShowError(103, 2 * gMovieSPUBufferSize, 0, 0, 0);
            gFreeProc(gMovieSoundBuffer);
            if (gMovieSoundVoice[0] != -1) {
                snd_HardFreeVoice(gMovieSoundVoice[0]);
                gMovieSoundVoice[0] = -1;
            }
            if (gMovieSoundVoice[1] != -1) {
                snd_HardFreeVoice(gMovieSoundVoice[1]);
                gMovieSoundVoice[1] = -1;
            }
            return 0;
        }
    }

    while ((gMovieDMAChannel = snd_GetFreeSPUDMA()) == -1) {
        if (!msg) {
            msg = 1;
        }
        while (gMovieDMAChannel < 1000) {
            ++gMovieDMAChannel;
        }
    }

    sceSdSetTransIntrHandler(gMovieDMAChannel, 0, 0);
    gMovieVol = (127 * vol) >> 10;
    gMoviePan = pan;
    gMovieVolGroup = vol_group;
    gMovieSoundPausePos = 0;
    snd_ClearMovieSoundBuffer();
    gMovieSoundRunning = 0;
    gMovieSPUVol.left = 0;
    gMovieSPUVol.right = 0;
    return gMovieSoundBuffer;
}

void snd_UpdateMovieADPCM(SInt32 data_size, UInt32 offset) {
    sceSdVoiceTrans(gMovieDMAChannel, 0, gMovieSoundBuffer, gMovieSoundSPUBuffer + offset, data_size);
    sceSdVoiceTransStatus(gMovieDMAChannel, 1);
}

UInt32 snd_GetMovieNAX() {
    SInt32 core;
    SInt32 voice;
    UInt32 addr;

    core = gMovieSoundVoice[0] / 24;
    voice = gMovieSoundVoice[0] % 24;

    addr = sceSdGetAddr(core | SD_VOICE(voice) | SD_VA_NAX);

    return addr - gMovieSoundSPUBuffer;
}

void snd_ClearMovieSoundBuffer() {
    SInt32 i;
    SInt32 len;
    SInt32 *walk;

    if (gMovieSoundType != 3) {
        walk = gMovieSoundBuffer;
        len = gMovieSoundBufferSize / 4;
        for (i = 0; i < len; ++i)
            walk[i] = 0;
        sceSdBlockTrans(gMovieDMAChannel, SD_TRANS_MODE_WRITE, gMovieSoundBuffer, 1024);
    }
}

void snd_CloseMovieSound() {
    snd_ResetMovieSound();
    if (gMovieDMAChannel == 0 || gMovieDMAChannel == 1) {
        snd_FreeSPUDMA(gMovieDMAChannel);
    }
    gMovieDMAChannel = -1;
    gMovieSoundRunning = 0;
    if (gMovieSoundBuffer) {
        gFreeProc(gMovieSoundBuffer);
    }

    if (gMovieSoundSPUBuffer) {
        snd_SRAMFree(gMovieSoundSPUBuffer, 2 * gMovieSPUBufferSize);
    }

    if (gMovieSoundVoice[0] != -1) {
        snd_HardFreeVoice(gMovieSoundVoice[0]);
        gMovieSoundVoice[0] = -1;
    }

    if (gMovieSoundVoice[1] != -1) {
        snd_HardFreeVoice(gMovieSoundVoice[1]);
        gMovieSoundVoice[1] = -1;
    }
    gMovieSoundBuffer = 0;
}

void snd_ResetMovieSound() {
    UInt32 core;
    UInt32 c_v;
    SInt32 intr_state;
    SInt32 dis;

    if (gMovieDMAChannel == -1) {
        return;
    }

    if (gMovieSoundType != 3) {
        dis = CpuSuspendIntr(&intr_state);
        sceSdSetParam(gMovieDMAChannel | SD_P_BVOLL, 0);
        sceSdSetParam(gMovieDMAChannel | SD_P_BVOLR, 0);
        sceSdSetParam(gMovieDMAChannel | SD_P_MMIX, sceSdGetParam(gMovieDMAChannel | SD_P_MMIX) & 0xFF3F);
        if (!dis) {
            CpuResumeIntr(intr_state);
        }
        sceSdBlockTrans(gMovieDMAChannel, 2, 0, 0);
    } else {
        core = gMovieSoundVoice[0] / 24;
        c_v = gMovieSoundVoice[0] % 24;
        snd_KeyOffVoiceRaw(core, c_v);
        core = gMovieSoundVoice[1] / 24;
        c_v = gMovieSoundVoice[1] % 24;
        snd_KeyOffVoiceRaw(core, c_v);
    }

    gMovieSPUVol.left = 0;
    gMovieSPUVol.right = 0;
    gMovieSoundRunning = 0;
}

void snd_StartMovieSoundEx(void *buff, SInt32 buff_size, SInt32 offset, UInt32 sr, SInt32 ch) {
    SInt32 vol;
    SInt32 ret;
    UInt32 core;
    UInt32 c_v;
    SInt32 intr_state;
    SInt32 dis;

    gMovieSPUVol.left = 0;
    gMovieSPUVol.right = 0;
    snd_CalcStereoBalance3d(gMovieVol, gMoviePan, &gMovieSPUVol);

    if (gMovieSoundType != 3) {
        ret = sceSdBlockTrans(gMovieDMAChannel, SD_TRANS_MODE_WRITE_FROM | SD_BLOCK_LOOP, buff, buff_size, offset);
        dis = CpuSuspendIntr(&intr_state);

        vol = snd_AdjustVolToGroup(gMovieSPUVol.left, gMovieVolGroup);
        sceSdSetParam(gMovieDMAChannel | SD_P_BVOLL, vol);
        vol = snd_AdjustVolToGroup(gMovieSPUVol.right, gMovieVolGroup);
        sceSdSetParam(gMovieDMAChannel | SD_P_BVOLR, vol);
        sceSdSetParam(gMovieDMAChannel | SD_P_MMIX, sceSdGetParam(gMovieDMAChannel | SD_P_MMIX) | 0xC0);
        if (!dis) {
            CpuResumeIntr(intr_state);
        }
    } else {
        gMovieSoundCh = ch;
        gMovieSoundSR = sr;
        dis = CpuSuspendIntr(&intr_state);
        core = gMovieSoundVoice[0] / 24;
        c_v = gMovieSoundVoice[0] % 24;

        sceSdSetParam(core | SD_VOICE(c_v) | SD_VP_ADSR1, 0xFu);
        sceSdSetParam(core | SD_VOICE(c_v) | SD_VP_ADSR2, 0x1FC0u);
        sceSdSetParam(core | SD_VOICE(c_v) | SD_VP_PITCH, ((sr * 0x1000) / 48000));
        sceSdSetAddr(core | SD_VOICE(c_v) | SD_VA_SSA, gMovieSoundSPUBuffer + gMovieSoundPausePos);

        sceSdSetSwitch(core | SD_S_VMIXL, sceSdGetSwitch(core | SD_S_VMIXL) | (1 << c_v));
        sceSdSetSwitch(core | SD_S_VMIXR, sceSdGetSwitch(core | SD_S_VMIXR) | (1 << c_v));
        sceSdSetParam(core | SD_VOICE(c_v) | SD_VP_VOLL,
                      (UInt16)snd_AdjustVolToGroup(gMovieSPUVol.left, gMovieVolGroup) >> 1);

        if (ch >= 2) {
            sceSdSetParam(core | SD_VOICE(c_v) | SD_VP_VOLR, 0);
        } else {
            sceSdSetParam(core | SD_VOICE(c_v) | SD_VP_VOLR,
                          (UInt16)snd_AdjustVolToGroup(gMovieSPUVol.right, gMovieVolGroup) >> 1);
        }

        if (ch >= 2) {
            core = gMovieSoundVoice[1] / 24;
            c_v = gMovieSoundVoice[1] % 24;
            sceSdSetParam(core | SD_VOICE(c_v) | SD_VP_ADSR1, 0xFu);
            sceSdSetParam(core | SD_VOICE(c_v) | SD_VP_ADSR2, 0x1FC0u);
            sceSdSetParam(core | SD_VOICE(c_v) | SD_VP_PITCH, ((sr * 0x1000) / 48000));
            sceSdSetAddr(core | SD_VOICE(c_v) | SD_VA_SSA,
                         gMovieSoundSPUBuffer + gMovieSPUBufferSize + gMovieSoundPausePos);

            sceSdSetSwitch(core | SD_S_VMIXL, sceSdGetSwitch(core | SD_S_VMIXL) | (1 << c_v));
            sceSdSetSwitch(core | SD_S_VMIXR, sceSdGetSwitch(core | SD_S_VMIXR) | (1 << c_v));
            sceSdSetParam(core | SD_VOICE(c_v) | SD_VP_VOLL, 0);
            sceSdSetParam(core | SD_VOICE(c_v) | SD_VP_VOLR,
                          (UInt16)snd_AdjustVolToGroup(gMovieSPUVol.right, gMovieVolGroup) >> 1);

            snd_KeyOnVoiceRaw(core, c_v, 0);
        }

        core = gMovieSoundVoice[0] / 24;
        c_v = gMovieSoundVoice[0] % 24;
        snd_KeyOnVoiceRaw(core, c_v, 0);

        if (!dis) {
            CpuResumeIntr(intr_state);
        }
    }

    gMovieSoundRunning = true;
}

void snd_AdjustMovieVolDueToMasterChange(SInt32 which) {
    SInt32 vol;
    SInt32 intr_state;
    SInt32 dis;
    UInt32 core;
    UInt32 c_v;

    if (!gMovieSoundRunning || which != gMovieVolGroup) {
        return;
    }

    snd_CalcStereoBalance3d(gMovieVol, gMoviePan, &gMovieSPUVol);
    dis = CpuSuspendIntr(&intr_state);

    if (gMovieSoundType != 3) {
        vol = snd_AdjustVolToGroup(gMovieSPUVol.left, gMovieVolGroup);
        sceSdSetParam(gMovieDMAChannel | SD_P_BVOLL, vol);
        vol = snd_AdjustVolToGroup(gMovieSPUVol.right, gMovieVolGroup);
        sceSdSetParam(gMovieDMAChannel | SD_P_BVOLR, vol);
    } else {
        core = gMovieSoundVoice[0] / 24;
        c_v = gMovieSoundVoice[0] % 24;
        sceSdSetParam(core | SD_VOICE(c_v) | SD_VP_VOLL,
                      (UInt16)snd_AdjustVolToGroup(gMovieSPUVol.left, gMovieVolGroup) >> 1);

        if (gMovieSoundCh >= 2) {
            sceSdSetParam(core | SD_VOICE(c_v) | SD_VP_VOLR, 0);

            core = gMovieSoundVoice[1] / 24;
            c_v = gMovieSoundVoice[1] % 24;

            sceSdSetParam(core | SD_VOICE(c_v) | SD_VP_VOLL, 0);

            sceSdSetParam(core | SD_VOICE(c_v) | SD_VP_VOLR,
                          (UInt16)snd_AdjustVolToGroup(gMovieSPUVol.right, gMovieVolGroup) >> 1);
        } else {
            sceSdSetParam(core | SD_VOICE(c_v) | SD_VP_VOLR,
                          (UInt16)snd_AdjustVolToGroup(gMovieSPUVol.right, gMovieVolGroup) >> 1);
        }
    }

    if (!dis) {
        CpuResumeIntr(intr_state);
    }
}

void snd_SetMovieSoundVolPan(SInt32 vol, SInt32 pan) {
    SInt32 intr_state;
    SInt32 dis;
    UInt32 core;
    UInt32 c_v;

    gMovieVol = (127 * vol) >> 10;
    gMoviePan = pan;

    if (gMovieDMAChannel == -1 || !gMovieSoundRunning) {
        return;
    }

    dis = CpuSuspendIntr(&intr_state);
    snd_CalcStereoBalance3d(gMovieVol, gMoviePan, &gMovieSPUVol);

    if (gMovieSoundType != 3) {
        vol = snd_AdjustVolToGroup(gMovieSPUVol.left, gMovieVolGroup);
        sceSdSetParam(gMovieDMAChannel | SD_P_BVOLL, vol);
        vol = snd_AdjustVolToGroup(gMovieSPUVol.right, gMovieVolGroup);
        sceSdSetParam(gMovieDMAChannel | SD_P_BVOLR, vol);
    } else {
        core = gMovieSoundVoice[0] / 24;
        c_v = gMovieSoundVoice[0] % 24;
        sceSdSetParam(core | SD_VOICE(c_v) | SD_VP_VOLL,
                      (UInt16)snd_AdjustVolToGroup(gMovieSPUVol.left, gMovieVolGroup) >> 1);

        if (gMovieSoundCh >= 2) {
            sceSdSetParam(core | SD_VOICE(c_v) | SD_VP_VOLR, 0);
            core = gMovieSoundVoice[1] / 24;
            c_v = gMovieSoundVoice[1] % 24;
            sceSdSetParam(core | SD_VOICE(c_v) | SD_VP_VOLL, 0);
            sceSdSetParam(core | SD_VOICE(c_v) | SD_VP_VOLR,
                          (UInt16)snd_AdjustVolToGroup(gMovieSPUVol.right, gMovieVolGroup) >> 1);
        } else {
            sceSdSetParam(core | SD_VOICE(c_v) | SD_VP_VOLR,
                          (UInt16)snd_AdjustVolToGroup(gMovieSPUVol.right, gMovieVolGroup) >> 1);
        }
    }

    if (!dis) {
        CpuResumeIntr(intr_state);
    }
}

UInt32 snd_GetMovieBufferPos() {
    UInt32 ret;

    ret = sceSdBlockTransStatus(gMovieDMAChannel, 0);

    return ret;
}

SInt32 snd_PauseMovieSound() {
    SInt32 ret;
    UInt32 core;
    UInt32 c_v;
    SInt32 intr_state;
    SInt32 dis;

    if (gMovieSoundType != 3) {
        dis = CpuSuspendIntr(&intr_state);
        sceSdSetParam(gMovieDMAChannel | 0xF80, 0);
        sceSdSetParam(gMovieDMAChannel | 0x1080, 0);

        if (!dis) {
            CpuResumeIntr(intr_state);
        }

        ret = sceSdBlockTrans(gMovieDMAChannel, SD_TRANS_MODE_STOP, 0, 0) & 0xFFFFFF;
    } else {
        gMovieSoundPausePos = snd_GetMovieNAX();

        core = gMovieSoundVoice[0] / 24;
        c_v = gMovieSoundVoice[0] % 24;
        snd_KeyOffVoiceRaw(core, c_v);

        core = gMovieSoundVoice[1] / 24;
        c_v = gMovieSoundVoice[1] % 24;
        snd_KeyOffVoiceRaw(core, c_v);

        ret = gMovieSoundPausePos;
    }

    return ret;
}
