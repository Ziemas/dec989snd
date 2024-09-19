#include "common.h"
#include "functions.h"

void snd_AutoPan(UInt32 handle, SInt32 pan, SInt32 dir, SInt32 delta_time,
                 SInt32 delta_from) {
    SInt32 wouldhavechanged;
    SInt32 panchange;
    SInt32 shortest;
    GSoundHandlerPtr handler;
    BasicEffectPtr effect;
    SInt32 new_effect;

    shortest = 0;
    new_effect = 0;
    snd_LockMasterTick(154);

    if (!(handler = snd_CheckHandlerStillActive(handle))) {
        snd_UnlockMasterTick();
        return;
    }

    panchange = pan - handler->Current_Pan;
    if (!dir) {
        shortest = 1;
        dir = 1;
    }

    if (dir > 0 && panchange < 0) {
        panchange += 360;
    }
    if (dir < 0 && panchange > 0) {
        panchange -= 360;
    }

    if (shortest) {
        if (abs(panchange) >= 181) {
            dir = -dir;
            panchange -= 360;
        }
    }

    if (delta_from == 4) {
        delta_time = abs(panchange) * delta_time / 180;
    }

    if (!delta_time) {
        snd_RemoveEffect(handler, 4);
        snd_SetSoundVolPan(handle, 0x7fffffff, pan);
        snd_UnlockMasterTick();
        return;
    }

    if (!panchange) {
        snd_RemoveEffect(handler, 4);
        snd_UnlockMasterTick();
        return;
    }

    effect = snd_FindEffect(handler, 4);
    if (effect) {
        effect->ec.Flags |= 0x80000000;
        if (delta_from == 3) {
            wouldhavechanged = pan - effect->destination;
            if (dir > 0 && wouldhavechanged < 0) {
                wouldhavechanged += 360;
            }
            if (dir < 0 && wouldhavechanged > 0) {
                wouldhavechanged -= 360;
            }
            if (!wouldhavechanged) {
                snd_RemoveEffect(handler, 4);
                snd_SetSoundVolPan(handle, 0x7fffffff, pan);
                snd_UnlockMasterTick();
                return;
            }

            delta_time = abs(panchange * delta_time / wouldhavechanged);
            if (delta_time <= 0) {
                delta_time = 1;
            }
        }
    } else {
        effect = snd_GetFreeBasicEffect();
        if (!effect) {
            snd_SetSoundVolPan(handle, 0x7fffffff, pan);
            snd_UnlockMasterTick();
            return;
        }

        effect->ec.Flags |= 4;
        effect->ec.proc = snd_AutoPanHandler;
        new_effect = 1;
    }

    if (abs(panchange) < delta_time) {
        effect->delta_time = abs(delta_time / panchange);
        effect->ec.delta_type = 1;
        effect->ec.delta_counter = effect->delta_time;
        effect->delta_value = panchange < 0 ? -1 : 1;
    } else {
        effect->delta_value = (panchange << 5) / delta_time;
        effect->ec.delta_type = 0;
        effect->ec.delta_counter = 0;
    }

    effect->destination = pan;
    if (new_effect) {
        snd_AddEffectToHandler(handler, &effect->ec);
    } else {
        effect->ec.Flags &= ~0x80000000;
    }

    snd_UnlockMasterTick();
}

// INCLUDE_ASM("asm/nonmatchings/autopan", snd_AutoPanHandler);
SInt32 snd_AutoPanHandler(BasicEffectPtr effect, GSoundHandlerPtr handler) {
    SInt16 newMpan;
    SInt16 wrap_dest;

    if (abs(effect->delta_value) >= 2) {
        newMpan = ((handler->Current_Pan << 5) + effect->delta_value) >> 5;
    } else {
        newMpan = handler->Current_Pan + effect->delta_value;
    }

    if (newMpan >= 360 && effect->destination < handler->Current_Pan) {
        wrap_dest = effect->destination + 360;
    } else if (newMpan < 0 && effect->destination > handler->Current_Pan) {
        wrap_dest = effect->destination - 360;
    } else {
        wrap_dest = effect->destination;
    }

    if ((effect->delta_value > 0 && newMpan > wrap_dest &&
         handler->Current_Pan < wrap_dest) ||
        (effect->delta_value < 0 && newMpan < wrap_dest &&
         handler->Current_Pan > wrap_dest)) {
        newMpan = effect->destination;
    } else {
        if (newMpan < 0) {
            newMpan += 360;
        } else if (newMpan >= 360) {
            newMpan -= 360;
        }
    }

    snd_SetSoundVolPan(handler->OwnerID, 0x7fffffff, newMpan);
    if (newMpan == effect->destination) {
        snd_RemoveEffectFromHandler(handler, &effect->ec);

        return 0;
    }

    if (effect->ec.delta_type == 1) {
        effect->ec.delta_counter = effect->delta_time;
    }

    return 0;
}
