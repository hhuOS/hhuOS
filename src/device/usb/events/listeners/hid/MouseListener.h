#ifndef MouseListener__include
#define MouseListener__include

#include "../EventListener.h"
#include "../../event/Event.h"
#include "stdint.h"

#define __INIT_MOUSE_LISTENER__(name) \
    __SUPER__(name, call) = &mouse_call; \
    __SUPER__(name, type_of) = &type_of_mouse; \
    __SUPER__(name, new_super_event_listener) = &new_super_event_listener; \
    \
    __CALL_SUPER__(name->super, new_super_event_listener)

struct MouseListener{
    EventListener super;
    void (*new_mouse_listener)(struct MouseListener* listener);
};

typedef struct MouseListener MouseListener;

void new_mouse_listener(MouseListener* listener);

#endif