#include "pc.h"
#include "StringUtils.h"

#include "Misc/Log.h"

#include <sstream>
#include <cstring>
#include <cstdarg>
#include <string>

template <typename T>
std::string ToString(T val)
{
    std::stringstream stream;
    stream << val;
    return stream.str();
}

String Bytes2String(size_t bytes) {
    const StringView str[] = { " byte" , " bytes" , " KByte", " KBytes", 
                               " MByte", " MBytes", " GByte", " GBytes" };
    u32 index = 0;
    u64 res = bytes;

    for( u32 i = 1, j = 0; bytes > i && j < 4; j++, i *= 1024 ) {
        if( bytes == i ) {
            index = j * 2;
            break;
        } else if( bytes < 1024 * i ) {
            index = j * 2 + 1;
            break;
        }

        res /= 1024;
    }

    return ToString(res) + str[index].data();
}

String StringReplace(String &str, StringView substr, StringView replace_with) {
    u64 pos = str.find(substr.data());

    while( pos != String::npos ) {
        str.replace(pos, substr.size(), replace_with.data());
        
        pos = str.find(substr.data(), pos + replace_with.size());
    }

    return str;
}

StringW StringReplace(StringW &str, StringWView substr, StringWView replace_with) {
    u64 pos = str.find(substr.data());

    while( pos != StringW::npos ) {
        str.replace(pos, substr.size(), replace_with.data());
        
        pos = str.find(substr.data(), pos + replace_with.size());
    }

    return str;
}

bool StringIsWhitespace(char ch) {
    return (ch == ' ') || (ch == '\t') || StringIsNewline(ch);
}

bool StringIsNewline(char ch) {
    return (ch == '\n') || (ch == '\r');
}

StringW StringWiden(StringView str) {
    using namespace std;

    wostringstream wstm;
    const ctype<wchar_t>& ctfacet = use_facet< ctype<wchar_t> >(wstm.getloc());

    for( u64 i = 0; i < str.size(); i++ ) 
        wstm << static_cast<wchar_t>(ctfacet.widen(str[i]));
    return wstm.str();
}

String StringNarrow(StringWView str) {
    using namespace std;

    ostringstream stm;
    const ctype<char>& ctfacet = use_facet< ctype<char> >(stm.getloc());

    for( u64 i = 0; i < str.size(); i++ ) 
        stm << static_cast<char>(ctfacet.narrow(str[i], '\0'));
    return stm.str();
}

f32 IEEEFloat(u32 f) {
    static_assert(sizeof(f32) == sizeof(f), "`f32` has a weird size.");
    f32 ret;
    memcpy(&ret, &f, sizeof(f32));
    return ret;
}

u32 IEEEUInt32(f32 f) {
    static_assert(sizeof(u32) == sizeof(f), "`u32` has a weird size.");
    u32 ret;
    memcpy(&ret, &f, sizeof(u32));
    return ret;
}

bool StringMask(cstr mask, cstr string) {
    for( ; *mask != '\0'; ++mask ) {
        switch( *mask ) {
            case '?':
                if( *string == '\0' )
                    return false;
                
                string++;
                break;
                
            case '*':
            {
                if( mask[1] == '\0' )
                    return true;
                
                for( u64 i = 0, len = strlen(string); i < len; i++ ) {
                    if( StringMask(mask + 1, string + i) )
                        return true;
                }
                
                return false;
            }
            
            default:
                if( *string != *mask )
                    return false;
                
                string++;
                break;
        }
    }
    
    return *string == '\0';
}

String StringFormat(cstr format, ...) {
    char buff[4096];
    va_list _ArgList;

#if SMOL_OS == SMOL_OS_WIN
    __crt_va_start(_ArgList, format);
    vsprintf_s(buff, format, _ArgList);
    __crt_va_end(_ArgList);
#endif

#if SMOL_OS == SMOL_OS_SWITCH
    __builtin_va_start(_ArgList, format);
    vsprintf(buff, format, _ArgList);
    __builtin_va_end(_ArgList);
#endif

    return buff;
}

Vector<StringW> StringSplit(StringWView string, wchar_t delim) {
    Vector<StringW> arr{ };

    StringW str = string.data();
    for( u32 i = 0; i < string.length(); i++ ) {
        if( str[i] == delim ) {
            arr.push_back(str.substr(0, i));
            str = str.substr(i + 1);
        }
    }

    // No delimeters were met
    if( arr.size() == 0 )
        arr.push_back(str);

    return arr;
}

Vector<String> StringSplit(StringView string, char delim) {
    Vector<String> arr{ };

    String str = string.data();
    for( u32 i = 0; i < string.length(); i++ ) {
        if( str[i] == delim ) {
            arr.push_back(str.substr(0, i));
            str = str.substr(i + 1);
        }
    }

    // No delimeters were met
    if( arr.size() == 0 )
        arr.push_back(str);

    return arr;
}
