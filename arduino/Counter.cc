#include "Counter.h"

#include <Arduino.h>

void Counter::init (unsigned long startDelay, unsigned long rpmCount, unsigned long signalsPerRPM)
{
    this->startDelay    = startDelay;
    this->rpmCount      = rpmCount;
    this->signalsPerRPM = signalsPerRPM;
    reset();
}

void Counter::setStartDelay (unsigned long val)
{
    startDelay = val;
}

void Counter::setRPMCount (unsigned long val)
{
    rpmCount = val;
}

void Counter::setSignalsPerRPM (unsigned long val)
{
    signalsPerRPM = val;
}

void Counter::start (unsigned long startTime)
{
    if (state == READY)
    {
        this->startTime = startTime;
        state = WAITING_FOR_TIMEOUT;
    }
}

void Counter::reset ()
{
    state = READY;
}

void Counter::update (unsigned long t, int rpmSignal)
{
    switch (state)
    {
    case WAITING_FOR_TIMEOUT:
    {
        if (t - startTime >= startDelay)
        {
            startTime = t; // Update start time for counting state
            signals = 0;
            state = COUNTING;
        }
        break;
    }
    case COUNTING:
    {
        // Count signals in 1 second intervals
        if (lastRpmSignal != rpmSignal && rpmSignal == HIGH)
        {
            signals++;
        }
        // 1 second interval reached
        if (t - startTime >= 1000)
        {
            float rpm = ((float) signals / (float) signalsPerRPM) * 60.0f;
            if (rpm <= rpmCount)
            {
                startTime = t; // Update start time for signaling state
                state = SIGNALING;
            }
            else
            {
                // RPM threshold not reached.
                // Count signals per second from scratch.
                //Serial.print("rpm: \r\n"); Serial.print(rpm); Serial.print("\r\n");
                startTime = t;
                signals = 0;
            }
        }
        break;
    }
    case SIGNALING:
    {
        if (t - startTime >= 3000)
        {
            state = READY;
        }
    }
    default: break;
    }
    lastRpmSignal = rpmSignal;
}

Counter::State Counter::getState () const
{
    return state;
}
