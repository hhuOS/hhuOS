#ifndef HHUOS_LFBDRIVERCHANGEDEVENT_H
#define HHUOS_LFBDRIVERCHANGEDEVENT_H

#include <devices/graphics/lfb/LinearFrameBuffer.h>
#include "kernel/events/Event.h"

class LfbDriverChangedEvent : public Event {

public:

    LfbDriverChangedEvent();

    explicit LfbDriverChangedEvent(LinearFrameBuffer *lfb);

    LfbDriverChangedEvent(const LfbDriverChangedEvent &other);

    char* getName() override;

    LinearFrameBuffer *getLinearFrameBuffer();

    static const uint32_t TYPE   = 0x00000009;

private:

    LinearFrameBuffer *lfb = nullptr;

};

#endif
