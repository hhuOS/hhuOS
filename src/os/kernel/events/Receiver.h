#ifndef __Receiver_include__
#define __Receiver_include__

#include "Event.h"

class Receiver {

public:

    Receiver() = default;

    virtual ~Receiver() = default;

    Receiver(const Receiver &other) = delete;

    virtual void onEvent(const Event &event) = 0;

    bool operator==(const Receiver &other) {
        return this == &other;
    }

    bool operator!=(const Receiver &other) {
        return this != &other;
    }

    uint32_t hashCode() const {
        return (uint32_t) this;
    }
};


#endif
