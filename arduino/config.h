#pragma once

struct Config
{
    unsigned long startDelay;    // Start delay in milliseconds
    unsigned long rpmCount;      // Number of RPM ticks to count
    unsigned long signalsPerRPM; // Number of signal ticks per RPM tick
};

const Config& readConfig ();

void writeConfig (const Config&);
