#ifndef MOVIESND_H_
#define MOVIESND_H_

#include "common.h"

void *snd_InitMovieSoundEx(SInt32 buff_size, SInt32 spu_buff_size, SInt32 vol, SInt32 pan, SInt32 vol_group,
                           SInt32 type);
void snd_UpdateMovieADPCM(SInt32 data_size, UInt32 offset);
UInt32 snd_GetMovieNAX();
void snd_ClearMovieSoundBuffer();
void snd_CloseMovieSound();
void snd_ResetMovieSound();
void snd_StartMovieSoundEx(void *buff, SInt32 buff_size, SInt32 offset, UInt32 sr, SInt32 ch);
void snd_AdjustMovieVolDueToMasterChange(SInt32 which);
void snd_SetMovieSoundVolPan(SInt32 vol, SInt32 pan);
UInt32 snd_GetMovieBufferPos();
SInt32 snd_PauseMovieSound();

#endif // MOVIESND_H_
