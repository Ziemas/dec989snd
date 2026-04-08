#ifndef AUTOPAN_H_
#define AUTOPAN_H_

#include "common.h"

void snd_AutoPan(UInt32 handle, SInt32 pan, SInt32 dir, SInt32 delta_time, SInt32 delta_from);
SInt32 snd_AutoPanHandler(BasicEffectPtr effect, GSoundHandlerPtr handler);

#endif // AUTOPAN_H_
