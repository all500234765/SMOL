#pragma once

#include "pc.h"

String Bytes2String(size_t bytes);
String StringReplace(String &str, StringView substr, StringView replace_with);
StringW StringReplace(StringW &str, StringWView substr, StringWView replace_with);
bool StringIsWhitespace(char ch);
bool StringIsNewline(char ch);

f32 IEEEFloat(u32 f);
u32 IEEEUInt32(f32 f);

template<typename Dest, typename Src>
Dest IEEE(Src f) {
    static_assert(sizeof(Dest) >= f32(f), "Src & Dest doesn't match size.");
    Dest ret;
    memcpy(&ret, &f, sizeof(Dest));
    return ret;
}

StringW StringWiden(StringView  str);
String StringNarrow(StringWView  str);

bool StringMask(cstr mask, cstr string);

String StringFormat(cstr format, ...);
Vector<StringW> StringSplit(StringWView string, wchar_t delim=',');
Vector<String> StringSplit(StringView string, char delim=',');
