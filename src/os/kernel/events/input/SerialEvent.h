#ifndef HHUOS_SERIALEVENT_H
#define HHUOS_SERIALEVENT_H

#include <kernel/events/Event.h>
#include <devices/Serial.h>

class SerialEvent : public Event {

public:

    SerialEvent();

    explicit SerialEvent(Serial::ComPort port, char c);

    SerialEvent(const SerialEvent &other);

    char* getName() override;

    char getChar();

    Serial::ComPort getPortNumber();

    static const uint32_t TYPE   = 0x00000007;

private:

    Serial::ComPort port;

    char c;
};

#endif
