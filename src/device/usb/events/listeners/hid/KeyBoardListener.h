#ifndef KeyBoardListener__include
#define KeyBoardListener__include

#include "../EventListener.h"
#include "../../event/Event.h"
#include "stdint.h"

struct KeyBoardListener{
    EventListener super;
    void (*new_listener)(struct KeyBoardListener* listener);
};

typedef struct KeyBoardListener KeyBoardListener;

void key_board_call(EventListener* listener, GenericEvent* event);
void new_key_board_listener(struct KeyBoardListener* listener);
uint16_t type_of_key_board(EventListener* listener);
void extract_event(GenericEvent* event);

#endif