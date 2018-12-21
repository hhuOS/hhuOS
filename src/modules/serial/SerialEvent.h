#ifndef HHUOS_SERIALEVENT_H
#define HHUOS_SERIALEVENT_H

#include <devices/ports/PortEvent.h>
#include "Common.h"

namespace Serial {

template<ComPort port>
class SerialEvent : public PortEvent {

public:

    SerialEvent();

    explicit SerialEvent(char c);

    SerialEvent(const SerialEvent &other);

    SerialEvent &operator=(const SerialEvent &other);

    ~SerialEvent() override = default;

    String getType() const override;

    char getChar() override;

private:

    char c;

};

template<ComPort port>
SerialEvent<port>::SerialEvent() : c(0) {

}

template <ComPort port>
SerialEvent<port>::SerialEvent(char c) : c(c) {

}

template<ComPort port>
SerialEvent<port>::SerialEvent(const SerialEvent &other) : c(other.c) {
}

template<ComPort port>
SerialEvent<port> &SerialEvent<port>::operator=(const SerialEvent &other) {
    if (&other == this) {
        return *this;
    }

    c = other.c;

    return *this;
}

template <ComPort port>
String SerialEvent<port>::getType() const {
    return "Com" + String::valueOf(port, 10) + "Event";
}

template <ComPort port>
char SerialEvent<port>::getChar() {
    return c;
}

}

#endif
