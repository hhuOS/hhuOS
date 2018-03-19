//
// Created by burak on 05.03.18.
//

#ifndef HHUOS_MOUSECLICKEDEVENT_H
#define HHUOS_MOUSECLICKEDEVENT_H


#include <cstdint>
#include <kernel/events/Event.h>

class MouseClickedEvent : public Event {

private:
    /*
     * 0: left clicked
     * 1: right clicked
     * 2: middle clicked
     */
    uint8_t bitmask;

public:
    MouseClickedEvent ();

    explicit MouseClickedEvent(uint8_t bitmask);

    static const uint32_t TYPE   = 0x00000004;

    char* getName() override;

    bool isLeftClicked();

    bool isRightClicked();

    bool isMiddleClicked();
};


#endif //HHUOS_MOUSECLICKEDEVENT_H
