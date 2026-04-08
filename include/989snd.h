#ifndef SND989_H_
#define SND989_H_

#include "common.h"

extern Extern989MonitorInfo *g989Monitor;
extern SInt32 gThreadPriority_TICK;
extern SInt32 gThreadPriority_RPC;
extern SInt32 gThreadPriority_STREAM;
extern SInt32 gEEDMADoneSema;
extern SInt32 gFileReadMutex;

BOOL snd_RegisterExternProcHandler(Extern989HandlerPtr hand);
BOOL snd_UnregisterExternProcHandler(Extern989HandlerPtr hand);
Extern989HandlerPtr snd_FindExternProcHandler(UInt32 id);
SInt32 snd_DoExternCall(UInt32 proc_id, SInt32 func_index, SInt32 arg1, SInt32 arg2, SInt32 arg3, SInt32 arg4,
                        SInt32 arg5);
SInt32 snd_DoExternCallWithData(UInt32 proc_id, SInt32 func_index, SInt32 data_size, void *data_ptr);
SInt32 snd_StartEEMessaging();
SInt32 snd_StartEELoaderMessaging();
SInt32 start(SInt32 argc, SInt8 **argv);
void snd_ParseCommandLineArg(char *arg);
SInt32 snd_GetVal(char *st);
void snd_DumpVersionAndInfo();
void snd_RegisterErrorDisplayFunc(SndErrorDisplayFunc func);
void snd_ShowError(SInt32 num, UInt32 a1, UInt32 a2, UInt32 a3, UInt32 a4);
void snd_EEDMADone(SInt32 *sema_id_ptr);
void snd_Install989Monitor(Extern989MonitorInfo *mon);

#endif // SND989_H_
