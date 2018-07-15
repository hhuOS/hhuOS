#include "LfbDriverChangedEvent.h"

LfbDriverChangedEvent::LfbDriverChangedEvent() : Event(TYPE) {

}


LfbDriverChangedEvent::LfbDriverChangedEvent(LinearFrameBuffer *lfb) : Event(TYPE), lfb(lfb){

}

LfbDriverChangedEvent::LfbDriverChangedEvent(const LfbDriverChangedEvent &other) : Event(other) {
    this->lfb = other.lfb;
}

char *LfbDriverChangedEvent::getName() {
    return const_cast<char *>("LfbDriverChangedEvent");
}

LinearFrameBuffer *LfbDriverChangedEvent::getLinearFrameBuffer() {
    return lfb;
}