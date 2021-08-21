#pragma once

#include "pc.h"

#if SMOL_OS == SMOL_OS_WIN
    typedef void* CONSOLE_HANDLE;
#elif SMOL_OS == SMOL_OS_SWITCH
    typedef void* CONSOLE_HANDLE;
    typedef std::va_list va_list;
#elif SMOL_OS == SMOL_OS_PSP
    typedef void* CONSOLE_HANDLE;
#endif

class Log {
private:
    static CONSOLE_HANDLE hConsole;

    static void SetConsoleColor(u32 color);
    static void PrintBuffer(char buffer[]);

public:
    enum ConsoleColors {
        DarkBlue = 1, DarkGreen, DarkCyan, DarkRed, 
        DarkPurple, DarkYellow, DarkWhite, 

        Gray, Blue, Green, Cyan, 
        Red, Purple, Yellow, White, 

        BackgroundShift = 0xF
    };

    static void Init();
    static void Release();

    static void Message(cstr msg, ...);
    static void Warning(cstr msg, ...);
    static void Success(cstr msg, ...);
    static void Error  (cstr msg, ...);
    static void Crash  (cstr msg, ...);
};
