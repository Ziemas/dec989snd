#include "common.h"
#include "functions.h"

void snd_AutoVol(UInt32 handle, SInt32 vol, SInt32 delta_time, SInt32 delta_from) {
	SInt32 wouldhavechanged;
	SInt32 volchange;
	SInt32 stopit;
	GSoundHandlerPtr handler;
	BasicEffectPtr effect;
	SInt32 new_effect;

    stopit = 0;
    new_effect = 0;
    snd_LockMasterTick(202);

	if (!(handler = snd_CheckHandlerStillActive(handle))) {
		snd_UnlockMasterTick();
		return;
	}

    if (vol == -4) {
        vol = 0;
        stopit = 1;
    } else {
        vol = (handler->Original_Vol * vol) >> 10;
        if (vol >= 128) {
            vol = 127;
        }
    }
    
    if (!delta_time) {
        snd_RemoveEffect(handler, 2);
        if (stopit == 1) {
            snd_StopSound(handle);
        } else {
            snd_SetSoundVolPan(handle, vol * -1, -2);
        }

        snd_UnlockMasterTick();
        return;
    }

    volchange = vol - handler->Current_Vol;
    if (!volchange) {
        snd_RemoveEffect(handler, 2);
        snd_UnlockMasterTick();
        return;
    }

    effect = snd_FindEffect(handler, 2);
    if (effect) {
		effect->ec.Flags |= 0x80000000;
        if (delta_from == 3) {
            wouldhavechanged = vol - effect->destination;
            if (!wouldhavechanged) {
                snd_RemoveEffect(handler, 2);
                snd_SetSoundVolPan(handle, vol * -1, -2);
                snd_UnlockMasterTick();
                return;
            }
            delta_time = abs(volchange * delta_time / wouldhavechanged);
            if (delta_time <= 0) {
                delta_time = 1;
            }
        }
    } else {
        effect = snd_GetFreeBasicEffect();
        if (!effect) {
            snd_SetSoundVolPan(handle, vol * -1, -2);
            snd_UnlockMasterTick();
            return;
        }
        effect->ec.Flags |= 2;
        effect->ec.proc = snd_AutoVolHandler;
        new_effect = 1;
    }

    if (abs(volchange) < delta_time) {
        effect->delta_time = abs(delta_time / volchange);
        effect->ec.delta_type = 1;
        effect->ec.delta_counter = effect->delta_time;
		effect->delta_value = (vol < handler->Current_Vol) ? -1 : 1;
    } else {
        effect->delta_value = (volchange << 5) / delta_time;
        effect->ec.delta_type = 0;
        effect->ec.delta_counter = 0;
    }

    effect->destination = stopit == 1 ? -4 : vol;
	if (new_effect) {
		snd_AddEffectToHandler(handler, &effect->ec);
	} else {
		effect->ec.Flags &= ~0x80000000;
	}

    snd_UnlockMasterTick();
}

SInt32 snd_AutoVolHandler(BasicEffectPtr effect, GSoundHandlerPtr handler) {
	SInt16 newMvol;
	SInt16 stopit;

    stopit = 0;
    if (effect->destination == -4) {
        effect->destination = 0;
        stopit = 1;
    }

    if (abs(effect->delta_value) >= 2) {
        newMvol = ((handler->Current_Vol << 5) + effect->delta_value) >> 5;
    } else {
        newMvol = handler->Current_Vol + effect->delta_value;
    }
    

    if ((effect->delta_value > 0 && newMvol > effect->destination) 
        || (effect->delta_value < 0 && newMvol < effect->destination)) {

        newMvol = effect->destination;
    }

    snd_SetSoundVolPan(handler->OwnerID, newMvol * -1, -2);
    if (newMvol == effect->destination) {
        snd_RemoveEffectFromHandler(handler, &effect->ec);
        return stopit;
    }

	if (effect->ec.delta_type == 1) {
		effect->ec.delta_counter = effect->delta_time;
	}

    if (stopit) {
        effect->destination = -4;
    }

    return 0;
}
