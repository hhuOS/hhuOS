#ifndef HHUOS_SERIALSERVICE_H
#define HHUOS_SERIALSERVICE_H

#include <devices/Serial.h>

class SerialService : public KernelService {

private:

    Serial *com1 = nullptr;
    Serial *com2 = nullptr;
    Serial *com3 = nullptr;
    Serial *com4 = nullptr;

public:

    SerialService();

    static constexpr const char* SERVICE_NAME = "SerialService";

    Serial *getSerialPort(Serial::ComPort port);

    bool isPortAvailable(Serial::ComPort port);
};

#endif
