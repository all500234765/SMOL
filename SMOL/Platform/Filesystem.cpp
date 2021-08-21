#include "pc.h"
#include "Filesystem.h"

#if SMOL_OS == SMOL_OS_WIN
    #include "Win32/WinFilesystem.inl"
#elif SMOL_OS == SMOL_OS_PSP
    #include "PSP/PSPFilesystem.inl"
#elif SMOL_OS == SMOL_OS_SWITCH
    #include "Switch/SwitchFilesystem.inl"
#endif
