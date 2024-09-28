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

INCLUDE_ASM("asm/nonmatchings/playsnd", snd_PlaySoundVolPanPMPB);

INCLUDE_ASM("asm/nonmatchings/playsnd", snd_StopSound);

INCLUDE_ASM("asm/nonmatchings/playsnd", snd_SoundIsStillPlaying);

INCLUDE_ASM("asm/nonmatchings/playsnd", snd_IsSoundALooper);

INCLUDE_ASM("asm/nonmatchings/playsnd", snd_SetSoundVolPan);

INCLUDE_ASM("asm/nonmatchings/playsnd", snd_SetSoundParams);

INCLUDE_ASM("asm/nonmatchings/playsnd", snd_GetSoundOriginalPitch);

INCLUDE_ASM("asm/nonmatchings/playsnd", snd_GetSoundCurrentPitch);

INCLUDE_ASM("asm/nonmatchings/playsnd", snd_GetSoundPitchBend);

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
