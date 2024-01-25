#ifndef Event__include 
#define Event__include

#include "stdint.h"

struct GenericEvent{
    uint16_t event_value; // pressed, released or else
    uint16_t event_type; // mouse, kbd or else
    uint16_t event_code; 
};

typedef struct GenericEvent GenericEvent;

typedef void (*event_callback)(void* event);

#endif