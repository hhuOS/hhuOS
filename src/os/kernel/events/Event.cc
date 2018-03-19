#include "Event.h"

uint32_t Event::getType() const {
    return type;
}

bool Event::is(uint32_t type) {
    return this->type == type;
}

Event::Event(const Event &other) {
    this->type = other.type;
}
