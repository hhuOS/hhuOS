#ifndef AUDIO_SAMPLE_EVENT__INCLUDE
#define AUDIO_SAMPLE_EVENT__INCLUDE

#include "../Event.h"

struct AudioSampleEvent{
    GenericEvent super;
    uint8_t sub_frame; // 1,2,3 or 4
};

typedef struct AudioSampleEvent AudioSampleEvent;

#endif