#include "TextDriverChangedEvent.h"

TextDriverChangedEvent::TextDriverChangedEvent() : Event(TYPE) {

}

TextDriverChangedEvent::TextDriverChangedEvent(TextDriver *textDriver) : Event(TYPE), textDriver(textDriver) {

}

TextDriverChangedEvent::TextDriverChangedEvent(const TextDriverChangedEvent &other) : Event(other) {
    this->textDriver = other.textDriver;
}

char *TextDriverChangedEvent::getName() {
    return const_cast<char *>("TextDriverChangedEvent");
}

TextDriver *TextDriverChangedEvent::getTextDriver() {
    return textDriver;
}
