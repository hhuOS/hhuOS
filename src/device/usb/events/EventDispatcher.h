#ifndef EventDispatcher__include
#define EventDispatcher__include

#include "../interfaces/MapInterface.h"
#include "../utility/Utils.h"
#include "listeners/EventListener.h"
#include "event/Event.h"
#include "../interfaces/MutexInterface.h"

#define __INIT_EVENT_DISPATCHER(name) \
    __ENTRY__(name, publish_event) = &publish_event; \
    __ENTRY__(name, register_event_listener) = &register_event_listener; \
    __ENTRY__(name, deregister_event_listener) = &deregister_event_listener; \
    __ENTRY__(name, getListener) = &getListener; \
    __ENTRY__(name, reg_callback) = &reg_callback; \
    __ENTRY__(name, dereg_callback) = &dereg_callback; \
    __ENTRY__(name, init_event_dispatcher_map) = &init_event_dispatcher_map

struct EventDispatcher{
    void (*publish_event)(struct EventDispatcher* dispatcher, GenericEvent* event, int id);
    int (*register_event_listener)(struct EventDispatcher* dispatcher, EventListener* event_listener);
    int (*deregister_event_listener)(struct EventDispatcher* dispatcher, int id);
    EventListener* (*getListener)(struct EventDispatcher* dispatcher, int id);
    int (*reg_callback)(struct EventDispatcher* dispatcher, event_callback callback, uint16_t event_listener_type,
        void* buffer);
    int (*dereg_callback)(struct EventDispatcher* dispatcher, event_callback callback, uint16_t event_listener_type);
    void (*new_event_dispatcher)(struct EventDispatcher* event_dispatcher);
    void (*init_event_dispatcher_map)(struct EventDispatcher* event_dispatcher);

    list_head head;
    SuperMap* event_listener_map; // Map<Integer,EventListener*>
    Mutex_C* dispatcher_mutex;
    int event_listener_counter;
};

typedef struct EventDispatcher EventDispatcher;

void new_event_dispatcher(EventDispatcher* event_dispatcher);

#endif