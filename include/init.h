#ifndef INIT_H_
#define INIT_H_

#include "common.h"

SInt32 snd_SystemRunning();
SInt32 snd_StartSoundSystemEx(UInt32 flags);
SInt32 snd_InstallSoundTick();
void snd_RemoveSoundTick();
SInt32 snd_StopSoundSystem();
SInt32 snd_GetTickRate();
SInt32 snd_TimerLLIRQLoop();

#endif // INIT_H_
