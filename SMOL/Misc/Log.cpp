#include "pc.h"
#include "Log.h"

#include "StringUtils.h"

#include "Platform/Window.h"

#if SMOL_OS == SMOL_OS_WIN
    #include <Windows.h>
    #include <stdio.h>
#else
    #include <cstdarg>

    #define printf_s printf
#endif


CONSOLE_HANDLE Log::hConsole = (CONSOLE_HANDLE)0;

namespace 
{

static FILE* file = nullptr;

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

void Log::Init() {
#if SMOL_OS == SMOL_OS_WIN
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    file = fopen("log.txt", "w");
#elif SMOL_OS == SMOL_OS_SWITCH
    //file = fopen("save:/log.txt", "w");
#endif

    if( file == nullptr ) 
        Log::Warning("Log::Init(): Failed to open log.txt\n");
}

void Log::Release() {
#if SMOL_OS == SMOL_OS_WIN
    if( file != nullptr ) 
        fclose(file);
#endif
}

#if SMOL_OS == SMOL_OS_SWITCH || SMOL_OS == SMOL_OS_PSP
    #define CALL_PRINTF_S \
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
