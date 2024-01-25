#ifndef MouseEvent__include
#define MouseEvent__include

#include "../Event.h"

struct MouseEvent{
    GenericEvent super;
    int8_t x_displacement;
    int8_t y_displacement;
    int8_t z_displacement; // mouse wheel scroll
};

typedef struct MouseEvent MouseEvent;

#endif