#include "config.h"
#include "Counter.h"

#define RPM_INPUT 2
#define START_BUTTON 3
#define COUNTING_LED 4
#define TIMEOUT_LED 5
#define READY_LED 6
#define SIGNAL_PIN 7

enum ErrorCode
{
    Ok,
    Error
};

enum MessageHeader
{
    ReadConfig,
    UpdateStartDelay,
    UpdateRPMCount,
    UpdateSignalsPerRPM
};

Config config;
Counter counter;

void setup ()
{
    Serial.begin(9600);

    pinMode(RPM_INPUT, INPUT);
    pinMode(START_BUTTON, INPUT);
    pinMode(COUNTING_LED, OUTPUT);
    pinMode(TIMEOUT_LED, OUTPUT);
    pinMode(READY_LED, OUTPUT);
    pinMode(SIGNAL_PIN, OUTPUT);
    config = readConfig();
    counter.init(config.startDelay, config.rpmCount, config.signalsPerRPM);
    digitalWrite(READY_LED, HIGH);
}

void updateSerial ()
{
    unsigned long val = -1;
    bool sendACK = false;
    byte code;

    while (Serial.available() > 0)
    {
        MessageHeader header = (MessageHeader) Serial.read();
        switch (header)
        {
        case ReadConfig:
            code = Ok;
            Serial.write(code);
            Serial.write((const uint8_t*) &config.startDelay,    sizeof(config.startDelay));
            Serial.write((const uint8_t*) &config.rpmCount,      sizeof(config.rpmCount));
            Serial.write((const uint8_t*) &config.signalsPerRPM, sizeof(config.signalsPerRPM));
            Serial.flush();
            break;

        case UpdateStartDelay:
            Serial.readBytes((char*)&val, sizeof(val));
            config.startDelay = val;
            writeConfig(config);
            sendACK = true;
            break;

        case UpdateRPMCount:
            Serial.readBytes((char*)&val, sizeof(val));
            config.rpmCount = val;
            writeConfig(config);
            sendACK = true;
            break;

        case UpdateSignalsPerRPM:
            Serial.readBytes((char*)&val, sizeof(val));
            config.signalsPerRPM = val;
            writeConfig(config);
            sendACK = true;
            break;

        default:
            code = Error;
            Serial.write(code);
            Serial.flush();
            break;
        }
    }

    if (sendACK)
    {
        code = Ok;
        Serial.write(code);
        Serial.write((const uint8_t*) &val, sizeof(val));
        Serial.flush();
    }
}

void loop ()
{
    Counter::State state = counter.getState();
    if (state == Counter::READY)
    {
        if (digitalRead(START_BUTTON) == HIGH)
        {
            unsigned long t = millis();
            counter.start(t);
            digitalWrite(READY_LED, LOW);
            digitalWrite(TIMEOUT_LED, HIGH);
        }
        else updateSerial();
    }
    else if (state == Counter::WAITING_FOR_TIMEOUT)
    {
        unsigned long t = millis();
        int rpmSignal = digitalRead(RPM_INPUT);
        counter.update(t, rpmSignal);
        if (counter.getState() == Counter::COUNTING)
        {
            digitalWrite(TIMEOUT_LED, LOW);
            digitalWrite(COUNTING_LED, HIGH);
        }
    }
    else if (state == Counter::COUNTING)
    {
        unsigned long t = millis();
        int rpmSignal = digitalRead(RPM_INPUT);
        counter.update(t, rpmSignal);
        if (counter.getState() == Counter::SIGNALING)
        {
            digitalWrite(COUNTING_LED, LOW);
            digitalWrite(SIGNAL_PIN, HIGH);
        }
    }
    else if (state == Counter::SIGNALING)
    {
        unsigned long t = millis();
        counter.update(t, LOW);
        if (counter.getState() == Counter::READY)
        {
            digitalWrite(SIGNAL_PIN, LOW);
            digitalWrite(READY_LED, HIGH);
        }
    }
}
