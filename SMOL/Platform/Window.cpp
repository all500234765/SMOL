#include "pc.h"
#include "Window.h"

#if SMOL_OS == SMOL_OS_WIN
    #include "Win32/WinWindow.inl"
#elif SMOL_OS == SMOL_OS_PSP
    #include "PSP/PSPWindow.inl"
#elif SMOL_OS == SMOL_OS_SWITCH
    #include "Switch/SwitchWindow.inl"
#endif
