#pragma once

#include "pc.h"

#include <cstring>

class StringView {
private:
    cstr str = nullptr;
    u64 len = 0;
    
public:
    StringView() {}
    StringView(const String& ref): str(ref.data()), len(ref.length()) {}
    StringView(cstr ref): str(ref), len(strlen(ref)) {}
    
    char operator[](u32 i) { return str[i]; }
    char operator[](u64 i) { return str[i]; }

    cstr data() const { return str; }
    u64 length() const { return len; }
    u64 size() const { return len * sizeof(char); }
};
