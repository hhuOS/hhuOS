#include "SerialEvent.h"

SerialEvent::SerialEvent() : Event(TYPE) {

}

SerialEvent::SerialEvent(Serial::ComPort port, char c) : Event(TYPE), port(port), c(c) {

}

char *SerialEvent::getName() {
    return const_cast<char *>("SerialEvent");
}

char SerialEvent::getChar() {
    return c;
}

SerialEvent::SerialEvent(const SerialEvent &other) : Event(other){
    this->c = other.c;
    this->port = other.port;
}

Serial::ComPort SerialEvent::getPortNumber() {
    return port;
}
