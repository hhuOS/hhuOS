#ifndef KeyBoardListener__include
#define KeyBoardListener__include

#include "../EventListener.h"
#include "../../event/Event.h"
#include "stdint.h"

#define __INIT_KBD_LISTENER__(name) \
    __SUPER__(name, call) = &key_board_call; \
    __SUPER__(name, type_of) = &type_of_key_board; \
    __SUPER__(name, new_super_event_listener) = &new_super_event_listener; \
    \
    __CALL_SUPER__(name->super, new_super_event_listener)

struct KeyBoardListener{
    EventListener super;
    void (*new_listener)(struct KeyBoardListener* listener);
};

typedef struct KeyBoardListener KeyBoardListener;

void new_key_board_listener(struct KeyBoardListener* listener);

#endif