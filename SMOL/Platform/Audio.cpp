#include "pc.h"
#include "Audio.h"

#if SMOL_OS == SMOL_OS_WIN
    #include "Win32/WinAudio.inl"
#elif SMOL_OS == SMOL_OS_PSP
    #include "PSP/PSPAudio.inl"
#elif SMOL_OS == SMOL_OS_SWITCH
    #include "Switch/SwitchAudio.inl"
#endif
