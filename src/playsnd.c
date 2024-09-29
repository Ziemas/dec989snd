#include "common.h"
#include "functions.h"
#include "globals.h"
#include "stdio.h"

/* data 1b78 */ SInt32 gGlobalExcite = 0;
/* data 1b7c */ SoundBankPtr gSoloBank[8] = {0, 0, 0, 0, 0, 0, 0, 0};
/* data 1b9c */ SInt32 gSoloSound[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
/* data 1bbc */ SInt32 gNumSoloSounds = 0;

SInt32 snd_GetSoundUserData(SoundBankPtr bank, char *bank_name, int sound_index,
                            char *sound_name, int *destination) {
    SFXBlock2Ptr block;

    block = (SFXBlock2Ptr)bank;

    if (!block && bank_name) {
        block = snd_FindBankByName(bank_name);
        if (!block) {
            if (!gPrefs_Silent && gPrefs_ShowMissingSounds) {
                printf("989snd: Couldn't find named bank -> %s\n", bank_name);
            }
            return 0;
        }
    }

    if (sound_index > -1 && !block) {
        return 0;
    }

    if (sound_index == -1) {
        sound_index = snd_FindSoundByName(block, sound_name, &block);
        if (sound_index < 0) {
            if (!gPrefs_Silent && gPrefs_ShowMissingSounds) {
                printf("989snd: Didn't find sound named -> %s\n", sound_name);
            }
            return 0;
        }
    }

    if (block && block->DataID != 0x6B6C4253) {
        return 0;
    }

    destination[0] = block->SFXUD[sound_index].data[0];
    destination[1] = block->SFXUD[sound_index].data[1];
    destination[2] = block->SFXUD[sound_index].data[2];
    destination[3] = block->SFXUD[sound_index].data[3];

    return 1;
}

UInt32 snd_PlaySoundByNameVolPanPMPB(SoundBankPtr block, char *block_name,
                                     char *sound, SInt32 vol, SInt32 pan,
                                     SInt16 pitch_mod, SInt16 bend) {
    SInt32 index;

    if (!block && block_name) {
        block = snd_FindBankByName(block_name);
        if (!block) {
            if (!gPrefs_Silent && gPrefs_ShowMissingSounds) {
                printf("989snd: Couldn't find named bank -> %s\n", block_name);
            }
            return 0;
        }
    }

    if (block && block->DataID != 0x6B6C4253) {
        return 0;
    }

    index = snd_FindSoundByName(block, sound, &block);
    if (index < 0) {
        if (!gPrefs_Silent && gPrefs_ShowMissingSounds) {
            printf("989snd: Didn't find sound named -> %s\n", sound);
        }
        return 0;
    }

    return snd_PlaySoundVolPanPMPB(block, index, vol, pan, pitch_mod, bend);
}

UInt32 snd_PlaySoundEx(SndPlayParamsPtr params) {
    SoundBankPtr bank;
    SInt32 index;
    SInt32 vol;
    SInt32 pan;
    SInt16 pitch_mod;
    SInt16 bend;

    if ((params->mask & 1) != 0) {
        bank = params->bank_spec.ptr;
    } else {
        if (!params->bank_spec.name[0]) {
            bank = NULL;
        } else {

            bank = snd_FindBankByName(params->bank_spec.name);
            if (!bank) {
                if (!gPrefs_Silent && gPrefs_ShowMissingSounds) {
                    printf("989snd: Couldn't find named bank -> %s\n",
                           params->bank_spec.name);
                }
                return 0;
            }
        }
    }

    if ((params->mask & 2) != 0) {
        index = params->snd_spec.index;
    } else {
        index = snd_FindSoundByName(bank, params->snd_spec.name, &bank);
        if (index < 0) {
            if (!gPrefs_Silent && gPrefs_ShowMissingSounds) {
                printf("989snd: Didn't find sound named -> %s\n",
                       params->snd_spec.name);
            }
            return 0;
        }
    }

    if (bank->DataID == 0x6B6C4253) {
        return snd_PlaySFXWithStruct(bank, index, -1, -1, params);
    } else {
        if (!(params->mask & 4)) {
            vol = 0x400;
        } else {
            vol = params->vol;
        }

        if (!(params->mask & 8)) {
            pan = -1;
        } else {
            pan = params->pan;
        }

        if (!(params->mask & 0x10)) {
            pitch_mod = 0;
        } else {
            pitch_mod = params->pitch_mod;
        }

        if (!(params->mask & 0x20)) {
            bend = 0;
        } else {
            bend = params->pitch_bend;
        }

        return snd_PlaySoundVolPanPMPB(bank, index, vol, pan, pitch_mod, bend);
    }
}

UInt32 snd_PlaySoundVolPanPMPB(SoundBankPtr bank, SInt32 sound, SInt32 vol,
                               SInt32 pan, SInt16 pitch_mod, SInt16 bend) {
    SoundPtr sound_ptr;
    UInt32 ret;

    if (bank == (SoundBankPtr)-1) {
        bank = gBlockListHead;
    }

    if (!bank) {
        return 0;
    }

    if (bank->DataID == DATAID_SBLK) {
        return snd_PlaySFX((SFXBlock2Ptr)bank, sound, vol, pan, pitch_mod,
                           bend);
    } else {
        if (sound >= bank->NumSounds || sound < 0) {
            return 0;
        }

        if (!snd_DEBUG_CheckSolo(bank, sound)) {
            return 0;
        }

        sound_ptr = &bank->FirstSound[sound];
        if (!sound_ptr->Bank) {
            return 0;
        }

        switch (sound_ptr->Type) {
        case 1:
        case 2:
        case 3:
            return 0;
        case 4:
            return snd_PlayMIDISound(sound_ptr, vol, pan, pitch_mod, bend);
        case 5:
            ret = snd_PlayAMESound(sound_ptr, vol, pan, pitch_mod, bend);
            return ret;
        default:
            return 0;
        }
    }

    return 0;
}

void snd_StopSound(UInt32 handle) {
    GSoundHandlerPtr snd_ptr;

    snd_LockMasterTick(1280);
    snd_ptr = snd_CheckHandlerStillActive(handle);
    if (snd_ptr) {
        snd_StopHandlerPtr(snd_ptr, 1, 0, 0);
    }
    snd_UnlockMasterTick();
}

UInt32 snd_SoundIsStillPlaying(UInt32 handle) {
    if (!handle) {
        return 0;
    }

    if (handle == -1) {
        return -1;
    }

    if (snd_CheckHandlerStillActive(handle)) {
        return handle;
    } else {
        return 0;
    }
}

SInt32 snd_IsSoundALooper(SoundBankPtr bank, SInt32 sound) {
    SoundPtr sound_ptr;

    if (!bank) {
        return 0;
    }

    if (bank->DataID == DATAID_SBLK) {
        return snd_DoesSFXLoop((SFXBlock2 *)bank, sound);
    }

    if (sound >= bank->NumSounds || sound < 0) {
        return 0;
    }

    sound_ptr = &bank->FirstSound[sound];
    if (!sound_ptr->Bank) {
        return 0;
    }

    switch (sound_ptr->Type) {
    case 3:
        return 1;
    case 4:
        if (!((MIDISoundPtr)sound_ptr)->Repeats) {
            return 1;
        }
        return 0;
    case 5:
        return 1;
    default:
        return 0;
    }
}

void snd_SetSoundVolPan(UInt32 handle, SInt32 vol, SInt32 pan) {
    SInt32 type;

    type = (handle >> 24) & 0x1F;
    snd_LockMasterTick(1281);

    switch (type) {
    case 5:
        snd_SetSFXVolPan(handle, vol, pan, 0);
        break;
    case 0:
        break;
    case 1:
        snd_SetMIDISoundVolumePan(handle, vol, pan);
        break;
    case 2:
        snd_SetAMESoundVolumePan(handle, vol, pan);
        break;
    case 4:
        snd_SetVAGStreamVolPan(handle, vol, pan);
        break;
    }

    snd_UnlockMasterTick();
}

UInt32 snd_SetSoundParams(UInt32 handle, UInt32 mask, SInt32 vol, SInt32 pan,
                          SInt16 pitch_mod, SInt16 bend) {
    SInt32 p;

    if (!snd_CheckHandlerStillActive(handle)) {
        return 0;
    }

    if ((mask & 4) && (mask & 2)) {
        mask &= ~2;
    }

    p = pan;

    if (!(mask & 1)) {
        vol = 0x7FFFFFFF;
    }

    if (!(mask & 2)) {
        p = -2;
    }

    if ((mask & 1) || (mask & 2)) {
        snd_SetSoundVolPan(handle, vol, p);
    }

    if (mask & 4) {
        snd_AutoPan(handle, pan, 0, 30, 4);
    }

    if (mask & 8) {
        snd_SetSoundPitchModifier(handle, pitch_mod);
    }

    if (mask & 0x10) {
        snd_SetSoundPitchBend(handle, bend);
    }

    return handle;
}

SInt32 snd_GetSoundOriginalPitch(SoundBankPtr bank, SInt32 sound) {
    SoundPtr sound_ptr;

    if (!bank) {
        return 0;
    }

    if (bank->DataID == DATAID_SBLK) {
        return 0x1E00;
    }

    if (!bank || bank->DataID == DATAID_SBLK || sound >= bank->NumSounds ||
        sound < 0) {
        return 0;
    }

    sound_ptr = &bank->FirstSound[sound];
    if (!sound_ptr->Bank) {
        return 0;
    }

    switch (sound_ptr->Type) {
    case 1:
    case 2:
    case 3:
        return (sound_ptr->Note * 128) + sound_ptr->Fine;
    default:
        return 0;
    }
}

SInt32 snd_GetSoundCurrentPitch(UInt32 handle) {
    SInt32 type;
    SInt32 ret_val;
    VAGSoundHandlerPtr hand;

    type = (handle >> 24) & 0x1F;
    ret_val = 0;
    snd_LockMasterTick(1282);
    switch (type) {
    case 0:
    case 5:
        if ((hand = (VAGSoundHandlerPtr)snd_CheckHandlerStillActive(handle))) {
            ret_val = (hand->Current_Note * 128) + hand->Current_Fine;
        }
    }
    snd_UnlockMasterTick();

    return ret_val;
}

SInt32 snd_GetSoundPitchBend(UInt32 handle) {
    SInt32 type;
    SInt32 ret_val;
    VAGSoundHandlerPtr hand;

    type = (handle >> 24) & 0x1f;
    ret_val = 0;
    snd_LockMasterTick(1283);
    switch (type) {
    case 5:
        if ((hand = (VAGSoundHandlerPtr)snd_CheckHandlerStillActive(handle))) {
            ret_val = ((BlockSoundHandlerPtr)hand)->App_PB;
        }
        break;
    case 0:
        if ((hand = (VAGSoundHandlerPtr)snd_CheckHandlerStillActive(handle))) {
            ret_val = hand->Current_PB;
        }
        break;
    }

    snd_UnlockMasterTick();

    return ret_val;
}

INCLUDE_ASM("asm/nonmatchings/playsnd", snd_SetSoundPitch);

INCLUDE_ASM("asm/nonmatchings/playsnd", snd_SetSoundPitchBend);

INCLUDE_ASM("asm/nonmatchings/playsnd", snd_SetSoundPitchModifier);

INCLUDE_ASM("asm/nonmatchings/playsnd", snd_GetSoundReg);

INCLUDE_ASM("asm/nonmatchings/playsnd", snd_SetSoundReg);

INCLUDE_ASM("asm/nonmatchings/playsnd", snd_SetAllSoundReg);

INCLUDE_ASM("asm/nonmatchings/playsnd", snd_GetSoundPitchModifier);

INCLUDE_ASM("asm/nonmatchings/playsnd", snd_PauseSound);

INCLUDE_ASM("asm/nonmatchings/playsnd", snd_ContinueSound);

INCLUDE_ASM("asm/nonmatchings/playsnd", snd_StopAllSoundsInBank);

INCLUDE_ASM("asm/nonmatchings/playsnd", snd_SetGlobalExcite);

INCLUDE_ASM("asm/nonmatchings/playsnd", snd_GetGlobalExcite);

INCLUDE_ASM("asm/nonmatchings/playsnd", snd_FindBankByName);

INCLUDE_ASM("asm/nonmatchings/playsnd", snd_FindSoundByName);

INCLUDE_ASM("asm/nonmatchings/playsnd", snd_CalcSoundNameHash);

INCLUDE_ASM("asm/nonmatchings/playsnd", snd_CollectTones);

INCLUDE_ASM("asm/nonmatchings/playsnd", snd_DEBUG_SoloSound);

INCLUDE_ASM("asm/nonmatchings/playsnd", snd_DEBUG_CheckSolo);

INCLUDE_ASM("asm/nonmatchings/playsnd", snd_DEBUG_ClearSolo);
