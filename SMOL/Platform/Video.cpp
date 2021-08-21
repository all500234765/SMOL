#include "pc.h"
#include "Video.h"

#if SMOL_OS == SMOL_OS_WIN
    #include "Win32/WinVideo.inl"
#elif SMOL_OS == SMOL_OS_PSP
    #include "PSP/PSPVideo.inl"
#elif SMOL_OS == SMOL_OS_SWITCH
    #include "Switch/SwitchVideo.inl"
#endif
