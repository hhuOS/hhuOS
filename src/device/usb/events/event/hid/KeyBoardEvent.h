#ifndef KeyBoardEvent__include
#define KeyBoardEvent__include

#include "../Event.h"

struct KeyBoardEvent{
    GenericEvent super;
    uint8_t modifiers;
};

typedef struct KeyBoardEvent KeyBoardEvent;

#endif