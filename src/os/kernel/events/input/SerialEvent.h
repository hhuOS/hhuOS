#ifndef HHUOS_SERIALEVENT_H
#define HHUOS_SERIALEVENT_H

#include <kernel/events/Event.h>

class SerialEvent : public Event {

public:

    SerialEvent();

    explicit SerialEvent(char c);

    SerialEvent(const SerialEvent &other);

    char* getName() override;

    char getChar();

    static const uint32_t TYPE   = 0x00000007;

private:

    char c;
};

#endif
