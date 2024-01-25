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

/*void extract_mouse_event(GenericEvent* event){
    MouseEvent* m_event = (MouseEvent*)container_of(event, MouseEvent, super);
    int8_t x = m_event->x_displacement;
    int8_t y = m_event->y_displacement;
    uint16_t event_value = event->event_value;
    uint16_t event_code  = event->event_code;

    logger_c->trace_c(logger_c, "prev x-position : %d - current x-position %d", current_x_pos, current_x_pos + x);
    logger_c->trace_c(logger_c, "prev y-position : %d - current y-position %d", current_y_pos, current_y_pos + y);

    current_x_pos = current_x_pos + x;
    current_y_pos = current_y_pos + y;

    char* event_code_description = 0;
    char* event_description = 0;

    if(event_code == MOUSE_LEFT){
        event_code_description = "left click";
    }
    else if(event_code == MOUSE_RIGHT){
        event_code_description = "right click";
    }
    else if(event_code == MOUSE_SCROLL){
        event_code_description = "mid click";
    }

    if(event_value == MOUSE_ENTERED){
        event_description = "mouse entered";
    }
    else if(event_value == MOUSE_RELEASED){
        event_description = "mouse released";
    }
    else if(event_value == MOUSE_HOLD){
        event_description = "mouse hold";
    }

    logger_c->trace_c(logger_c, "%s - %s", event_description == 0 ? "mouse event detected" : event_description, event_code_description == 0 ? "" : event_code_description);
} */

uint16_t type_of_mouse(EventListener* listener){
    return MOUSE_LISTENER;
}

void new_mouse_listener(MouseListener* listener){
    listener->super.call = &mouse_call;
    listener->super.type_of = &type_of_mouse;
    listener->super.new_super_event_listener = &new_super_event_listener;
    listener->super.new_super_event_listener(&listener->super);
}