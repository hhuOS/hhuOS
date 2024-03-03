#include "KeyBoardListener.h"
#include "../../event/Event.h"
#include "../../../../../lib/util/io/key/InputEvents.h"
#include "../../event/hid/KeyBoardEvent.h"
#include "../../../include/UsbControllerInclude.h"
#include "../../../include/UsbInterface.h"
#include "../../../interfaces/LoggerInterface.h"
#include "../../../utility/Utils.h"
#include "../EventListener.h"
#include "stdint.h"

// move as top method, since all listeners are just traversing the list_elements
void key_board_call(EventListener *listener, GenericEvent *event) {
  list_element *l_e = listener->head.l_e;
  //listener->listener_mutex->acquire_c(listener->listener_mutex);
  while (l_e != (void *)0) {
    Event_Callback *e_c =
        (Event_Callback *)container_of(l_e, Event_Callback, l_e);
    KeyBoardEvent* k_evt = container_of(event, KeyBoardEvent, super);    
    e_c->callback((void*)k_evt);
    l_e = l_e->l_e;
  }
  //listener->listener_mutex->release_c(listener->listener_mutex);
}

uint16_t type_of_key_board(EventListener *listener) {
  return KEY_BOARD_LISTENER;
}

void new_key_board_listener(struct KeyBoardListener *listener) {
  listener->super.call = &key_board_call;
  listener->super.type_of = &type_of_key_board;
  listener->super.new_super_event_listener = &new_super_event_listener;
  listener->super.new_super_event_listener(&listener->super);
}