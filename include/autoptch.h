#ifndef AUTOPTCH_H_
#define AUTOPTCH_H_

#include "common.h"

void snd_AutoPitch(UInt32 handle, SInt32 pitch, SInt32 delta_time, SInt32 delta_from);
SInt32 snd_AutoPitchHandler(BasicEffectPtr effect, GSoundHandlerPtr handler);

#endif // AUTOPTCH_H_
