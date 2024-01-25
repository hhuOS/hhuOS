#ifndef EventDispatcher__include
#define EventDispatcher__include

#include "../interfaces/MapInterface.h"
#include "../utility/Utils.h"
#include "listeners/EventListener.h"
#include "event/Event.h"
#include "../interfaces/MutexInterface.h"

struct EventDispatcher{
    void (*publish_event)(struct EventDispatcher* dispatcher, GenericEvent* event, int id);
    int (*register_event_listener)(struct EventDispatcher* dispatcher, EventListener* event_listener);
    int (*deregister_event_listener)(struct EventDispatcher* dispatcher, int id);
    EventListener* (*getListener)(struct EventDispatcher* dispatcher, int id);

    int (*reg_callback)(struct EventDispatcher* dispatcher, event_callback callback, uint16_t event_listener_type);
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
void init_event_dispatcher_map(EventDispatcher* event_dispatcher);

void publish_event(EventDispatcher* dispatcher,  GenericEvent* event, int id); 

int register_event_listener(EventDispatcher* dispatcher, EventListener* event_listener);
int deregister_event_listener(EventDispatcher* dispatcher, int id);

EventListener* getListener(EventDispatcher* dispatcher, int id);

int reg_callback(EventDispatcher* dispatcher, event_callback callback, uint16_t event_listener_type);
int dereg_callback(EventDispatcher* dispatcher, event_callback callback, uint16_t event_listener_type);

#endif