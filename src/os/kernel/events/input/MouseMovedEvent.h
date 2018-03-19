//
// Created by burak on 05.03.18.
//

#ifndef HHUOS_MOUSEMOVEDEVENT_H
#define HHUOS_MOUSEMOVEDEVENT_H


#include <kernel/events/Event.h>

class MouseMovedEvent : public Event {
private:
    int32_t dx;
    int32_t dy;
public:
    MouseMovedEvent();

    explicit MouseMovedEvent(int32_t dx, int32_t dy);

    static const uint32_t TYPE   = 0x00000006;

    char* getName() override;

    int32_t getXMovement();
    int32_t getYMovement();
};


#endif //HHUOS_MOUSEMOVEDEVENT_H
