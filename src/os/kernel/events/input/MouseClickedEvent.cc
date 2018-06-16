//
// Created by burak on 05.03.18.
//

#include "MouseClickedEvent.h"

MouseClickedEvent::MouseClickedEvent() : Event(TYPE) {
    bitmask = 0;
}

MouseClickedEvent::MouseClickedEvent(uint8_t bitmask) : Event(TYPE) {
    this->bitmask = bitmask;
}

bool MouseClickedEvent::isLeftClicked() {
    return (bitmask & 0x1u) != 0;
}

bool MouseClickedEvent::isRightClicked() {
    return (bitmask & 0x2u) != 0;
}

bool MouseClickedEvent::isMiddleClicked() {
    return (bitmask & 0x4u) != 0;
}

char* MouseClickedEvent::getName() {
    return const_cast<char *>("MouseClickedEvent");
}
