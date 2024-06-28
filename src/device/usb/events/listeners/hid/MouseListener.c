#include "MouseListener.h"
#include "lib/util/usb/input/InputEvents.h"
#include "../../../include/UsbControllerInclude.h"
#include "../../../include/UsbInterface.h"
#include "../../../interfaces/LoggerInterface.h"
#include "../../../utility/Utils.h"
#include "../../event/Event.h"
#include "../../event/hid/MouseEvent.h"
#include "stdint.h"

static uint16_t type_of_mouse(EventListener* listener);
static void mouse_call(EventListener* listener, GenericEvent* event);

static void mouse_call(EventListener *listener, GenericEvent *event) {
  list_element *l_e = listener->head.l_e;
  // extract_mouse_event(event);
  // listener->listener_mutex->acquire_c(listener->listener_mutex);
  while (__NOT_NULL__(l_e)) {
    Event_Callback *e_c =
        (Event_Callback *)container_of(l_e, Event_Callback, l_e);
    e_c->callback(event, e_c->buffer);
    __UPDATE_LIST_ENTRY__(l_e);
  }
  // listener->listener_mutex->release_c(listener->listener_mutex);
}

static uint16_t type_of_mouse(EventListener *listener) { return MOUSE_LISTENER; }

void new_mouse_listener(MouseListener *listener) {
  __INIT_MOUSE_LISTENER__(listener);
}