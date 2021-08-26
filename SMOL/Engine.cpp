#include "pc.h"
#include "Engine.h"

#include "Platform/Audio.h"
#include "Platform/Video.h"
#include "Platform/Input.h"
#include "Platform/Window.h"
#include "Platform/Pipeline.h"
#include "Platform/Streaming.h"
#include "Platform/Filesystem.h"
#include "Platform/Pipeline.h"
#include "Misc/Utils.h"
#include "Misc/Log.h"

#include <chrono>

static bool running = true;

#if SMOL_OS == SMOL_OS_WIN
    #include <Windows.h>
#endif

#if SMOL_OS == SMOL_OS_PSP
    #include <pspkernel.h>
    #include <pspctrl.h>
    #include <pspdisplay.h>
    PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER | PSP_THREAD_ATTR_VFPU);

    int exitCallback(int arg1, int arg2, void *common) {
        running = 0;
        return 0;
    }

    int callbackThread(SceSize args, void *argp) {
        int callbackID = sceKernelCreateCallback("Exit Callback", exitCallback, NULL);
        sceKernelRegisterExitCallback(callbackID);

        sceKernelSleepThreadCB();

        return 0;
    }

    int setupExitCallback() {
        int threadID = sceKernelCreateThread("Callback Update Thread", callbackThread, 0x11, 0xFA0, PSP_THREAD_ATTR_USER, 0);

        if( threadID >= 0 )
            sceKernelStartThread(threadID, 0, 0);

        return threadID;
    }
#endif

void EngineInit(int argc, char** argv) {
    Log::Init();

    Filesystem::Init();
    Streaming::Init();
    Window::Init();
    Pipeline::Init();

    Input::Init();
    Audio::Init();
    Video::Init();
}

void EngineRun(Game* game) {
    std::chrono::high_resolution_clock clock;
    auto time_point_current = clock.now();
    auto time_point_next = time_point_current;

#if SMOL_OS == SMOL_OS_PSP
    //pspDebugScreenInit();

    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
#endif

    game->GameStart();

    running = true;
    while( running )
    {
        Input::UpdateA();

#if SMOL_OS == SMOL_OS_WIN
        MSG msg{};

        while( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) ) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if( msg.message == WM_QUIT ) {
                running = false;
                break;
            }
        }
#else
        sceDisplayWaitVblankStart();
#endif
        Input::UpdateB();

        time_point_next    = clock.now();
        time_point_current = time_point_next;
        f32 delta_time     = ToF32(std::chrono::duration_cast<std::chrono::milliseconds>(time_point_next - time_point_current).count());

        Input::UpdateC();

        game->Step(delta_time);

        Pipeline::Present();
    }

    game->GameEnd();

#if SMOL_OS == SMOL_OS_PSP
    sceKernelExitGame();
#endif
}
