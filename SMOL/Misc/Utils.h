#pragma once

#include "pc.h"

//#define MAKE_CONVERT_TO(TYPE, NAME) \
//template<typename T> \
//constexpr inline TYPE To ## NAME(T v) noexcept { return static_cast<TYPE>(v); }; \


/// Unsigned
template<typename T>
constexpr inline u8 ToU8(T v) noexcept { return static_cast<u8>(v); };

template<typename T>
constexpr inline u16 ToU16(T v) noexcept { return static_cast<u16>(v); };

template<typename T>
constexpr inline u32 ToU32(T v) noexcept { return static_cast<u32>(v); };

template<typename T>
constexpr inline u64 ToU64(T v) noexcept { return static_cast<u64>(v); };


/// Signed
template<typename T>
constexpr inline s8 ToS8(T v) noexcept { return static_cast<s8>(v); };

template<typename T>
constexpr inline s16 ToS16(T v) noexcept { return static_cast<s16>(v); };

template<typename T>
constexpr inline s32 ToS32(T v) noexcept { return static_cast<s32>(v); };

template<typename T>
constexpr inline s64 ToS64(T v) noexcept { return static_cast<s64>(v); };


/// Float
//template<typename T>
//constexpr inline f16 ToF16(T v) noexcept { return static_cast<f16>(v); };

template<typename T>
constexpr inline f32 ToF32(T v) noexcept { return static_cast<f32>(v); };

template<typename T>
constexpr inline f64 ToF64(T v) noexcept { return static_cast<f64>(v); };



//MAKE_CONVERT_TO(u8, U8)
//MAKE_CONVERT_TO(u16, U16)
//MAKE_CONVERT_TO(u32, U32)
//MAKE_CONVERT_TO(u64, U64)
//
//MAKE_CONVERT_TO(s8, S8)
//MAKE_CONVERT_TO(s16, S16)
//MAKE_CONVERT_TO(s32, S32)
//MAKE_CONVERT_TO(s64, S64)
//
////MAKE_CONVERT_TO(f16, F16)
//MAKE_CONVERT_TO(f32, F32)
//MAKE_CONVERT_TO(f64, F64)
//
//#undef MAKE_CONVERT_TO

template<typename T, typename From>
constexpr inline T To(From from) noexcept { return static_cast<T>(from); }

template<typename T, typename From>
constexpr inline T ToPtr(From from) noexcept { return reinterpret_cast<T>(from); }

template<typename From>
constexpr inline cstr ToCStr(From from) noexcept { return reinterpret_cast<cstr>(from); }

template<typename From>
constexpr inline cwstr ToCWStr(From from) noexcept { return reinterpret_cast<cwstr>(from); }

template<typename From>
constexpr inline char ToChar(From from) noexcept { return static_cast<char>(from); }

template<typename From>
constexpr inline wchar_t ToWChar(From from) noexcept { return static_cast<wchar_t>(from); }

template<typename T, u64 N>
constexpr inline u64 dim(T(&)[N]) { return N; }

