//
// Created by burak on 05.03.18.
//

#include <kernel/events/Event.h>

#ifndef HHUOS_MOUSERELEASEDEVENT_H
#define HHUOS_MOUSERELEASEDEVENT_H


class MouseReleasedEvent : public Event  {

private:
    /*
     * 0: left released
     * 1: right released
     * 2: middle released
     */
    uint8_t bitmask;

public:
    MouseReleasedEvent ();

    explicit MouseReleasedEvent(uint8_t bitmask);

    static const uint32_t TYPE   = 0x00000005;

    char* getName() override;

    bool isLeftReleased();

    bool isRightReleased();

    bool isMiddleReleased();
};

#endif //HHUOS_MOUSERELEASEDEVENT_H
