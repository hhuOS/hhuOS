//
// Created by burak on 05.03.18.
//

#include "MouseReleasedEvent.h"

MouseReleasedEvent::MouseReleasedEvent() : Event(TYPE) {
    bitmask = 0;
}

MouseReleasedEvent::MouseReleasedEvent(uint8_t bitmask) : Event(TYPE) {
    this->bitmask = bitmask;
}

bool MouseReleasedEvent::isLeftReleased() {
    return (bitmask & 0x1u) != 0;
}

bool MouseReleasedEvent::isRightReleased() {
    return (bitmask & 0x2u) != 0;
}

bool MouseReleasedEvent::isMiddleReleased() {
    return (bitmask & 0x4u) != 0;
}

char* MouseReleasedEvent::getName() {
    return const_cast<char *>("MouseReleasedEvent");
}
