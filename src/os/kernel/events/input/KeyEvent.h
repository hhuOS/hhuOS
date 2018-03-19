#ifndef __KeyEvent_include__
#define __KeyEvent_include__


#include "devices/input/Key.h"
#include "kernel/events/Event.h"

class KeyEvent : public Event {

public:

    KeyEvent();

    explicit KeyEvent(Key key);

    KeyEvent(const KeyEvent &other);

    char* getName() override;

    static const uint32_t TYPE   = 0x00000001;

    Key getKey();

private:

    Key key;

};


#endif
