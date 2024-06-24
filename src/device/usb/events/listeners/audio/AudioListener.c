#include "AudioListener.h"
#include "lib/util/usb/input/InputEvents.h"
#include "../../../include/UsbControllerInclude.h"
#include "../../../include/UsbInterface.h"
#include "../../../utility/Utils.h"
#include "../../event/Event.h"
#include "../EventListener.h"
#include "stdint.h"
#include "../../event/audio/AudioSampleEvent.h"

static void audio_call(EventListener* listener, GenericEvent* event);
static uint16_t type_of_audio(EventListener* listener);

// move as top method, since all listeners are just traversing the list_elements
static void audio_call(EventListener *listener, GenericEvent *event) {
  list_element *l_e = __LIST_FIRST_ENTRY__(listener->head);
  // listener->listener_mutex->acquire_c(listener->listener_mutex);
  while (__NOT_NULL__(l_e)) {
    Event_Callback *e_c =
        (Event_Callback *)container_of(l_e, Event_Callback, l_e);
    AudioSampleEvent* a_evt = container_of(event, AudioSampleEvent, super);
    e_c->callback(a_evt, e_c->buffer);
    __UPDATE_LIST_ENTRY__(l_e);
  }
  // listener->listener_mutex->release_c(listener->listener_mutex);
}

static uint16_t type_of_audio(EventListener *listener) {
  return AUDIO_LISTENER;
}

void new_audio_listener(struct AudioListener *listener) {
  __INIT_AUDIO_LISTENER__(listener);
}