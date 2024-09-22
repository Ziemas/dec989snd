#include "common.h"
#include "functions.h"
#include "globals.h"
#include "intrman.h"
#include "loadcore.h"

/* data 12dc */ SInt32 gResLibInited = 0;

extern libhead snd989_entry;

SInt32 InitResidentModule() {
    SInt32 ret;

    if (!gResLibInited && RegisterLibraryEntries(&snd989_entry)) {
        ret = 1;
    } else {
        ret = 0;
        gResLibInited = 1;
    }

    return ret;
}

UInt32 snd_PlaySoundByNameVolPanPMPB_E(SoundBankPtr block, char *block_name,
                                       char *sound, SInt32 vol, SInt32 pan,
                                       SInt16 pitch_mod, SInt16 bend) {
    UInt32 ret;

    ret = snd_PlaySoundByNameVolPanPMPB(block, block_name, sound, vol, pan,
                                        pitch_mod, bend);

    return ret;
}

SInt32 snd_GetSoundUserData_E(SoundBankPtr bank, char *bank_name,
                              int sound_index, char *sound_name,
                              int *destination) {
    SInt32 ret;

    ret = snd_GetSoundUserData(bank, bank_name, sound_index, sound_name,
                               destination);
    return ret;
}

void snd_SetMIDIRegister_E(UInt32 handle, SInt32 reg, SInt16 value) {
    snd_SetMIDIRegister(handle, reg, value);
}

SInt32 snd_GetMIDIRegister_E(UInt32 handle, SInt32 reg) {
    SInt32 ret;

    ret = snd_GetMIDIRegister(handle, reg);

    return ret;
}

void snd_SetGlobalExcite_E(SInt32 value) { snd_SetGlobalExcite(value); }

SInt32 snd_GetGlobalExcite_E() {
    SInt32 ret;

    ret = snd_GetGlobalExcite();

    return ret;
}

UInt32 snd_SRAMMalloc_E(UInt32 size) {
    UInt32 ret;
    SInt32 dis;
    SInt32 oldstat;

    dis = CpuSuspendIntr(&oldstat);
    ret = snd_SRAMMalloc(size);
    if (!dis) {
        CpuResumeIntr(oldstat);
    }
    return ret;
}

UInt32 snd_SRAMMarkUsed_E(UInt32 loc, UInt32 size) {
    UInt32 ret;
    SInt32 dis;
    SInt32 oldstat;

    dis = CpuSuspendIntr(&oldstat);
    ret = snd_SRAMMarkUsed(loc, size);
    if (!dis) {
        CpuResumeIntr(oldstat);
    }
    return ret;
}

void snd_SRAMFree_E(UInt32 loc, UInt32 size) {
    SInt32 dis;
    SInt32 oldstat;

    dis = CpuSuspendIntr(&oldstat);
    snd_SRAMFree(loc, size);
    if (!dis) {
        CpuResumeIntr(oldstat);
    }
}

SInt32 snd_GetDopplerPitchMod_E(SInt32 approaching_mph) {
    SInt32 ret;

    ret = 1524 * approaching_mph / 741;

    return ret;
}

void snd_MIDINoteOn_E(MIDIHandlerPtr stream) { snd_MIDINoteOn(stream); }

void snd_MIDINoteOff_E(MIDIHandlerPtr stream) { snd_MIDINoteOff(stream); }

void snd_PitchBend_E(MIDIHandlerPtr stream) { snd_PitchBend(stream); }

SInt16 snd_MIDITo360Pan_E(SInt8 pan) {
    SInt16 ret;

    ret = snd_MIDITo360Pan(pan);
    return ret;
}

void snd_ReleaseDamper_E(MIDIHandlerPtr stream) { snd_ReleaseDamper(stream); }

void snd_ResetControllers_E(MIDIHandlerPtr stream) {
    snd_ResetControllers(stream);
}

SInt32 snd_SRAMGetFreeMem_E() {
    SInt32 ret;
    SInt32 dis;
    SInt32 oldstat;

    dis = CpuSuspendIntr(&oldstat);
    ret = snd_SRAMGetFreeMem();
    if (!dis) {
        CpuResumeIntr(oldstat);
    }
    return ret;
}

SInt32 snd_SRAMMaxFreeBlockSize_E() {
    SInt32 ret;
    SInt32 dis;
    SInt32 oldstat;

    dis = CpuSuspendIntr(&oldstat);
    ret = snd_SRAMMaxFreeBlockSize();
    if (!dis) {
        CpuResumeIntr(oldstat);
    }
    return ret;
}
