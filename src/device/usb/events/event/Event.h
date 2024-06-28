#ifndef Event__include 
#define Event__include

#include "stdint.h"

struct GenericEvent{
    uint16_t event_value; // pressed, released or else
    uint16_t event_type; // mouse, kbd or else
    uint32_t event_code; // raw data or input event
};

typedef struct GenericEvent GenericEvent;

typedef void (*event_callback)(void* event, void* buffer);

#endif