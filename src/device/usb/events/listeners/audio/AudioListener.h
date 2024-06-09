#ifndef AUDIO_LISTENER__INCLUDE
#define AUDIO_LISTENER__INCLUDE

#include "../EventListener.h"
#include "../../event/Event.h"
#include "stdint.h"

#define __INIT_AUDIO_LISTENER__(name) \
    __SUPER__(name, call) = &audio_call; \
    __SUPER__(name, type_of) = &type_of_audio; \
    __SUPER__(name, new_super_event_listener) = &new_super_event_listener; \
    \
    __CALL_SUPER__(name->super, new_super_event_listener)

struct AudioListener{
    EventListener super;
    void (*new_listener)(struct AudioListener* listener);
};

typedef struct AudioListener AudioListener;

void new_audio_listener(struct AudioListener* listener);

#endif