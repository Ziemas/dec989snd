#include "common.h"
#include "functions.h"
#include "globals.h"
#include "intrman.h"
#include "libsd.h"

/* data 1e6c */ SInt32 gMasterVol[32] = {
    0x400, 0x400, 0x400, 0x400, 0x400, 0x400, 0x400, 0x400, 0x400, 0x400, 0x400, 0x400, 0x400, 0x400, 0x400, 0x400,
    0x400, 0x400, 0x400, 0x400, 0x400, 0x400, 0x400, 0x400, 0x400, 0x400, 0x400, 0x400, 0x400, 0x400, 0x400, 0x400,
};
/* data 1eec */ SInt32 gGroupDuckMult[32] = {
    0x10000, 0x10000, 0x10000, 0x10000, 0x10000, 0x10000, 0x10000, 0x10000, 0x10000, 0x10000, 0x10000,
    0x10000, 0x10000, 0x10000, 0x10000, 0x10000, 0x10000, 0x10000, 0x10000, 0x10000, 0x10000, 0x10000,
    0x10000, 0x10000, 0x10000, 0x10000, 0x10000, 0x10000, 0x10000, 0x10000, 0x10000, 0x10000,
};
/* data 1f6c */ SInt16 gStereoOrMono = 0;
/* data 1f70 */ SInt32 gCurrentXAVolGroup = -1;
/* data 1f74 */ SInt32 gCurrentXAVol = 0;
/* data 1f78 */ SInt32 gCurrentXAPan = 0;
/* bss 9d50 */ DuckerDef gDuckers[4];

void snd_InitDuckers() {
    SInt32 i;

    for (i = 0; i < 4; ++i) {
        gDuckers[i].source_group = -1;
    }

    for (i = 0; i < 15; ++i) {
        gGroupDuckMult[i] = 0x10000;
    }
}

void snd_SetMasterVolume(SInt32 which, SInt32 vol) {
    SInt32 intr_state;
    SInt32 dis;

    if (vol > 0x400) {
        vol = 0x400;
    }

    if (vol < 0) {
        vol = 0;
    }

    if (which == 15) {
        snd_ShowError(83, 0, 0, 0, 0);
        return;
    }

    gMasterVol[which] = vol;
    if (g989Monitor && g989Monitor->vol_changed) {
        g989Monitor->vol_changed(which, vol);
    }

    if (which != 16) {
        snd_AdjustAllChannelsMasterVolume(1 << which);
        snd_AdjustMovieVolDueToMasterChange(which);
    } else {
        dis = CpuSuspendIntr(&intr_state);
        sceSdSetParam(SD_CORE_1 | SD_P_MVOLL, (0x3FFF * vol / 1024));
        sceSdSetParam(SD_CORE_1 | SD_P_MVOLR, (0x3FFF * vol / 1024));
        if (!dis) {
            CpuResumeIntr(intr_state);
        }
    }
}

SInt32 snd_GetMasterVolume(SInt32 which) { return gMasterVol[which]; }

void snd_SetPlaybackMode(SInt32 mode) { gStereoOrMono = mode; }

SInt32 snd_GetPlaybackMode() { return gStereoOrMono; }

void snd_MakeVolumes(SInt32 snd_vol, SInt32 snd_pan, SInt32 prog_vol, SInt32 prog_pan, SInt32 tone_vol, SInt32 tone_pan,
                     SpuVolume *spu_vol) {
    SInt32 voll;
    SInt32 volr;
    SInt32 work_vol;

    work_vol = 0x102 * snd_vol;
    work_vol = (work_vol * tone_vol) / 127;
    work_vol = (work_vol * prog_vol) / 127;

    if (!work_vol) {
        spu_vol->left = 0;
        spu_vol->right = 0;
        return;
    }

    if (gStereoOrMono == 1) {
        spu_vol->left = work_vol;
        spu_vol->right = work_vol;
        return;
    }

    snd_pan += tone_pan + prog_pan;
    while (snd_pan >= 360) {
        snd_pan -= 360;
    }
    while (snd_pan < 0) {
        snd_pan += 360;
    }

    snd_pan = snd_pan < 270 ? snd_pan + 90 : snd_pan - 270;
    if (snd_pan < 180) {
        voll = gPanTable[snd_pan].left * work_vol / 0x3FFF;
        volr = gPanTable[snd_pan].right * work_vol / 0x3FFF;
        if (spu_vol->left < 0 && spu_vol->right < 0) {
            voll *= -1;
            volr *= -1;
        }

        if (spu_vol->left < 0 && spu_vol->right > -1 || spu_vol->left > -1 && spu_vol->right < 0) {
            if (abs(spu_vol->left) > abs(spu_vol->right)) {
                voll = (spu_vol->left / abs(spu_vol->left)) * voll;
                volr = (spu_vol->left / abs(spu_vol->left)) * volr;
            } else {
                voll = (spu_vol->right / abs(spu_vol->right)) * voll;
                volr = (spu_vol->right / abs(spu_vol->right)) * volr;
            }
        }
    } else {
        snd_pan -= 180;
        volr = gPanTable[snd_pan].left * work_vol / 0x3FFF;
        voll = gPanTable[snd_pan].right * work_vol / 0x3FFF;
        if (gStereoOrMono != 2) {
            if (spu_vol->left > -1 && spu_vol->right > -1) {
                if (volr < voll) {
                    volr *= -1;
                } else {
                    voll *= -1;
                }
            } else if (spu_vol->left < 0 && spu_vol->right < 0) {
                if (volr > voll) {
                    volr *= -1;
                } else {
                    voll *= -1;
                }
            } else {
                if (spu_vol->left < 0) {
                    voll *= -1;
                } else {
                    volr *= -1;
                }
            }
        }
    }

    spu_vol->left = voll;
    spu_vol->right = volr;
}

void snd_MakeVolumesB(SInt32 bank_vol, SInt32 snd_vol, SInt32 snd_pan, SInt32 prog_vol, SInt32 prog_pan,
                      SInt32 tone_vol, SInt32 tone_pan, SpuVolume *spu_vol) {
    SInt32 voll;
    SInt32 volr;
    SInt32 work_vol;

    work_vol = 0x102 * snd_vol;
    work_vol = (work_vol * tone_vol) / 127;
    work_vol = (work_vol * prog_vol) / 127;
    work_vol = (work_vol * bank_vol) / 127;

    if (!work_vol) {
        spu_vol->left = 0;
        spu_vol->right = 0;
        return;
    }

    if (gStereoOrMono == 1) {
        spu_vol->left = work_vol;
        spu_vol->right = work_vol;
        return;
    }

    snd_pan += tone_pan + prog_pan;
    while (snd_pan >= 360) {
        snd_pan -= 360;
    }
    while (snd_pan < 0) {
        snd_pan += 360;
    }

    snd_pan = snd_pan < 270 ? snd_pan + 90 : snd_pan - 270;
    if (snd_pan < 180) {
        voll = gPanTable[snd_pan].left * work_vol / 0x3FFF;
        volr = gPanTable[snd_pan].right * work_vol / 0x3FFF;
        if (spu_vol->left < 0 && spu_vol->right < 0) {
            voll *= -1;
            volr *= -1;
        }

        if (spu_vol->left < 0 && spu_vol->right > -1 || spu_vol->left > -1 && spu_vol->right < 0) {
            if (abs(spu_vol->left) > abs(spu_vol->right)) {
                voll = (spu_vol->left / abs(spu_vol->left)) * voll;
                volr = (spu_vol->left / abs(spu_vol->left)) * volr;
            } else {
                voll = (spu_vol->right / abs(spu_vol->right)) * voll;
                volr = (spu_vol->right / abs(spu_vol->right)) * volr;
            }
        }
    } else {
        snd_pan -= 180;
        volr = gPanTable[snd_pan].left * work_vol / 0x3FFF;
        voll = gPanTable[snd_pan].right * work_vol / 0x3FFF;
        if (gStereoOrMono != 2) {
            if (spu_vol->left > -1 && spu_vol->right > -1) {
                if (volr < voll) {
                    volr *= -1;
                } else {
                    voll *= -1;
                }
            } else if (spu_vol->left < 0 && spu_vol->right < 0) {
                if (volr > voll) {
                    volr *= -1;
                } else {
                    voll *= -1;
                }
            } else {
                if (spu_vol->left < 0) {
                    voll *= -1;
                } else {
                    volr *= -1;
                }
            }
        }
    }

    spu_vol->left = voll;
    spu_vol->right = volr;
}

void snd_CalcStereoBalance3d(SInt32 snd_vol, SInt32 snd_pan, SpuVolume *spu_vol) {
    SInt32 voll;
    SInt32 volr;
    SInt32 work_vol;

    work_vol = 0x102 * snd_vol;

    if (gStereoOrMono == 1) {
        spu_vol->left = work_vol;
        spu_vol->right = work_vol;
        return;
    }

    while (snd_pan >= 360) {
        snd_pan -= 360;
    }
    while (snd_pan < 0) {
        snd_pan += 360;
    }

    snd_pan = snd_pan < 270 ? snd_pan + 90 : snd_pan - 270;
    if (snd_pan < 180) {
        voll = gPanTable2[snd_pan].left * work_vol / 0x3FFF;
        volr = gPanTable2[snd_pan].right * work_vol / 0x3FFF;
        if (spu_vol->left < 0 && spu_vol->right < 0) {
            voll *= -1;
            volr *= -1;
        }

        if (spu_vol->left < 0 && spu_vol->right > -1 || spu_vol->left > -1 && spu_vol->right < 0) {
            if (abs(spu_vol->left) > abs(spu_vol->right)) {
                voll = (spu_vol->left / abs(spu_vol->left)) * voll;
                volr = (spu_vol->left / abs(spu_vol->left)) * volr;
            } else {
                voll = (spu_vol->right / abs(spu_vol->right)) * voll;
                volr = (spu_vol->right / abs(spu_vol->right)) * volr;
            }
        }
    } else {
        snd_pan -= 180;
        volr = gPanTable2[snd_pan].left * work_vol / 0x3FFF;
        voll = gPanTable2[snd_pan].right * work_vol / 0x3FFF;
        if (gStereoOrMono != 2) {
            if (spu_vol->left > -1 && spu_vol->right > -1) {
                if (volr < voll) {
                    volr *= -1;
                } else {
                    voll *= -1;
                }
            } else if (spu_vol->left < 0 && spu_vol->right < 0) {
                if (volr > voll) {
                    volr *= -1;
                } else {
                    voll *= -1;
                }
            } else {
                if (spu_vol->left < 0) {
                    voll *= -1;
                } else {
                    volr *= -1;
                }
            }
        }
    }

    spu_vol->left = voll;
    spu_vol->right = volr;
}

SInt16 snd_AdjustVolToGroup(SInt16 vol, SInt32 group) {
    SInt32 phase;
    SInt32 new_vol;
    SInt32 m_vol;

    if (group >= 15) {
        return vol;
    }

    if (vol >= 0x7FFF) {
        vol = 0x7FFE;
    }

    m_vol = (gMasterVol[group] * gGroupDuckMult[group] / 0x10000);
    new_vol = vol * m_vol / 0x400;

    phase = 1;
    if (new_vol < 0) {
        phase = -1;
    }

    new_vol = new_vol * new_vol / 32766;
    new_vol *= phase;

    return new_vol;
}

void snd_AdjustAllChannelsMasterVolume(UInt32 flags) {
    SInt32 count;
    SInt32 intr_state;
    SInt32 dis;
    UInt32 core;
    UInt32 c_v;

    snd_LockVoiceAllocatorEx(1, 0x8Au);
    dis = CpuSuspendIntr(&intr_state);
    for (count = 0; count < 48; ++count) {
        if ((gChannelStatus[count].Owner->flags & 2) == 0 &&
            (gChannelStatus[count].Status == 1 || gChannelStatus[count].Status == 4) &&
            ((1 << gChannelStatus[count].VolGroup) & flags) != 0) {
            core = count / 24;
            c_v = count % 24;
            sceSdSetParam(
                core | SD_VOICE(c_v) | SD_VP_VOLL,
                (UInt16)snd_AdjustVolToGroup(gChannelStatus[count].Volume.left, gChannelStatus[count].VolGroup) >> 1);
            sceSdSetParam(
                core | SD_VOICE(c_v) | SD_VP_VOLR,
                (UInt16)snd_AdjustVolToGroup(gChannelStatus[count].Volume.right, gChannelStatus[count].VolGroup) >> 1);
        }
    }

    if (!dis) {
        CpuResumeIntr(intr_state);
    }

    snd_UnlockVoiceAllocator();
}

SInt16 snd_MIDITo360Pan(SInt8 pan) {
    if (pan < 64) {
        return 90 * pan / 64 + 270;
    } else {
        return 90 * (pan - 64) / 63;
    }
}

void snd_CheckDuckers() {
    SInt32 i;
    SInt32 c;
    UInt32 playing_groups;
    UInt32 affected_groups;
    UInt32 all_affected_groups;
    SInt32 m_vol_track[15];

    playing_groups = 0;
    affected_groups = 0;
    all_affected_groups = 0;
    snd_LockVoiceAllocatorEx(1, 0x8Bu);
    for (i = 0; i < 48; ++i) {
        if (gChannelStatus[i].Status == 1 || gChannelStatus[i].Status == 4) {
            if (gChannelStatus[i].Owner) {
                if ((gChannelStatus[i].Owner->flags & 2) == 0)
                    playing_groups |= 1 << gChannelStatus[i].VolGroup;
            }
        }
    }

    snd_UnlockVoiceAllocator();
    for (c = 0; c < 15; ++c) {
        m_vol_track[c] = 0x10000;
    }

    for (i = 0; i < 4; ++i) {
        affected_groups = 0;
        if (gDuckers[i].source_group != -1) {
            if (((1 << gDuckers[i].source_group) & playing_groups) != 0) {
                if (gDuckers[i].current_duck_mult != gDuckers[i].full_duck_mult) {
                    gDuckers[i].current_duck_mult -= gDuckers[i].attack_time;
                    if (gDuckers[i].current_duck_mult < gDuckers[i].full_duck_mult) {
                        gDuckers[i].current_duck_mult = gDuckers[i].full_duck_mult;
                    }
                }

                affected_groups |= gDuckers[i].target_groups;
            } else if (gDuckers[i].current_duck_mult != 0x10000) {
                gDuckers[i].current_duck_mult += gDuckers[i].release_time;
                if (gDuckers[i].current_duck_mult > 0x10000 || gDuckers[i].current_duck_mult < 0) {
                    gDuckers[i].current_duck_mult = 0x10000;
                }

                affected_groups |= gDuckers[i].target_groups;
            }

            if (affected_groups) {
                for (c = 0; c < 15; ++c) {
                    if (((1 << c) & affected_groups) != 0 && gDuckers[i].current_duck_mult < m_vol_track[c])
                        m_vol_track[c] = gDuckers[i].current_duck_mult;
                }

                all_affected_groups |= affected_groups;
            }
        }
    }

    all_affected_groups = 0;
    for (c = 0; c < 15; ++c) {
        if (gGroupDuckMult[c] != m_vol_track[c]) {
            gGroupDuckMult[c] = m_vol_track[c];
            all_affected_groups |= 1 << c;
        }
    }

    if (all_affected_groups) {
        snd_AdjustAllChannelsMasterVolume(all_affected_groups);
    }
}

void snd_SetMasterVolumeDucker(SInt32 which, DuckerDefPtr state) {
    SInt32 delta;
    SInt32 frames;

    if (which < 0 || which >= 4) {
        return;
    }

    if (!state) {
        gDuckers[which].source_group = -1;
        return;
    }

    gDuckers[which] = *state;
    delta = 0x10000 - gDuckers[which].full_duck_mult;
    frames = (30 * gDuckers[which].attack_time) >> 16;
    if (!frames) {
        frames = 1;
    }

    gDuckers[which].attack_time = delta / frames;
    if (!gDuckers[which].attack_time) {
        gDuckers[which].attack_time = 1;
    }

    frames = (30 * gDuckers[which].release_time) >> 16;
    if (!frames) {
        frames = 1;
    }

    gDuckers[which].release_time = delta / frames;
    if (!gDuckers[which].release_time) {
        gDuckers[which].release_time = 1;
    }

    gDuckers[which].current_duck_mult = 0x10000;
}