#pragma once

#include <stdint.h>

/*
File: Types
*/

#if defined(__GNUC__) || defined(__GNUG__) //Compiling with GNU C/C++ compiler
    typedef __INT8_TYPE__   I8;
    typedef __INT16_TYPE__  I16;
    typedef __INT32_TYPE__  I32;
    typedef __INT64_TYPE__  I64;
    typedef __UINT8_TYPE__  U8;
    typedef __UINT16_TYPE__ U16;
    typedef __UINT32_TYPE__ U32;
    typedef __UINT64_TYPE__ U64;
#else //Visual Studio
    typedef __int8  I8;
    typedef __int16 I16;
    typedef __int32 I32;
    typedef __int64 I64;
    typedef unsigned __int8  U8;
    typedef unsigned __int16 U16;
    typedef unsigned __int32 U32;
    typedef unsigned __int64 U64;
#endif
