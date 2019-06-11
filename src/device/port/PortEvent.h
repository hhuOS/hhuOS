#ifndef HHUOS_PORTEVENT_H
#define HHUOS_PORTEVENT_H

#include "kernel/event/Event.h"

class PortEvent : public Kernel::Event {

public:

    PortEvent() = default;

    ~PortEvent() override = default;

    String getType() const override = 0;

    virtual char getChar() = 0;

};

#endif
