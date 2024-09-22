#include "common.h"
#include "functions.h"
#include "thread.h"

/* data 1308 */ BOOL gKillTimer = 0;
/* data 130c */ SInt32 gTransfering = 0;
/* data 1310 */ SpuTransferCallbackProc gTransferDoneCallback = NULL;
/* data 1314 */ SInt32 gSPURAMTransSema = -1;
/* data 1318 */ SInt32 gDoneLoadSema = 0;
/* data 131c */ SInt32 gOddCdError = 0;
/* data 1320 */ SInt32 gOddCdErrorCode = 0;
/* data 1324 */ SInt32 gCdBusy = 0;
/* data 1328 */ SInt32 gCdBusyTime = 0;
/* data 132c */ SInt32 gDMAInUse[2] = {0, 0};
/* data 1334 */ SInt32 gLastDMA0Complete = 0;
/* data 1338 */ SInt32 gLastDMA1Complete = 0;
/* data 133c */ SInt32 gDoneDMASema = 0;
/* data 1340 */ SInt32 gWaitingDMAComplete = 0;

SInt32 snd_TimerLLTick(timercommon *common) {
    iWakeupThread(*common->thid);
    if (gKillTimer) {
        gKillTimer = 0;
        return 0;
    } else {
        return common->compare;
    }
}

int snd_TransCallback(SInt32 ch, void *data) {
    if (!gTransfering) {
        return 1;
    }

    snd_FreeSPUDMA(gTransfering - 1);
    gTransfering = 0;

    if (gTransferDoneCallback) {
        gTransferDoneCallback();
    }

    if (gSPURAMTransSema > -1) {
        iSignalSema(gSPURAMTransSema);
    }

    return 0;
}

void snd_DoneVAGReadCB(SInt32 code) {
    gCdBusy = 0;
    if (code == 1) {
        iSignalSema(gDoneLoadSema);
        return;
    }

    gOddCdError = 1;
    gOddCdErrorCode = code;
    iSignalSema(gDoneLoadSema);
}

int snd_VAGDMADone0(int ch, void *data) {
    gDMAInUse[ch] = 0;
    if (!ch) {
        gLastDMA0Complete = 1;
    } else {
        gLastDMA1Complete = 1;
    }

    iSignalSema(gDoneDMASema);

    if (gWaitingDMAComplete > 0) {
        iWakeupThread(gWaitingDMAComplete);
        gWaitingDMAComplete = 0;
    }
    return 0;
}
