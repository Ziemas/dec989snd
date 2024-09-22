#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include "common.h"

// 989snd.c
extern SInt32 gThreadPriority_TICK;

// loader.c
extern char *gEEStatusAddr;

// init.c
extern BOOL gPrefs_Silent;

// intr_code.c
extern BOOL gKillTimer;

// loader.c
extern BOOL gLimit2Meg;
extern ExternSndIOPAlloc gAllocProc;
extern ExternSndIOPFree gFreeProc;

// playsnd.c
extern SInt32 gGlobalExcite;

// reverb.c
extern SInt16 gReverbDepthL[2];
extern SInt16 gReverbDepthR[2];

// sndhand.c
extern GSoundHandlerPtr gActiveSoundListHead;

// stick.c
extern SInt32 gLockMasterTick;
extern UInt32 gMasterTickOwner[32];

// valloc.c
extern UInt32 gVAllocOwnerID;
extern VoiceAttributes gChannelStatus[48];
extern UInt32 gAwaitingKeyOff[2];

#endif