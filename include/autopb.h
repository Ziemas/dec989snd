#ifndef AUTOPB_H_
#define AUTOPB_H_

#include "common.h"

void snd_AutoPitchBend(UInt32 handle, SInt32 pb, SInt32 delta_time, SInt32 delta_from);
SInt32 snd_AutoPitchBendHandler(BasicEffectPtr effect, GSoundHandlerPtr handler);

#endif // AUTOPB_H_
