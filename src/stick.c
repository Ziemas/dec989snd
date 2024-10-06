#include "common.h"
#include "functions.h"
#include "globals.h"
#include "libsd.h"
#include "stdio.h"

/* data 1c2c */ static UInt32 sTick = 0;
/* data 1c30 */ SInt32 gLockMasterTick = 0;
/* data 1c34 */ UInt32 gMasterTickOwner[32] = {};
/* data 1cb4 */ UInt32 gMasterTickOwnerLast = 0;
/* bss 8d00 */ SInt16 gEnvelopeStatus[48];
/* bss 8d60 */ SInt8 gAutoFreeState[48];

void snd_LockMasterTick(UInt32 ownerID) {
    if (gLockMasterTick >= 32) {
        printf("snd_LockMasterTick: nest too deep to track!\n");
    } else {
        gMasterTickOwner[gLockMasterTick] = ownerID;
    }

    gMasterTickOwnerLast = ownerID;
    gLockMasterTick++;
}

void snd_UnlockMasterTick() {
    if (!gLockMasterTick) {
        return;
    }

    gLockMasterTick--;
    gMasterTickOwnerLast = 0;

    if (gLockMasterTick < 0) {
        snd_ShowError(36, 0, 0, 0, 0);
        return;
    }

    if (gLockMasterTick >= 32) {
        printf("snd_LockMasterTick: nest too deep to track!\n");
    } else {
        gMasterTickOwner[gLockMasterTick] = 0;
    }
}

SInt32 snd_SoundTickHandler() {
    SInt16 env;
    VoiceAttributes *walk;
    static SInt32 in_handler = 0;
    SInt32 core;
    SInt32 c_v;

    if (in_handler) {
        return 0;
    }

    in_handler = 1;

    if (gVoiceAllocatorInUse || gLockMasterTick) {
        gTickAttentionNeeded++;
        if (gTickAttentionNeeded > 480) {
            printf("Sound System Tick locked out for 2 seconds.\n");
            printf("     Tick Owner list:\n");
            for (env = 0; env < 32; env++) {
                printf("         [%d] = 0x%8.8x\n", env, gMasterTickOwner[env]);
            }
            printf("     VAlloc Owner : 0x%8.8x\n", gVAllocOwnerID);
        }

        in_handler = 0;
        return 0;
    }

    gTickAttentionNeeded++;
    for (walk = gPlayingListHead; walk; walk = walk->playlist) {
        core = walk->voice / 24;
        c_v = walk->voice % 24;

        if ((gAwaitingKeyOn[core] & (1 << c_v)) == 0) {
            env = sceSdGetParam((core | SD_VOICE(c_v)) | SD_VP_ENVX);
            if (!env) {
                if ((gKeyedOffVoices[core] & (1 << c_v)) != 0) {
                    snd_VoiceIsDone(walk->voice);
                } else if ((gKeyedOnVoices[core] & (1 << c_v)) != 0) {
                    gAutoFreeState[walk->voice]--;
                    if (gAutoFreeState[walk->voice] == 0) {
                        snd_KeyOffVoice(walk->voice);
                    } else {
                        if (gAutoFreeState[walk->voice] < 0) {
                            gAutoFreeState[walk->voice] = 4;
                        }
                    }
                }
            } else {
                gAutoFreeState[walk->voice] = 4;
            }
        }
    }

    while (gTickAttentionNeeded > 0) {
        sTick++;
        snd_UpdateHandlers();
        snd_StopPendingVoices();
        snd_StartPendingVoices();
        snd_CheckVerb();
        snd_HandleLFOs();
        if ((sTick % 4) == 0) {
            snd_CheckDuckers();
        }
        gTickAttentionNeeded--;
    }
    gTickAttentionNeeded = 0;

    if (gCdBusy) {
        gCdBusyTime++;
        if (gCdBusyTime > 2400) {
            if (!gPrefs_Silent) {
                printf("989snd: Cd has been busy reading %s for 10 seconds.\n",
                       gCdBusy == 1 ? "a vag" : "data");
            }
            sceCdBreak();
            gCdBusyTime = 0;
        }
    }

    in_handler = 0;
    return 0;
}

UInt32 snd_GetTick() {
    return sTick;
}

void snd_SetTick(UInt32 val) {
    sTick = val;
}
