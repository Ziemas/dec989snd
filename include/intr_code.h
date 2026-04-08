#ifndef INTR_CODE_H_
#define INTR_CODE_H_

#include "common.h"

extern SInt32 gCdBusy;
extern SInt32 gCdBusyTime;
extern BOOL gKillTimer;
extern SInt32 gTransfering;
extern SpuTransferCallbackProc gTransferDoneCallback;
extern SInt32 gSPURAMTransSema;
extern SInt32 gDoneLoadSema;
extern SInt32 gOddCdError;
extern SInt32 gDMAInUse[2];
extern SInt32 gLastDMA0Complete;
extern SInt32 gLastDMA1Complete;
extern SInt32 gDoneDMASema;
extern SInt32 gWaitingDMAComplete;

SInt32 snd_TimerLLTick(timercommon *common);
int snd_TransCallback(SInt32 ch, void *data);
void snd_DoneVAGReadCB(SInt32 code);
int snd_VAGDMADone0(int ch, void *data);

#endif // INTR_CODE_H_
