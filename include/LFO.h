#ifndef LFO_H_
#define LFO_H_

#include "common.h"

UInt32 snd_DoOneLFO(LFOTracker *tracker);
SInt32 snd_LFO_TYPE_OFF(LFOTracker *tracker, int step);
SInt32 snd_LFO_TYPE_SINE(LFOTracker *tracker, int step);
SInt32 snd_LFO_TYPE_SQUARE(LFOTracker *tracker, int step);
SInt32 snd_LFO_TYPE_TRIANGLE(LFOTracker *tracker, int step);
SInt32 snd_LFO_TYPE_SAW(LFOTracker *tracker, int step);
SInt32 snd_LFO_TYPE_RAND(LFOTracker *tracker, int step);
void snd_InitLFO(LFOTracker *tracker);
void snd_RemoveLFOsForHandler(BlockSoundHandlerPtr handler);
void snd_RemoveLFO(LFOTracker *lfo);
BOOL snd_InsertLFO(LFOTracker *lfo);
void snd_HandleLFOs();
void snd_CalcLFODepth(LFOTracker *lfo);
SInt32 snd_GetLFO(LFOTracker *tracker, SInt32 step_mult);

#endif // LFO_H_
