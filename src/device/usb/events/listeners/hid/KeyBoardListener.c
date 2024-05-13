#include "KeyBoardListener.h"
#include "../../../../../lib/util/io/key/InputEvents.h"
#include "../../../include/UsbControllerInclude.h"
#include "../../../include/UsbInterface.h"
#include "../../../interfaces/LoggerInterface.h"
#include "../../../utility/Utils.h"
#include "../../event/Event.h"
#include "../../event/hid/KeyBoardEvent.h"
#include "../EventListener.h"
#include "stdint.h"

static void key_board_call(EventListener* listener, GenericEvent* event);
static uint16_t type_of_key_board(EventListener* listener);

// move as top method, since all listeners are just traversing the list_elements
static void key_board_call(EventListener *listener, GenericEvent *event) {
  list_element *l_e = __LIST_FIRST_ENTRY__(listener->head);
  // listener->listener_mutex->acquire_c(listener->listener_mutex);
  while (__NOT_NULL__(l_e)) {
    Event_Callback *e_c =
        (Event_Callback *)container_of(l_e, Event_Callback, l_e);
    KeyBoardEvent *k_evt = container_of(event, KeyBoardEvent, super);
    e_c->callback((void *)k_evt);
    __UPDATE_LIST_ENTRY__(l_e);
  }
  // listener->listener_mutex->release_c(listener->listener_mutex);
}

static uint16_t type_of_key_board(EventListener *listener) {
  return KEY_BOARD_LISTENER;
}

void new_key_board_listener(struct KeyBoardListener *listener) {
  __INIT_KBD_LISTENER__(listener);
}