//
// Created by burak on 05.03.18.
//

#include "MouseMovedEvent.h"

MouseMovedEvent::MouseMovedEvent() : Event(TYPE) {
    dx = 0;
    dy = 0;
}

MouseMovedEvent::MouseMovedEvent(int32_t dx, int32_t dy) : Event(TYPE) {
    this->dx = dx;
    this->dy = dy;
}

char* MouseMovedEvent::getName() {
    return const_cast<char *>("MouseMovedEvent");
}

int32_t MouseMovedEvent::getXMovement() {
    return dx;
}

int32_t MouseMovedEvent::getYMovement() {
    return dy;
}