#ifndef AUTOVOL_H_
#define AUTOVOL_H_

#include "common.h"

void snd_AutoVol(UInt32 handle, SInt32 vol, SInt32 delta_time, SInt32 delta_from);
SInt32 snd_AutoVolHandler(BasicEffectPtr effect, GSoundHandlerPtr handler);

#endif // AUTOVOL_H_
