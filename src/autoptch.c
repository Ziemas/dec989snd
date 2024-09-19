#include "common.h"
#include "functions.h"

void snd_AutoPitch(UInt32 handle, SInt32 pitch, SInt32 delta_time,
                   SInt32 delta_from) {
    SInt32 wouldhavechanged;
    SInt32 pitchchange;
    SInt32 current_pitch;
    GSoundHandlerPtr handler;
    BasicEffectPtr effect;
    SInt32 new_effect;

    new_effect = 0;

    snd_LockMasterTick(186);
    if (!(handler = snd_CheckHandlerStillActive(handle))) {
        snd_UnlockMasterTick();
        return;
    }

    current_pitch = snd_GetSoundCurrentPitch(handle);
    if (current_pitch == 0) {
        snd_UnlockMasterTick();
        return;
    }

    pitchchange = pitch - current_pitch;
    if (!delta_time || !pitchchange) {
        snd_RemoveEffect(handler, 8);
        snd_SetSoundPitch(handle, pitch);
        snd_UnlockMasterTick();
        return;
    }

    effect = snd_FindEffect(handler, 8);
    if (effect) {
        effect->ec.Flags |= 0x80000000;
        if (delta_from == 3) {
            wouldhavechanged = pitch - effect->destination;
            if (!wouldhavechanged) {
                snd_RemoveEffect(handler, 8);
                snd_SetSoundPitch(handle, pitch);
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
            snd_SetSoundPitch(handle, pitch);
            snd_UnlockMasterTick();
            return;
        }

        effect->ec.Flags |= 0x8;
        effect->ec.proc = snd_AutoPitchHandler;
        new_effect = 1;
    }

    if (abs(pitchchange) < delta_time) {
        effect->delta_time = abs(delta_time / pitchchange);
        effect->ec.delta_type = 1;
        effect->ec.delta_counter = effect->delta_time;
        effect->delta_value = (pitch < current_pitch) ? -1 : 1;
    } else {
        effect->delta_value = pitchchange / delta_time;
        effect->ec.delta_type = 0;
        effect->ec.delta_counter = 0;
    }

    effect->destination = pitch;
    if (new_effect) {
        snd_AddEffectToHandler(handler, &effect->ec);
    } else {
        effect->ec.Flags &= ~0x80000000;
    }

    snd_UnlockMasterTick();
}

SInt32 snd_AutoPitchHandler(BasicEffectPtr effect, GSoundHandlerPtr handler) {
    SInt16 newMpitch;

    newMpitch =
        snd_GetSoundCurrentPitch(handler->OwnerID) + effect->delta_value;

    if ((effect->delta_value > 0 && newMpitch > effect->destination) ||
        (effect->delta_value < 0 && newMpitch < effect->destination)) {
        newMpitch = effect->destination;
    }

    snd_SetSoundPitch(handler->OwnerID, newMpitch);

    if (newMpitch == effect->destination) {
        snd_RemoveEffectFromHandler(handler, &effect->ec);
        return 0;
    }

    if (effect->ec.delta_type == 1) {
        effect->ec.delta_counter = effect->delta_time;
    }

    return 0;
}
