#ifndef SNDHAND_H_
#define SNDHAND_H_

#include "common.h"

extern GSoundHandlerPtr gActiveSoundListHead;

void snd_InitHandlers(GSoundHandlerPtr handlers, SInt32 count, UInt32 type, SInt32 size);
void snd_InitSoundHandlers();
GSoundHandlerPtr snd_FindFreeHandler(GSoundHandlerPtr handlers, SInt32 count, UInt32 type, SInt32 size);
BOOL snd_CheckInstanceLimit(SFX2 *sfx, SInt32 vol, BOOL parent, BlockSoundHandlerPtr *weakest_holder);
BlockSoundHandlerPtr snd_GetFreeBlockSoundHandler(SFX2 *sfx, SInt32 vol, BOOL parent);
MIDIHandlerPtr snd_GetFreeMIDIHandler();
AMEHandlerPtr snd_GetFreeAMEHandler();
void snd_ActivateHandler(GSoundHandlerPtr snd);
void snd_DeactivateHandler(GSoundHandlerPtr snd, SInt32 and_child);
void snd_AttachSoundToHandlersChildList(GSoundHandlerPtr handler, UInt32 id);
GSoundHandlerPtr snd_CheckHandlerStillActive(UInt32 handle);
void snd_StopHandler(UInt32 handle, SInt32 and_child, SInt32 silence, BOOL vlimit_stop);
void snd_PauseHandlerPtr(GSoundHandlerPtr snd, SInt32 and_child);
void snd_ContinueHandlerPtr(GSoundHandlerPtr snd, SInt32 and_child);
void snd_PauseAllSoundsInGroup(UInt32 groups);
void snd_StopAllSoundsInGroup(UInt32 groups);
void snd_ContinueAllSoundsInGroup(UInt32 groups);
void snd_StopAllSounds();
void snd_StopHandlerPtr(GSoundHandlerPtr snd, SInt32 and_child, SInt32 silence, BOOL vlimit_stop);
void snd_StopAllHandlersForSound(SoundPtr snd, SInt32 silence, BOOL vlimit_stop);
BOOL snd_KillChildrenWithSound(GSoundHandlerPtr handler, void *sfx);
SInt32 snd_GetNextHandlerVoice(GSoundHandlerPtr snd, SInt32 start_v);
void snd_UpdateHandlers();
SInt32 snd_UpdateEffect(EffectChainPtr effect, GSoundHandlerPtr owner);
void snd_FreeEffectChain(EffectChainPtr effect);
void snd_RemoveEffect(GSoundHandlerPtr handler, SInt32 type_flag);
EffectChainPtr snd_FindEffect(GSoundHandlerPtr handler, SInt32 type_flag);
BasicEffectPtr snd_GetFreeBasicEffect();
void snd_AddEffectToHandler(GSoundHandlerPtr handler, EffectChainPtr effect);
void snd_RemoveEffectFromHandler(GSoundHandlerPtr handler, EffectChainPtr effect);

#endif // SNDHAND_H_
