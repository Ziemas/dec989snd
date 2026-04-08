#ifndef STICK_H_
#define STICK_H_

#include "common.h"

void snd_LockMasterTick(UInt32 ownerID);
void snd_UnlockMasterTick();
SInt32 snd_SoundTickHandler();
UInt32 snd_GetTick();
void snd_SetTick(UInt32 val);

#endif // STICK_H_
