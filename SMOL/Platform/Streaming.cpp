#include "pc.h"
#include "Streaming.h"

#if SMOL_OS == SMOL_OS_WIN
    #include "Win32/WinStreaming.inl"
#elif SMOL_OS == SMOL_OS_PSP
    #include "PSP/PSPStreaming.inl"
#elif SMOL_OS == SMOL_OS_SWITCH
    #include "Switch/SwitchStreaming.inl"
#endif
