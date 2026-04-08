#ifndef INTR_CODE_H_
#define INTR_CODE_H_

#include "common.h"

SInt32 snd_TimerLLTick(timercommon *common);
int snd_TransCallback(SInt32 ch, void *data);
void snd_DoneVAGReadCB(SInt32 code);
int snd_VAGDMADone0(int ch, void *data);

#endif // INTR_CODE_H_
