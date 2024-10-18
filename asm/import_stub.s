.include "macro.inc"

.set noat
.set noreorder

.section .text, "ax"

/* Generated by spimdisasm 1.30.2 */

glabel sysmem_stub
    /* 3A460 0003A3C0 0000E041 */ .word 0x41E00000 /* invalid instruction */
    /* 3A464 0003A3C4 00000000 */ .word 0x0
    /* 3A468 0003A3C8 01010000 */ .word 0x00000101 /* invalid instruction */
    /* 3A46C 0003A3CC 7379736D */ .word 0x6D737973
    /* 3A470 0003A3D0 656D0000 */ .word 0x00006D65 /* invalid instruction */
.size sysmem_stub, . - sysmem_stub

glabel AllocSysMemory
    /* 3A474 0003A3D4 0800E003 */  jr         $ra
    /* 3A478 0003A3D8 04000024 */   addiu     $zero, $zero, 0x4
.size AllocSysMemory, . - AllocSysMemory

glabel FreeSysMemory
    /* 3A47C 0003A3DC 0800E003 */  jr         $ra
    /* 3A480 0003A3E0 05000024 */   addiu     $zero, $zero, 0x5
.size FreeSysMemory, . - FreeSysMemory

glabel QueryMaxFreeMemSize
    /* 3A484 0003A3E4 0800E003 */  jr         $ra
    /* 3A488 0003A3E8 07000024 */   addiu     $zero, $zero, 0x7
.size QueryMaxFreeMemSize, . - QueryMaxFreeMemSize
    /* 3A48C 0003A3EC 00000000 */  nop
    /* 3A490 0003A3F0 00000000 */  nop

glabel intrman_stub
    /* 3A494 0003A3F4 0000E041 */ .word 0x41E00000 /* invalid instruction */
    /* 3A498 0003A3F8 00000000 */ .word 0x0
    /* 3A49C 0003A3FC 02010000 */ .word 0x00000102
    /* 3A4A0 0003A400 696E7472 */ .word 0x72746E69 /* invalid instruction */
    /* 3A4A4 0003A404 6D616E00 */ .word 0x006E616D /* invalid instruction */
.size intrman_stub, . - intrman_stub

glabel CpuSuspendIntr
    /* 3A4A8 0003A408 0800E003 */  jr         $ra
    /* 3A4AC 0003A40C 11000024 */   addiu     $zero, $zero, 0x11
.size CpuSuspendIntr, . - CpuSuspendIntr

glabel CpuResumeIntr
    /* 3A4B0 0003A410 0800E003 */  jr         $ra
    /* 3A4B4 0003A414 12000024 */   addiu     $zero, $zero, 0x12
.size CpuResumeIntr, . - CpuResumeIntr
    /* 3A4B8 0003A418 00000000 */  nop
    /* 3A4BC 0003A41C 00000000 */  nop

glabel ioman_stub
    /* 3A4C0 0003A420 0000E041 */ .word 0x41E00000 /* invalid instruction */
    /* 3A4C4 0003A424 00000000 */ .word 0x0
    /* 3A4C8 0003A428 04010000 */ .word 0x00000104 /* invalid instruction */
    /* 3A4CC 0003A42C 696F6D61 */ .word 0x616D6F69
    /* 3A4D0 0003A430 6E000000 */ .word 0x0000006E /* invalid instruction */
.size ioman_stub, . - ioman_stub

glabel open
    /* 3A4D4 0003A434 0800E003 */  jr         $ra
    /* 3A4D8 0003A438 04000024 */   addiu     $zero, $zero, 0x4
.size open, . - open

glabel close
    /* 3A4DC 0003A43C 0800E003 */  jr         $ra
    /* 3A4E0 0003A440 05000024 */   addiu     $zero, $zero, 0x5
.size close, . - close

glabel read
    /* 3A4E4 0003A444 0800E003 */  jr         $ra
    /* 3A4E8 0003A448 06000024 */   addiu     $zero, $zero, 0x6
.size read, . - read

glabel lseek
    /* 3A4EC 0003A44C 0800E003 */  jr         $ra
    /* 3A4F0 0003A450 08000024 */   addiu     $zero, $zero, 0x8
.size lseek, . - lseek
    /* 3A4F4 0003A454 00000000 */  nop
    /* 3A4F8 0003A458 00000000 */  nop

glabel loadcore_stub
    /* 3A4FC 0003A45C 0000E041 */ .word 0x41E00000 /* invalid instruction */
    /* 3A500 0003A460 00000000 */ .word 0x0
    /* 3A504 0003A464 03010000 */ .word 0x00000103
    /* 3A508 0003A468 6C6F6164 */ .word 0x64616F6C
    /* 3A50C 0003A46C 636F7265 */ .word 0x65726F63
.size loadcore_stub, . - loadcore_stub

glabel RegisterLibraryEntries
    /* 3A510 0003A470 0800E003 */  jr         $ra
    /* 3A514 0003A474 06000024 */   addiu     $zero, $zero, 0x6
.size RegisterLibraryEntries, . - RegisterLibraryEntries
    /* 3A518 0003A478 00000000 */  nop
    /* 3A51C 0003A47C 00000000 */  nop

glabel sifcmd_stub
    /* 3A520 0003A480 0000E041 */ .word 0x41E00000 /* invalid instruction */
    /* 3A524 0003A484 00000000 */ .word 0x0
    /* 3A528 0003A488 01010000 */ .word 0x00000101 /* invalid instruction */
    /* 3A52C 0003A48C 73696663 */ .word 0x63666973
    /* 3A530 0003A490 6D640000 */ .word 0x0000646D /* invalid instruction */
.size sifcmd_stub, . - sifcmd_stub

glabel sceSifInitRpc
    /* 3A534 0003A494 0800E003 */  jr         $ra
    /* 3A538 0003A498 0E000024 */   addiu     $zero, $zero, 0xE
.size sceSifInitRpc, . - sceSifInitRpc

glabel sceSifRegisterRpc
    /* 3A53C 0003A49C 0800E003 */  jr         $ra
    /* 3A540 0003A4A0 11000024 */   addiu     $zero, $zero, 0x11
.size sceSifRegisterRpc, . - sceSifRegisterRpc

glabel sceSifSetRpcQueue
    /* 3A544 0003A4A4 0800E003 */  jr         $ra
    /* 3A548 0003A4A8 13000024 */   addiu     $zero, $zero, 0x13
.size sceSifSetRpcQueue, . - sceSifSetRpcQueue

glabel sceSifRpcLoop
    /* 3A54C 0003A4AC 0800E003 */  jr         $ra
    /* 3A550 0003A4B0 16000024 */   addiu     $zero, $zero, 0x16
.size sceSifRpcLoop, . - sceSifRpcLoop

glabel sceSifGetOtherData
    /* 3A554 0003A4B4 0800E003 */  jr         $ra
    /* 3A558 0003A4B8 17000024 */   addiu     $zero, $zero, 0x17
.size sceSifGetOtherData, . - sceSifGetOtherData
    /* 3A55C 0003A4BC 00000000 */  nop
    /* 3A560 0003A4C0 00000000 */  nop

glabel sifman_stub
    /* 3A564 0003A4C4 0000E041 */ .word 0x41E00000 /* invalid instruction */
    /* 3A568 0003A4C8 00000000 */ .word 0x0
    /* 3A56C 0003A4CC 01010000 */ .word 0x00000101 /* invalid instruction */
    /* 3A570 0003A4D0 7369666D */ .word 0x6D666973
    /* 3A574 0003A4D4 616E0000 */ .word 0x00006E61 /* invalid instruction */
.size sifman_stub, . - sifman_stub

glabel sceSifInit
    /* 3A578 0003A4D8 0800E003 */  jr         $ra
    /* 3A57C 0003A4DC 05000024 */   addiu     $zero, $zero, 0x5
.size sceSifInit, . - sceSifInit

glabel sceSifDmaStat
    /* 3A580 0003A4E0 0800E003 */  jr         $ra
    /* 3A584 0003A4E4 08000024 */   addiu     $zero, $zero, 0x8
.size sceSifDmaStat, . - sceSifDmaStat

glabel sceSifCheckInit
    /* 3A588 0003A4E8 0800E003 */  jr         $ra
    /* 3A58C 0003A4EC 1D000024 */   addiu     $zero, $zero, 0x1D
.size sceSifCheckInit, . - sceSifCheckInit

glabel sceSifSetDmaIntr
    /* 3A590 0003A4F0 0800E003 */  jr         $ra
    /* 3A594 0003A4F4 20000024 */   addiu     $zero, $zero, 0x20
.size sceSifSetDmaIntr, . - sceSifSetDmaIntr
    /* 3A598 0003A4F8 00000000 */  nop
    /* 3A59C 0003A4FC 00000000 */  nop

glabel stdio_stub
    /* 3A5A0 0003A500 0000E041 */ .word 0x41E00000 /* invalid instruction */
    /* 3A5A4 0003A504 00000000 */ .word 0x0
    /* 3A5A8 0003A508 03010000 */ .word 0x00000103
    /* 3A5AC 0003A50C 73746469 */ .word 0x69647473
    /* 3A5B0 0003A510 6F000000 */ .word 0x0000006F /* invalid instruction */
.size stdio_stub, . - stdio_stub

glabel printf
    /* 3A5B4 0003A514 0800E003 */  jr         $ra
    /* 3A5B8 0003A518 04000024 */   addiu     $zero, $zero, 0x4
.size printf, . - printf
    /* 3A5BC 0003A51C 00000000 */  nop
    /* 3A5C0 0003A520 00000000 */  nop

glabel sysclib_stub
    /* 3A5C4 0003A524 0000E041 */ .word 0x41E00000 /* invalid instruction */
    /* 3A5C8 0003A528 00000000 */ .word 0x0
    /* 3A5CC 0003A52C 03010000 */ .word 0x00000103
    /* 3A5D0 0003A530 73797363 */ .word 0x63737973
    /* 3A5D4 0003A534 6C696200 */ .word 0x0062696C /* invalid instruction */
.size sysclib_stub, . - sysclib_stub

glabel memcpy
    /* 3A5D8 0003A538 0800E003 */  jr         $ra
    /* 3A5DC 0003A53C 0C000024 */   addiu     $zero, $zero, 0xC
.size memcpy, . - memcpy

glabel memmove
    /* 3A5E0 0003A540 0800E003 */  jr         $ra
    /* 3A5E4 0003A544 0D000024 */   addiu     $zero, $zero, 0xD
.size memmove, . - memmove

glabel memset
    /* 3A5E8 0003A548 0800E003 */  jr         $ra
    /* 3A5EC 0003A54C 0E000024 */   addiu     $zero, $zero, 0xE
.size memset, . - memset

glabel strncmp
    /* 3A5F0 0003A550 0800E003 */  jr         $ra
    /* 3A5F4 0003A554 1D000024 */   addiu     $zero, $zero, 0x1D
.size strncmp, . - strncmp

glabel strtol
    /* 3A5F8 0003A558 0800E003 */  jr         $ra
    /* 3A5FC 0003A55C 24000024 */   addiu     $zero, $zero, 0x24
.size strtol, . - strtol
    /* 3A600 0003A560 00000000 */  nop
    /* 3A604 0003A564 00000000 */  nop

glabel thbase_stub
    /* 3A608 0003A568 0000E041 */ .word 0x41E00000 /* invalid instruction */
    /* 3A60C 0003A56C 00000000 */ .word 0x0
    /* 3A610 0003A570 02010000 */ .word 0x00000102
    /* 3A614 0003A574 74686261 */ .word 0x61626874
    /* 3A618 0003A578 73650000 */ .word 0x00006573
.size thbase_stub, . - thbase_stub

glabel CreateThread
    /* 3A61C 0003A57C 0800E003 */  jr         $ra
    /* 3A620 0003A580 04000024 */   addiu     $zero, $zero, 0x4
.size CreateThread, . - CreateThread

glabel DeleteThread
    /* 3A624 0003A584 0800E003 */  jr         $ra
    /* 3A628 0003A588 05000024 */   addiu     $zero, $zero, 0x5
.size DeleteThread, . - DeleteThread

glabel StartThread
    /* 3A62C 0003A58C 0800E003 */  jr         $ra
    /* 3A630 0003A590 06000024 */   addiu     $zero, $zero, 0x6
.size StartThread, . - StartThread

glabel TerminateThread
    /* 3A634 0003A594 0800E003 */  jr         $ra
    /* 3A638 0003A598 0A000024 */   addiu     $zero, $zero, 0xA
.size TerminateThread, . - TerminateThread

glabel GetThreadId
    /* 3A63C 0003A59C 0800E003 */  jr         $ra
    /* 3A640 0003A5A0 14000024 */   addiu     $zero, $zero, 0x14
.size GetThreadId, . - GetThreadId

glabel ReferThreadStatus
    /* 3A644 0003A5A4 0800E003 */  jr         $ra
    /* 3A648 0003A5A8 16000024 */   addiu     $zero, $zero, 0x16
.size ReferThreadStatus, . - ReferThreadStatus

glabel SleepThread
    /* 3A64C 0003A5AC 0800E003 */  jr         $ra
    /* 3A650 0003A5B0 18000024 */   addiu     $zero, $zero, 0x18
.size SleepThread, . - SleepThread

glabel WakeupThread
    /* 3A654 0003A5B4 0800E003 */  jr         $ra
    /* 3A658 0003A5B8 19000024 */   addiu     $zero, $zero, 0x19
.size WakeupThread, . - WakeupThread

glabel iWakeupThread
    /* 3A65C 0003A5BC 0800E003 */  jr         $ra
    /* 3A660 0003A5C0 1A000024 */   addiu     $zero, $zero, 0x1A
.size iWakeupThread, . - iWakeupThread

glabel DelayThread
    /* 3A664 0003A5C4 0800E003 */  jr         $ra
    /* 3A668 0003A5C8 21000024 */   addiu     $zero, $zero, 0x21
.size DelayThread, . - DelayThread

glabel USec2SysClock
    /* 3A66C 0003A5CC 0800E003 */  jr         $ra
    /* 3A670 0003A5D0 27000024 */   addiu     $zero, $zero, 0x27
.size USec2SysClock, . - USec2SysClock
    /* 3A674 0003A5D4 00000000 */  nop
    /* 3A678 0003A5D8 00000000 */  nop

glabel thsemap_stub
    /* 3A67C 0003A5DC 0000E041 */ .word 0x41E00000 /* invalid instruction */
    /* 3A680 0003A5E0 00000000 */ .word 0x0
    /* 3A684 0003A5E4 01010000 */ .word 0x00000101 /* invalid instruction */
    /* 3A688 0003A5E8 74687365 */ .word 0x65736874
    /* 3A68C 0003A5EC 6D617000 */ .word 0x0070616D /* invalid instruction */
.size thsemap_stub, . - thsemap_stub

glabel CreateSema
    /* 3A690 0003A5F0 0800E003 */  jr         $ra
    /* 3A694 0003A5F4 04000024 */   addiu     $zero, $zero, 0x4
.size CreateSema, . - CreateSema

glabel DeleteSema
    /* 3A698 0003A5F8 0800E003 */  jr         $ra
    /* 3A69C 0003A5FC 05000024 */   addiu     $zero, $zero, 0x5
.size DeleteSema, . - DeleteSema

glabel SignalSema
    /* 3A6A0 0003A600 0800E003 */  jr         $ra
    /* 3A6A4 0003A604 06000024 */   addiu     $zero, $zero, 0x6
.size SignalSema, . - SignalSema

glabel iSignalSema
    /* 3A6A8 0003A608 0800E003 */  jr         $ra
    /* 3A6AC 0003A60C 07000024 */   addiu     $zero, $zero, 0x7
.size iSignalSema, . - iSignalSema

glabel WaitSema
    /* 3A6B0 0003A610 0800E003 */  jr         $ra
    /* 3A6B4 0003A614 08000024 */   addiu     $zero, $zero, 0x8
.size WaitSema, . - WaitSema

glabel PollSema
    /* 3A6B8 0003A618 0800E003 */  jr         $ra
    /* 3A6BC 0003A61C 09000024 */   addiu     $zero, $zero, 0x9
.size PollSema, . - PollSema
    /* 3A6C0 0003A620 00000000 */  nop
    /* 3A6C4 0003A624 00000000 */  nop

glabel timrman_stub
    /* 3A6C8 0003A628 0000E041 */ .word 0x41E00000 /* invalid instruction */
    /* 3A6CC 0003A62C 00000000 */ .word 0x0
    /* 3A6D0 0003A630 03010000 */ .word 0x00000103
    /* 3A6D4 0003A634 74696D72 */ .word 0x726D6974 /* invalid instruction */
    /* 3A6D8 0003A638 6D616E00 */ .word 0x006E616D /* invalid instruction */
.size timrman_stub, . - timrman_stub

glabel AllocHardTimer
    /* 3A6DC 0003A63C 0800E003 */  jr         $ra
    /* 3A6E0 0003A640 04000024 */   addiu     $zero, $zero, 0x4
.size AllocHardTimer, . - AllocHardTimer

glabel FreeHardTimer
    /* 3A6E4 0003A644 0800E003 */  jr         $ra
    /* 3A6E8 0003A648 06000024 */   addiu     $zero, $zero, 0x6
.size FreeHardTimer, . - FreeHardTimer

glabel SetTimerHandler
    /* 3A6EC 0003A64C 0800E003 */  jr         $ra
    /* 3A6F0 0003A650 14000024 */   addiu     $zero, $zero, 0x14
.size SetTimerHandler, . - SetTimerHandler

glabel SetupHardTimer
    /* 3A6F4 0003A654 0800E003 */  jr         $ra
    /* 3A6F8 0003A658 16000024 */   addiu     $zero, $zero, 0x16
.size SetupHardTimer, . - SetupHardTimer

glabel StartHardTimer
    /* 3A6FC 0003A65C 0800E003 */  jr         $ra
    /* 3A700 0003A660 17000024 */   addiu     $zero, $zero, 0x17
.size StartHardTimer, . - StartHardTimer

glabel StopHardTimer
    /* 3A704 0003A664 0800E003 */  jr         $ra
    /* 3A708 0003A668 18000024 */   addiu     $zero, $zero, 0x18
.size StopHardTimer, . - StopHardTimer
    /* 3A70C 0003A66C 00000000 */  nop
    /* 3A710 0003A670 00000000 */  nop

glabel libsd_stub
    /* 3A714 0003A674 0000E041 */ .word 0x41E00000 /* invalid instruction */
    /* 3A718 0003A678 00000000 */ .word 0x0
    /* 3A71C 0003A67C 05010000 */ .word 0x00000105 /* invalid instruction */
    /* 3A720 0003A680 6C696273 */ .word 0x7362696C /* invalid instruction */
    /* 3A724 0003A684 64000000 */ .word 0x00000064 /* invalid instruction */
.size libsd_stub, . - libsd_stub

glabel sceSdInit
    /* 3A728 0003A688 0800E003 */  jr         $ra
    /* 3A72C 0003A68C 04000024 */   addiu     $zero, $zero, 0x4
.size sceSdInit, . - sceSdInit

glabel sceSdSetParam
    /* 3A730 0003A690 0800E003 */  jr         $ra
    /* 3A734 0003A694 05000024 */   addiu     $zero, $zero, 0x5
.size sceSdSetParam, . - sceSdSetParam

glabel sceSdGetParam
    /* 3A738 0003A698 0800E003 */  jr         $ra
    /* 3A73C 0003A69C 06000024 */   addiu     $zero, $zero, 0x6
.size sceSdGetParam, . - sceSdGetParam

glabel sceSdSetSwitch
    /* 3A740 0003A6A0 0800E003 */  jr         $ra
    /* 3A744 0003A6A4 07000024 */   addiu     $zero, $zero, 0x7
.size sceSdSetSwitch, . - sceSdSetSwitch

glabel sceSdGetSwitch
    /* 3A748 0003A6A8 0800E003 */  jr         $ra
    /* 3A74C 0003A6AC 08000024 */   addiu     $zero, $zero, 0x8
.size sceSdGetSwitch, . - sceSdGetSwitch

glabel sceSdSetAddr
    /* 3A750 0003A6B0 0800E003 */  jr         $ra
    /* 3A754 0003A6B4 09000024 */   addiu     $zero, $zero, 0x9
.size sceSdSetAddr, . - sceSdSetAddr

glabel sceSdGetAddr
    /* 3A758 0003A6B8 0800E003 */  jr         $ra
    /* 3A75C 0003A6BC 0A000024 */   addiu     $zero, $zero, 0xA
.size sceSdGetAddr, . - sceSdGetAddr

glabel sceSdSetCoreAttr
    /* 3A760 0003A6C0 0800E003 */  jr         $ra
    /* 3A764 0003A6C4 0B000024 */   addiu     $zero, $zero, 0xB
.size sceSdSetCoreAttr, . - sceSdSetCoreAttr

glabel sceSdNote2Pitch
    /* 3A768 0003A6C8 0800E003 */  jr         $ra
    /* 3A76C 0003A6CC 0D000024 */   addiu     $zero, $zero, 0xD
.size sceSdNote2Pitch, . - sceSdNote2Pitch

glabel sceSdVoiceTrans
    /* 3A770 0003A6D0 0800E003 */  jr         $ra
    /* 3A774 0003A6D4 11000024 */   addiu     $zero, $zero, 0x11
.size sceSdVoiceTrans, . - sceSdVoiceTrans

glabel sceSdBlockTrans
    /* 3A778 0003A6D8 0800E003 */  jr         $ra
    /* 3A77C 0003A6DC 12000024 */   addiu     $zero, $zero, 0x12
.size sceSdBlockTrans, . - sceSdBlockTrans

glabel sceSdVoiceTransStatus
    /* 3A780 0003A6E0 0800E003 */  jr         $ra
    /* 3A784 0003A6E4 13000024 */   addiu     $zero, $zero, 0x13
.size sceSdVoiceTransStatus, . - sceSdVoiceTransStatus

glabel sceSdBlockTransStatus
    /* 3A788 0003A6E8 0800E003 */  jr         $ra
    /* 3A78C 0003A6EC 14000024 */   addiu     $zero, $zero, 0x14
.size sceSdBlockTransStatus, . - sceSdBlockTransStatus

glabel sceSdSetEffectAttr
    /* 3A790 0003A6F0 0800E003 */  jr         $ra
    /* 3A794 0003A6F4 17000024 */   addiu     $zero, $zero, 0x17
.size sceSdSetEffectAttr, . - sceSdSetEffectAttr

glabel sceSdClearEffectWorkArea
    /* 3A798 0003A6F8 0800E003 */  jr         $ra
    /* 3A79C 0003A6FC 19000024 */   addiu     $zero, $zero, 0x19
.size sceSdClearEffectWorkArea, . - sceSdClearEffectWorkArea

glabel sceSdSetTransIntrHandler
    /* 3A7A0 0003A700 0800E003 */  jr         $ra
    /* 3A7A4 0003A704 1A000024 */   addiu     $zero, $zero, 0x1A
.size sceSdSetTransIntrHandler, . - sceSdSetTransIntrHandler
    /* 3A7A8 0003A708 00000000 */  nop
    /* 3A7AC 0003A70C 00000000 */  nop

glabel cdvdman_stub
    /* 3A7B0 0003A710 0000E041 */ .word 0x41E00000 /* invalid instruction */
    /* 3A7B4 0003A714 00000000 */ .word 0x0
    /* 3A7B8 0003A718 01010000 */ .word 0x00000101 /* invalid instruction */
    /* 3A7BC 0003A71C 63647664 */ .word 0x64766463
    /* 3A7C0 0003A720 6D616E00 */ .word 0x006E616D /* invalid instruction */
.size cdvdman_stub, . - cdvdman_stub

glabel sceCdRead
    /* 3A7C4 0003A724 0800E003 */  jr         $ra
    /* 3A7C8 0003A728 06000024 */   addiu     $zero, $zero, 0x6
.size sceCdRead, . - sceCdRead

glabel sceCdGetError
    /* 3A7CC 0003A72C 0800E003 */  jr         $ra
    /* 3A7D0 0003A730 08000024 */   addiu     $zero, $zero, 0x8
.size sceCdGetError, . - sceCdGetError

glabel sceCdSearchFile
    /* 3A7D4 0003A734 0800E003 */  jr         $ra
    /* 3A7D8 0003A738 0A000024 */   addiu     $zero, $zero, 0xA
.size sceCdSearchFile, . - sceCdSearchFile

glabel sceCdSync
    /* 3A7DC 0003A73C 0800E003 */  jr         $ra
    /* 3A7E0 0003A740 0B000024 */   addiu     $zero, $zero, 0xB
.size sceCdSync, . - sceCdSync

glabel sceCdStop
    /* 3A7E4 0003A744 0800E003 */  jr         $ra
    /* 3A7E8 0003A748 0F000024 */   addiu     $zero, $zero, 0xF
.size sceCdStop, . - sceCdStop

glabel sceCdStatus
    /* 3A7EC 0003A74C 0800E003 */  jr         $ra
    /* 3A7F0 0003A750 1C000024 */   addiu     $zero, $zero, 0x1C
.size sceCdStatus, . - sceCdStatus

glabel sceCdCallback
    /* 3A7F4 0003A754 0800E003 */  jr         $ra
    /* 3A7F8 0003A758 25000024 */   addiu     $zero, $zero, 0x25
.size sceCdCallback, . - sceCdCallback

glabel sceCdBreak
    /* 3A7FC 0003A75C 0800E003 */  jr         $ra
    /* 3A800 0003A760 27000024 */   addiu     $zero, $zero, 0x27
.size sceCdBreak, . - sceCdBreak
    /* 3A804 0003A764 00000000 */  nop
    /* 3A808 0003A768 00000000 */  nop
