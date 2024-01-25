#ifndef MouseListener__include
#define MouseListener__include

#include "../EventListener.h"
#include "../../event/Event.h"
#include "stdint.h"

struct MouseListener{
    EventListener super;
    void (*new_mouse_listener)(struct MouseListener* listener);
};

typedef struct MouseListener MouseListener;

void mouse_call(EventListener* listener, GenericEvent* event);
void new_mouse_listener(MouseListener* listener);
uint16_t type_of_mouse(EventListener* listener);
void extract_mouse_event(GenericEvent* event);

#endif