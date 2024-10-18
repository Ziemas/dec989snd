#include "common.h"
#include "functions.h"

void snd_AutoPitchBend(UInt32 handle, SInt32 pb, SInt32 delta_time, SInt32 delta_from) {
    SInt32 wouldhavechanged;
    SInt32 pitchchange;
    SInt32 current_pitch;
    GSoundHandlerPtr handler;
    BasicEffectPtr effect;
    SInt32 new_effect;

    new_effect = 0;
    snd_LockMasterTick(170);
    if (!(handler = snd_CheckHandlerStillActive(handle))) {
        snd_UnlockMasterTick();
        return;
    }

    current_pitch = snd_GetSoundPitchBend(handle);
    pitchchange = pb - current_pitch;

    if (!delta_time || !pitchchange) {
        snd_RemoveEffect(handler, 16);
        snd_SetSoundPitchBend(handle, pb);
        snd_UnlockMasterTick();
        return;
    }

    effect = snd_FindEffect(handler, 16);
    if (effect) {
        effect->ec.Flags |= 0x80000000;
        if (delta_from == 3) {
            wouldhavechanged = pb - effect->destination;
            if (!wouldhavechanged) {
                snd_RemoveEffect(handler, 16);
                snd_SetSoundPitchBend(handle, pb);
                snd_UnlockMasterTick();
                return;
            }

            delta_time = abs(pitchchange * delta_time / wouldhavechanged);
            if (delta_time <= 0) {
                delta_time = 1;
            }
        }
    } else {
        effect = snd_GetFreeBasicEffect();
        if (!effect) {
            snd_SetSoundPitchBend(handle, pb);
            snd_UnlockMasterTick();
            return;
        }

        effect->ec.Flags |= 0x10;
        effect->ec.proc = snd_AutoPitchBendHandler;
        new_effect = 1;
    }

    if (abs(pitchchange) < delta_time) {
        effect->delta_time = abs(delta_time / pitchchange);
        effect->ec.delta_type = 1;
        effect->ec.delta_counter = effect->delta_time;
        effect->delta_value = (pb < current_pitch) ? -1 : 1;
    } else {
        effect->delta_value = pitchchange / delta_time;
        effect->ec.delta_type = 0;
        effect->ec.delta_counter = 0;
    }

    effect->destination = pb;
    if (new_effect) {
        snd_AddEffectToHandler(handler, &effect->ec);
    } else {
        effect->ec.Flags &= ~0x80000000;
    }

    snd_UnlockMasterTick();
}

SInt32 snd_AutoPitchBendHandler(BasicEffectPtr effect, GSoundHandlerPtr handler) {
    SInt32 newMpitch;

    newMpitch = snd_GetSoundPitchBend(handler->OwnerID) + effect->delta_value;

    if ((effect->delta_value > 0 && effect->destination < newMpitch) ||
        (effect->delta_value < 0 && effect->destination > newMpitch)) {
        newMpitch = effect->destination;
    }

    snd_SetSoundPitchBend(handler->OwnerID, newMpitch);

    if (newMpitch == effect->destination) {
        snd_RemoveEffectFromHandler(handler, &effect->ec);
        return 0;
    }

    if (effect->ec.delta_type == 1) {
        effect->ec.delta_counter = effect->delta_time;
    }

    return 0;
}
