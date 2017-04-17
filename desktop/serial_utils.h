#pragma once

#include "types.h"

#include <SerialStream.h>

template <typename T>
T read (LibSerial::SerialStream& serial)
{
    T x;
    serial.read((char*) &x, sizeof(T));
    return x;
}

template <>
U32 read<U32> (LibSerial::SerialStream& serial)
{
    U32 x = 0;
    for (int i = 0; i < 4; ++i)
    {
        U8 b;
        serial.read((char*)&b, 1);
        x = x >> 8;
        x |= (b << 24);
    }
    return x;
}

template <typename T>
void write (LibSerial::SerialStream& serial, const T& val)
{
    serial.write((const char*) &val, sizeof(T));
}
