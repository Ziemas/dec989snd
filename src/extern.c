#include "common.h"

INCLUDE_ASM("asm/nonmatchings/extern", InitResidentModule);

INCLUDE_ASM("asm/nonmatchings/extern", snd_PlaySoundByNameVolPanPMPB_E);

INCLUDE_ASM("asm/nonmatchings/extern", snd_GetSoundUserData_E);

INCLUDE_ASM("asm/nonmatchings/extern", snd_SetMIDIRegister_E);

INCLUDE_ASM("asm/nonmatchings/extern", snd_GetMIDIRegister_E);

INCLUDE_ASM("asm/nonmatchings/extern", snd_SetGlobalExcite_E);

INCLUDE_ASM("asm/nonmatchings/extern", snd_GetGlobalExcite_E);

INCLUDE_ASM("asm/nonmatchings/extern", snd_SRAMMalloc_E);

INCLUDE_ASM("asm/nonmatchings/extern", snd_SRAMMarkUsed_E);

INCLUDE_ASM("asm/nonmatchings/extern", snd_SRAMFree_E);

INCLUDE_ASM("asm/nonmatchings/extern", snd_GetDopplerPitchMod_E);

INCLUDE_ASM("asm/nonmatchings/extern", snd_MIDINoteOn_E);

INCLUDE_ASM("asm/nonmatchings/extern", snd_MIDINoteOff_E);

INCLUDE_ASM("asm/nonmatchings/extern", snd_PitchBend_E);

INCLUDE_ASM("asm/nonmatchings/extern", snd_MIDITo360Pan_E);

INCLUDE_ASM("asm/nonmatchings/extern", snd_ReleaseDamper_E);

INCLUDE_ASM("asm/nonmatchings/extern", snd_ResetControllers_E);

INCLUDE_ASM("asm/nonmatchings/extern", snd_SRAMGetFreeMem_E);

INCLUDE_ASM("asm/nonmatchings/extern", snd_SRAMMaxFreeBlockSize_E);
