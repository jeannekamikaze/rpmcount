#include "config.h"

#include <Arduino.h>
#include <EEPROM.h>

#define ROM_INITIALISED 17

#define DEFAULT_SIGNALS_PER_RPM 133
#define DEFAULT_START_DELAY_MILLIS 1000
#define DEFAULT_RPM_COUNT 5000

static Config config;

int writeLong (int address, unsigned long val)
{
    const char* p = (char*) &val;
    int i = 0;
    for (; i < sizeof(val); ++i, ++p)
    {
        EEPROM.write(address + i, *p);
    }
    return address + i;
}

int readLong (int address, unsigned long& val)
{
    char* p = (char*) &val;
    int i = 0;
    for (; i < sizeof(val); ++i, ++p)
    {
        *p = EEPROM.read(address + i);
    }
    return address + i;
}

const Config& readConfig ()
{
    byte initialised = EEPROM.read(0);
    if (initialised != ROM_INITIALISED)
    {
        int addr = 1;
        addr = writeLong(addr, DEFAULT_START_DELAY_MILLIS);
        addr = writeLong(addr, DEFAULT_RPM_COUNT);
        addr = writeLong(addr, DEFAULT_SIGNALS_PER_RPM);
        EEPROM.write(0, ROM_INITIALISED);
    }
    int addr = 1;
    addr = readLong(addr, config.startDelay);
    addr = readLong(addr, config.rpmCount);
    addr = readLong(addr, config.signalsPerRPM);
    return config;
}

void writeConfig (const Config& config)
{
    int addr = 1;
    addr = writeLong(addr, config.startDelay);
    addr = writeLong(addr, config.rpmCount);
    addr = writeLong(addr, config.signalsPerRPM);
    EEPROM.write(0, ROM_INITIALISED);
}
