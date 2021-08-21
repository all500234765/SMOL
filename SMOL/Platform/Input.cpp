#include "pc.h"
#include "Input.h"

#if SMOL_OS == SMOL_OS_WIN
    #include "Win32/WinInput.inl"
#elif SMOL_OS == SMOL_OS_PSP
    #include "PSP/PSPInput.inl"
#elif SMOL_OS == SMOL_OS_SWITCH
    #include "Switch/SwitchInput.inl"
#endif
