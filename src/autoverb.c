#include "common.h"
#include "functions.h"
#include "globals.h"

/* bss 110 */ BasicEffect gVerbFX[2][2];

void snd_AutoReverb(SInt32 core, SInt32 depth, SInt32 delta_time,
                    SInt32 channel_flags) {
    if (core & 1) {
        snd_AutoReverb_L(0, depth, delta_time, channel_flags);
    }

    if (core & 2) {
        snd_AutoReverb_L(1, depth, delta_time, channel_flags);
    }
}

void snd_AutoReverb_L(SInt32 core, SInt32 depth, SInt32 delta_time,
                      SInt32 channel_flags) {
    SInt32 depthchange;
    SInt16 original_depth;
    SInt32 effect_index;

    if (channel_flags == 3) {
        core = core == 0 ? 1 : 2;
        snd_AutoReverb(core, depth, delta_time, 1);
        snd_AutoReverb(core, depth, delta_time, 2);

        return;
    }

    if (channel_flags == 1) {
        effect_index = 0;
        original_depth = gReverbDepthL[core];
    } else {
        effect_index = 1;
        original_depth = gReverbDepthR[core];
    }

    gVerbFX[core][effect_index].delta_value = 0;
    if (!delta_time) {
        if (channel_flags == 1) {
            snd_SetReverbDepth_L(core, depth, gReverbDepthR[core]);
        } else {
            snd_SetReverbDepth_L(core, gReverbDepthL[core], depth);
        }

        return;
    }

    depthchange = depth - original_depth;
    if (!depthchange) {
        return;
    }

    gVerbFX[core][effect_index].destination = depth;
    if (abs(depthchange) < delta_time) {
        gVerbFX[core][effect_index].delta_time = abs(delta_time / depthchange);
        gVerbFX[core][effect_index].ec.delta_type = 1;
        gVerbFX[core][effect_index].ec.delta_counter =
            gVerbFX[core][effect_index].delta_time;
        gVerbFX[core][effect_index].delta_value =
            (depth < original_depth) ? -1 : 1;
    } else {
        gVerbFX[core][effect_index].ec.delta_type = 0;
        gVerbFX[core][effect_index].ec.delta_counter = 0;
        gVerbFX[core][effect_index].delta_value = depthchange / delta_time;
    }
}

void snd_KillAutoVerbForCore(SInt32 core) {
    gVerbFX[core][0].delta_value = 0;
    gVerbFX[core][1].delta_value = 0;
}

void snd_AutoVerbHandler() {
    SInt32 core;
    SInt32 newdepth;

    for (core = 0; core < 2; core++) {
        if (gVerbFX[core][0].delta_value) {
            newdepth = gReverbDepthL[core] + gVerbFX[core][0].delta_value;

            if ((gVerbFX[core][0].delta_value > 0 &&
                 gVerbFX[core][0].destination < newdepth) ||
                (gVerbFX[core][0].delta_value < 0 &&
                 newdepth < gVerbFX[core][0].destination)) {

                newdepth = gVerbFX[core][0].destination;
            }

            snd_SetReverbDepth_L(core, newdepth, gReverbDepthR[core]);

            if (newdepth == gVerbFX[core][0].destination) {
                gVerbFX[core][0].delta_value = 0;
            }
        }

        if (gVerbFX[core][1].delta_value) {
            newdepth = gReverbDepthR[core] + gVerbFX[core][1].delta_value;

            if ((gVerbFX[core][1].delta_value > 0 &&
                 gVerbFX[core][1].destination < newdepth) ||
                (gVerbFX[core][1].delta_value < 0 &&
                 newdepth < gVerbFX[core][1].destination)) {

                newdepth = gVerbFX[core][1].destination;
            }

            snd_SetReverbDepth_L(core, gReverbDepthL[core], newdepth);

            if (newdepth == gVerbFX[core][1].destination) {
                gVerbFX[core][1].delta_value = 0;
            }
        }
    }
}
