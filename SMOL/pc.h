#define _CRT_SECURE_NO_WARNINGS


// Select operating system based defines
#define SMOL_OS_WIN    0
#define SMOL_OS_PSP    1
#define SMOL_OS_SWITCH 2

#ifdef _WIN64
    #undef SMOL_OS
    #define SMOL_OS SMOL_OS_WIN
#endif

#ifdef __PSP__
    #undef SMOL_OS
    #define SMOL_OS SMOL_OS_PSP
#endif

#ifdef __SWITCH__
    #undef SMOL_OS
    #define SMOL_OS SMOL_OS_SWITCH
#endif


#pragma warning(disable: 5030)


#include <vector>
#include <string>

#ifndef Vector
    #define Vector std::vector
#endif

/////////////////////////////////////////
/// Types
/////////////////////////////////////////
#if (SMOL_OS != SMOL_OS_SWITCH) && (SMOL_OS != SMOL_OS_PSP)
    typedef unsigned char      u8;
    typedef unsigned short     u16;
    typedef unsigned int       u32;
    typedef unsigned long long u64;
    typedef long long          s64;
    typedef int                s32;
    typedef short              s16;
    typedef char               s8;
#elif SMOL_OS == SMOL_OS_PSP
    #include <psptypes.h>
#endif

//typedef half               f16;
typedef float              f32;
typedef double             f64;
typedef char*              cptr;
typedef wchar_t*           cwptr;
typedef const char*        cstr;
typedef const wchar_t*     cwstr;


typedef std::string  String;
typedef std::wstring StringW;
typedef std::wstring StringWView; // Because of PSP

#include "Misc/StringView.h"

#if (SMOL_OS == SMOL_OS_SWITCH) || (SMOL_OS != SMOL_OS_PSP) || defined(SMOL_NO_WINDOWS_INC) && defined(SMOL_NO_NETWORKING)
    typedef long LONG;
    typedef struct tagRECT
    {
        LONG left;
        LONG top;
        LONG right;
        LONG bottom;
    } RECT;

    void* __cdecl memcpy(void* _Dst, void const* _Src, size_t _Size);
#else
    #define __memcpy_s
#endif

#ifndef SMOL_ALIGN
    #if SMOL_OS == SMOL_OS_WIN
        #define SMOL_ALIGN(X) __declspec(alignas(X))
        #define SMOL_DATA_ALIGNMENT 32
        #define SMOL_DATA_SIZE SMOL_DATA_ALIGNMENT * 1024 * 1024 * 8 * 4
    #else
        #define SMOL_ALIGN(X) __attribute__((aligned(X)))
        #define SMOL_DATA_ALIGNMENT 16
        #define SMOL_DATA_SIZE SMOL_DATA_ALIGNMENT * 256 * 256 * 8 * 12
    #endif
#endif
