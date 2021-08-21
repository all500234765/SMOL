#pragma once

#include "pc.h"

#include "Game.h"

#if SMOL_OS == SMOL_OS_PSP
    #include <pspmoduleinfo.h>
#else
    enum PspModuleInfoAttr
    {
        PSP_MODULE_USER         = 0,
        PSP_MODULE_NO_STOP      = 0x0001,
        PSP_MODULE_SINGLE_LOAD  = 0x0002,
        PSP_MODULE_SINGLE_START = 0x0004,
        PSP_MODULE_KERNEL       = 0x1000,
    };

    #define PSP_MODULE_INFO(name, attributes, major_version, minor_version)
#endif

void EngineInit(int argc, char** argv);
void EngineRun(Game* game); /// Remember to keep same signature in Pipeline class
