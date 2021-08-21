#pragma once

#include "pc.h"

class Window {
private:
    static s32 width;
    static s32 height;
    static bool windowed;

public:
    static void Init();

    static void* GetHandle();

    static bool IsWindowed() { return windowed; }
    static s32 GetWidth() { return width; }
    static s32 GetHeight() { return height; }
    static void SetSize(u32 w, u32 h) { width = w; height = h; }
};
