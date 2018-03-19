#ifndef __UsbEvent_include__
#define __UsbEvent_include__

#include "kernel/events/Event.h"

class UsbEvent : public Event {

public:

    UsbEvent();

    explicit UsbEvent(uint32_t subType);

    UsbEvent(const UsbEvent &other);

    uint32_t getSubtype();

    char *getName() override;

    static const uint32_t   TYPE    = 0x00000003;

    static const uint32_t   SUBTYPE_PORT_CHANGE = 0x00000001;

private:

    uint32_t subType;

};


#endif
