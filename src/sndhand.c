#include "common.h"

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_InitHandlers);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_InitSoundHandlers);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_FindFreeHandler);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_CheckInstanceLimit);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_GetFreeBlockSoundHandler);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_GetFreeMIDIHandler);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_GetFreeAMEHandler);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_ActivateHandler);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_DeactivateHandler);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_AttachSoundToHandlersChildList);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_CheckHandlerStillActive);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_StopHandler);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_PauseHandlerPtr);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_ContinueHandlerPtr);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_PauseAllSoundsInGroup);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_StopAllSoundsInGroup);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_ContinueAllSoundsInGroup);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_StopAllSounds);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_StopHandlerPtr);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_StopAllHandlersForSound);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_KillChildrenWithSound);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_GetNextHandlerVoice);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_UpdateHandlers);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_UpdateEffect);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_FreeEffectChain);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_RemoveEffect);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_FindEffect);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_GetFreeBasicEffect);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_AddEffectToHandler);

INCLUDE_ASM("asm/nonmatchings/sndhand", snd_RemoveEffectFromHandler);
