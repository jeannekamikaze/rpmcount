#include "serial_utils.h"
#include "term.h"
#include "types.h"

#include <SerialStream.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>

using namespace std;
using namespace LibSerial;

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

struct Config
{
    U32 startDelay;    // Start delay in milliseconds
    U32 rpmCount;      // Number of RPM ticks to count
    U32 signalsPerRPM; // Number of signal ticks per RPM tick
};

template <typename T>
bool writeSafe (SerialStream& serial, MessageHeader header, const T& val)
{
    write<U8>(serial, (U8)header);
    write<T>(serial, val);

    U8 errorCode = read<U8>(serial);
    T x = read<U32>(serial);

    fprintf(stderr, "error code: %d\n", errorCode);
    fprintf(stderr, "val: %d\n", x);

    return errorCode == Ok && x == val;
}

bool configureStartDelay (SerialStream& serial, Config& config)
{
    printf("Tiempo de retardo (milisegundos)\n");
    printf("--------------------------------\n");
    printf("\n");
    printf("Valor actual : %d\n", config.startDelay);
    printf("\n");
    printf("Nuevo valor  : ");

    U32 x;
    cin >> x;

    bool result = writeSafe<U32>(serial, UpdateStartDelay, x);
    if (result)
    {
        config.startDelay = x;
    }
    return result;
}

bool configureRPMCount (SerialStream& serial, Config& config)
{
    printf("RPM maximo\n");
    printf("----------\n");
    printf("\n");
    printf("Valor actual : %d\n", config.rpmCount);
    printf("\n");
    printf("Nuevo valor  : ");

    U32 x;
    cin >> x;

    bool result = writeSafe<U32>(serial, UpdateRPMCount, x);
    if (result)
    {
        config.rpmCount = x;
    }
    return result;
}

bool configureSignalsPerRPM (SerialStream& serial, Config& config)
{
    printf("Impulsos/RPM\n");
    printf("------------\n");
    printf("\n");
    printf("Valor actual : %d\n", config.signalsPerRPM);
    printf("\n");
    printf("Nuevo valor  : ");

    U32 x;
    cin >> x;

    bool result = writeSafe<U32>(serial, UpdateSignalsPerRPM, x);
    if (result)
    {
        config.signalsPerRPM = x;
    }
    return result;
}

void loop (SerialStream& serial, Config& config)
{
    int option = -1;

    while (option != 0)
    {
        system("clear");
        printf("Configuracion\n");
        printf("-------------\n");
        printf("Tiempo de retardo : %d\n", config.startDelay);
        printf("RPM maximo        : %d\n", config.rpmCount);
        printf("Impulsos/rpm      : %d\n", config.signalsPerRPM);
        printf("\n");
        printf("Menu\n");
        printf("----\n");
        printf("[0] Salir\n");
        printf("[1] Configurar tiempo de retardo\n");
        printf("[2] Configurar RPM maximo\n");
        printf("[3] Configurar impulsos/RPM\n");
        printf("\n");
        printf("> ");

        option = getch() - '0';
        system("clear");

        bool error = false;

        switch (option)
        {
        case 1: error = !configureStartDelay(serial, config); break;
        case 2: error = !configureRPMCount(serial, config); break;
        case 3: error = !configureSignalsPerRPM(serial, config); break;
        default: break;
        }

        if (error)
        {
            fprintf(stderr, "Error al escribir valor\n");
            return;
        }
    }
}

void readConfig (SerialStream& serial, Config& config)
{
    write<U8>(serial, (U8)ReadConfig);

    U8 errorCode = read<U8>(serial);
    if (errorCode == Ok)
    {
        config.startDelay    = read<U32>(serial);
        config.rpmCount      = read<U32>(serial);
        config.signalsPerRPM = read<U32>(serial);
    }
}

void openArduino (const char* serialDevice, SerialStream& serial)
{
    if (serial.IsOpen()) serial.Close();
    serial.Open(serialDevice);
    serial.SetBaudRate(SerialStreamBuf::BAUD_9600);
    serial.SetCharSize(SerialStreamBuf::CHAR_SIZE_8);
    serial.SetNumOfStopBits(1);
    serial.SetParity(SerialStreamBuf::PARITY_DEFAULT);
    serial.SetFlowControl(SerialStreamBuf::FLOW_CONTROL_DEFAULT);
}

void usage (const char* argv0)
{
    fprintf(stderr, "Uso:\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "%s <args>\n", argv0);
    fprintf(stderr, "\n");
    fprintf(stderr, "Args:\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "dispositivo  -  Nombre de dispositivo serie\n");
}

int main (int argc, char** argv)
{
    if (argc != 2)
    {
        usage(argv[0]);
        exit(0);
    }

    const char* serialDevice = argv[1];

    SerialStream serial;
    printf("Conectando con %s...\n", serialDevice);
    openArduino(serialDevice, serial);
    if (serial.IsOpen())
    {
        sleep(2); // Wait for arduino to wake up
        printf("Leyendo configuracion...\n");
        Config config;
        readConfig(serial, config);
        loop(serial, config);
    }
    else
    {
        printf("Error al conectar con %s\n", serialDevice);
        cin.get();
    }

    return 0;
}
