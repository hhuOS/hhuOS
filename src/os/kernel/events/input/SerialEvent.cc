#include "SerialEvent.h"

SerialEvent::SerialEvent() : Event(TYPE) {

}

SerialEvent::SerialEvent(char c) : Event(TYPE) {
    this->c = c;
}

char *SerialEvent::getName() {
    return const_cast<char *>("SerialEvent");
}

char SerialEvent::getChar() {
    return c;
}

SerialEvent::SerialEvent(const SerialEvent &other) : Event(other){
    this->c = other.c;
}