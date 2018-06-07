#ifndef HHUOS_SERIALSERVICE_H
#define HHUOS_SERIALSERVICE_H

#include <devices/Serial.h>

class SerialService : public KernelService {

private:

    Serial *serial;


public:

    SerialService();

    static constexpr const char* SERVICE_NAME = "SerialService";

    Serial *getSerial();
};

#endif
