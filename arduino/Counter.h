#pragma once

class Counter
{
public:

    enum State
    {
        READY,
        WAITING_FOR_TIMEOUT,
        COUNTING,
        SIGNALING
    };

public:

    void init (unsigned long startDelay, unsigned long rpmCount, unsigned long signalsPerRPM);

    void setStartDelay (unsigned long);

    void setRPMCount (unsigned long);

    void setSignalsPerRPM (unsigned long);

    void start (unsigned long startTime);

    void reset ();

    void update (unsigned long time, int rpmSignal);

    State getState () const;

private:

    State state;

    unsigned long startDelay;
    unsigned long rpmCount;
    unsigned long signalsPerRPM;
    unsigned long startTime;

    unsigned long signals;

    bool lastRpmSignal;
};
