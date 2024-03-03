#include "MouseListener.h"
#include "../../event/Event.h"
#include "../../../../../lib/util/io/key/InputEvents.h"
#include "../../event/hid/MouseEvent.h"
#include "../../../include/UsbControllerInclude.h"
#include "../../../include/UsbInterface.h"
#include "../../../utility/Utils.h"
#include "../../../interfaces/LoggerInterface.h"
#include "stdint.h"

void mouse_call(EventListener* listener, GenericEvent* event){
    list_element* l_e = listener->head.l_e;
    //extract_mouse_event(event);
    //listener->listener_mutex->acquire_c(listener->listener_mutex);
    while(l_e != (void*)0){
        Event_Callback* e_c = (Event_Callback*)container_of(l_e, Event_Callback, l_e);
        e_c->callback(event);
        l_e = l_e->l_e;
    }
    //listener->listener_mutex->release_c(listener->listener_mutex);
}

uint16_t type_of_mouse(EventListener* listener){
    return MOUSE_LISTENER;
}

void new_mouse_listener(MouseListener* listener){
    listener->super.call = &mouse_call;
    listener->super.type_of = &type_of_mouse;
    listener->super.new_super_event_listener = &new_super_event_listener;
    listener->super.new_super_event_listener(&listener->super);
}