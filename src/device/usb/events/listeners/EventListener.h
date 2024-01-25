#ifndef EventListener__include
#define EventListener__include

#include "../event/Event.h"
#include "../../utility/Utils.h"
#include "../../interfaces/SystemInterface.h"
#include "../../interfaces/MutexInterface.h"

struct Event_Callback{
    list_element l_e;
    event_callback callback;
};

typedef struct Event_Callback Event_Callback;

struct EventListener{
    void (*call)(struct EventListener* event_listener, GenericEvent* event);
    void (*register_event_callback)(struct EventListener* event_listener, event_callback callback);
    void (*deregister_event_callback)(struct EventListener* event_listener, event_callback callback);
    uint16_t (*type_of)(struct EventListener* event_listener);
    void (*new_super_event_listener)(struct EventListener* event_listener);
    
    list_element l_e;
    list_head head;
    Mutex_C* listener_mutex;

    int listener_id;
};

typedef struct EventListener EventListener;

void register_event_callback(struct EventListener* event_listener, event_callback callback);
void deregister_event_callback(struct EventListener* event_listener, event_callback callback);

void new_super_event_listener(struct EventListener* event_listener);

#endif