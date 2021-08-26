#include "pc.h"
#include "Log.h"

#include "StringUtils.h"

#include "Platform/Window.h"

#if SMOL_OS == SMOL_OS_WIN
    #include <Windows.h>
    #include <stdio.h>
#else
    #include <cstdarg>
    #include <sys/socket.h>
    #include <arpa/inet.h>

    #define printf_s printf
#endif


CONSOLE_HANDLE Log::hConsole = (CONSOLE_HANDLE)0;

namespace 
{

static FILE* file = nullptr;

#if (SMOL_OS == SMOL_OS_SWITCH) || (SMOL_OS == SMOL_OS_PSP)
    static sockaddr_in serv_addr = {};
    static int sock;
#endif

}; // namespace

void Log::PrintBuffer(char buffer[]) {
#if SMOL_OS == SMOL_OS_WIN
    OutputDebugStringA(buffer);
#endif

    printf_s(buffer);

#if SMOL_OS == SMOL_OS_WIN
    if( file != nullptr ) {
        fwrite(buffer, strlen(buffer), 1, file);
        fflush(file);
    }
#endif

    SetConsoleColor(White);
}

void Log::SetConsoleColor(u32 color) {
#if SMOL_OS == SMOL_OS_WIN
    SetConsoleTextAttribute(hConsole, color);
#elif SMOL_OS == SMOL_OS_SWITCH

#endif
}

#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspnet.h>
#include <pspnet_inet.h>
#include <pspnet_apctl.h>
#include <pspnet_resolver.h>
#include <psputility.h>


/*int netDialog()
{
    int done = 0;
    
    pspUtilityNetconfData data;
    
    memset(&data, 0, sizeof(data));
    data.base.size = sizeof(data);
    data.base.language = PSP_SYSTEMPARAM_LANGUAGE_ENGLISH;
    data.base.buttonSwap = PSP_UTILITY_ACCEPT_CROSS;
    data.base.graphicsThread = 17;
    data.base.accessThread = 19;
    data.base.fontThread = 18;
    data.base.soundThread = 16;
    data.action = PSP_NETCONF_ACTION_CONNECTAP;
    
    struct pspUtilityNetconfAdhoc adhocparam;
    memset(&adhocparam, 0, sizeof(adhocparam));
    data.adhocparam = &adhocparam;
    
    sceUtilityNetconfInitStart(&data);
    
    while(running)
    {
        drawStuff();
        
        switch(sceUtilityNetconfGetStatus())
        {
            case PSP_UTILITY_DIALOG_NONE:
                break;
            
            case PSP_UTILITY_DIALOG_VISIBLE:
                sceUtilityNetconfUpdate(1);
                break;
            
            case PSP_UTILITY_DIALOG_QUIT:
                sceUtilityNetconfShutdownStart();
                break;
            
            case PSP_UTILITY_DIALOG_FINISHED:
                done = 1;
                break;
            
            default:
                break;
        }
        
        sceDisplayWaitVblankStart();
        sceGuSwapBuffers();
        
        if(done)
            break;
    }
    
    return 1;
}*/

void netInit(void)
{
    sceNetInit(128*1024, 42, 4*1024, 42, 4*1024);
    sceNetInetInit();
    sceNetApctlInit(0x8000, 48);
}

void netTerm(void)
{
    sceNetApctlTerm();
    sceNetInetTerm();
    sceNetTerm();
}



int net_thread(SceSize args, void* argp)
{
    int err = 0;

    do
    {
        if( err = pspSdkInetInit() )
        {
            pspDebugScreenPrintf("Error, could not initialize network %08X\n", err);
            break;
        }


        {
            int stateLast = -1;
            if( (err = sceNetApctlConnect(1)) != 0 )
            {
                pspDebugScreenPrintf("Error, sceNetApctlConnect %08X\n", err);
                break;
            }
            //0x8002013A

            pspDebugScreenPrintf("Connecting...\n");

            while (1)
            {
                int state;
                err = sceNetApctlGetState(&state);
                if (err != 0)
                {
                    pspDebugScreenPrintf("sceNetApctlGetState returns $%x\n", err);
                    break;
                }
                if( state > stateLast )
                {
                    pspDebugScreenPrintf("  connection state %d of 4\n", state);
                    stateLast = state;
                }
                if (state == 4)
                    break;  // connected with static IP
                
                // wait a little before polling again
                sceKernelDelayThread(50*1000); // 50ms
            }

            pspDebugScreenPrintf("Connected!\n");
        }



        if( (sock = socket(PF_INET, SOCK_STREAM, 0)) < 0 )
        {
            pspDebugScreenPrintf("Error, could not bind the socket %08X\n", err);
            break;
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(27072);
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        
        if( (err = bind(sock, (sockaddr*)&serv_addr, sizeof(serv_addr))) < 0 )
        {
            pspDebugScreenPrintf("Error, could not bind the socket %08X\n", err);
            break;
        }

        inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
        connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr));
        
        char *hello = "Hello world";
        send(sock, hello, strlen(hello), 0);
        
        pspDebugScreenPrintf(hello);
        pspDebugScreenPrintf("\nSock: %d\n", sock);

    } while(0);

    return 0;
}

#include <errno.h>

void Log::Init() {
#if SMOL_OS == SMOL_OS_WIN
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    file = fopen("log.txt", "w");

    if( file == nullptr ) 
        Log::Warning("Log::Init(): Failed to open log.txt\n");
#elif (SMOL_OS == SMOL_OS_SWITCH) || (SMOL_OS == SMOL_OS_PSP)
    //file = fopen("save:/log.txt", "w");

    
    pspDebugScreenInit();
    pspDebugScreenClear();       // Clears screen pixels
    pspDebugScreenSetXY(0, 0);   // Reset where we draw

    {
        /*sceUtilityLoadNetModule(PSP_NET_MODULE_COMMON);
        sceUtilityLoadNetModule(PSP_NET_MODULE_INET);
        netInit();

        pspUtilityNetconfData data;
        
        memset(&data, 0, sizeof(data));
        data.base.size = sizeof(data);
        data.base.language = PSP_SYSTEMPARAM_LANGUAGE_ENGLISH;
        data.base.buttonSwap = PSP_UTILITY_ACCEPT_CROSS;
        data.base.graphicsThread = 17;
        data.base.accessThread = 19;
        data.base.fontThread = 18;
        data.base.soundThread = 16;
        data.action = PSP_NETCONF_ACTION_CONNECTAP;

        int err = 0;
        if( (sock = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
        {
            pspDebugScreenPrintf("Error, could not create the socket %08X, %08X\n", sock, errno);
            return;
        }
        
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(27072);
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        
        //if( (err = bind(sock, (sockaddr*)&serv_addr, sizeof(serv_addr))) < 0 )
        //{
        //    pspDebugScreenPrintf("Error, could not bind the socket %08X\n", err);
        //    return;
        //}
        
        inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
        connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr));
        
        char *hello = "Hello world";
        send(sock, hello, strlen(hello), 0);
        
        pspDebugScreenPrintf(hello);
        pspDebugScreenPrintf("\nSock: %d\n", sock);*/
    }
#endif
}

void Log::Release() {
#if SMOL_OS == SMOL_OS_WIN
    if( file != nullptr ) 
        fclose(file);
#endif
}

#if SMOL_OS == SMOL_OS_SWITCH || SMOL_OS == SMOL_OS_PSP
    #define CALL_PRINTF_S 
        /*va_list _ArgList; \
        __builtin_va_start(_ArgList, msg); \
        fiprintf(file, msg, _ArgList); \
        __builtin_va_end(_ArgList); \
        SetConsoleColor(White);*/
#else
    #define CALL_PRINTF_S \
        char buff[4096]; \
        va_list _ArgList; \
        __crt_va_start(_ArgList, msg); \
        vsprintf_s(buff, msg, _ArgList); \
        __crt_va_end(_ArgList); \
        PrintBuffer(buff);
#endif

void Log::Message(cstr msg, ...) {
    SetConsoleColor(White);
    
    //va_list _ArgList;
    /*__builtin_va_start(_ArgList, msg);
    //viprintf(msg, _ArgList);

    vfprintf(file, msg, _ArgList);

    __builtin_va_end(_ArgList);*/
    CALL_PRINTF_S;
}

void Log::Warning(cstr msg, ...) {
    SetConsoleColor(Yellow);

    CALL_PRINTF_S;
}

void Log::Error(cstr msg, ...) {
    SetConsoleColor(Red);

    CALL_PRINTF_S;
}

void Log::Success(cstr msg, ...) {
    SetConsoleColor(Green);

    CALL_PRINTF_S;
}

void Log::Crash(cstr msg, ...) {
#if SMOL_OS == SMOL_OS_WIN
    char buff[4096];
    va_list _ArgList;
    __crt_va_start(_ArgList, msg);
    vsprintf_s(buff, msg, _ArgList);
    __crt_va_end(_ArgList);

    PrintBuffer(buff);

    StringW wbuff = StringWiden(buff);
    MessageBox((HWND)Window::GetHandle(), wbuff.c_str(), L"App crashed!", MB_OK | MB_ICONERROR);
#elif SMOL_OS == SMOL_OS_SWITCH
    CALL_PRINTF_S;
    //fclose(file);
#endif

    exit(1);
}
