//
// Created by burak on 09.03.18.
//

#ifndef HHUOS_MOUSEDOUBLECLICKEVENT_H
#define HHUOS_MOUSEDOUBLECLICKEVENT_H


#include <kernel/events/Event.h>

class MouseDoubleClickEvent : public Event{

public:
    MouseDoubleClickEvent() : Event(TYPE) {};

    static const uint32_t TYPE = 0x00000007;

    char* getName() override {
        return "MouseDoubleClickEvent";
    }
};


#endif //HHUOS_MOUSEDOUBLECLICKEVENT_H
