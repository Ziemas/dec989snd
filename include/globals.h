#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include "common.h"

// loader.c
extern char *gEEStatusAddr;

// init.c
extern BOOL gPrefs_Silent;

// loader.c
extern BOOL gLimit2Meg;
extern ExternSndIOPAlloc gAllocProc;
extern ExternSndIOPFree gFreeProc;

// playsnd.c
extern SInt32 gGlobalExcite;

// reverb.c
extern SInt16 gReverbDepthL[2];
extern SInt16 gReverbDepthR[2];

// stick.c
extern UInt32 gMasterTickOwner[32];

// valloc.c
extern UInt32 gVAllocOwnerID;
extern VoiceAttributes gChannelStatus[48];

#endif