#ifndef HHUOS_TEXTDRIVERCHANGEDEVENT_H
#define HHUOS_TEXTDRIVERCHANGEDEVENT_H

#include <devices/graphics/text/TextDriver.h>
#include "kernel/events/Event.h"

class TextDriverChangedEvent : public Event {

public:

    TextDriverChangedEvent();

    explicit TextDriverChangedEvent(TextDriver *textDriver);

    TextDriverChangedEvent(const TextDriverChangedEvent &other);

    char* getName() override;

    TextDriver *getTextDriver();

    static const uint32_t TYPE   = 0x00000008;

private:

    TextDriver *textDriver = nullptr;

};

#endif
