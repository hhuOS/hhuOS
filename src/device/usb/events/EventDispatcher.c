#include "EventDispatcher.h"
#include "../include/UsbInterface.h"
#include "../interfaces/SystemInterface.h"
#include "event/Event.h"
#include "listeners/EventListener.h"

static void init_event_dispatcher_map(EventDispatcher* event_dispatcher);
static void publish_event(EventDispatcher* dispatcher,  GenericEvent* event, int id); 
static int register_event_listener(EventDispatcher* dispatcher, EventListener* event_listener);
static int deregister_event_listener(EventDispatcher* dispatcher, int id);
static EventListener* getListener(EventDispatcher* dispatcher, int id);
static int reg_callback(EventDispatcher* dispatcher, event_callback callback, uint16_t event_listener_type,
  void* buffer);
static int dereg_callback(EventDispatcher* dispatcher, event_callback callback, uint16_t event_listener_type);

static void publish_event(EventDispatcher *dispatcher, GenericEvent *event, int id) {
  EventListener *event_listener = __STRUCT_CALL__(dispatcher, getListener, id);
  __IF_RET__(__IS_NULL__(event_listener));
  __STRUCT_CALL__(event_listener, call, event);
    // call callbacks registered within event_listener
}

static int register_event_listener(EventDispatcher *dispatcher,
                            EventListener *event_listener) {
  int count = dispatcher->event_listener_counter;
  __MAP_PUT__(dispatcher->event_listener_map, &count, event_listener);
  dispatcher->event_listener_counter = count + 1;

  // dispatcher->dispatcher_mutex->acquire_c(dispatcher->dispatcher_mutex);

  list_element *l_e = __LIST_FIRST_ENTRY__(dispatcher->head);
  if (__IS_NULL__(l_e)) {
    __LIST_ADD_FIRST_ENTRY__(dispatcher->head, &event_listener->l_e);
    // dispatcher->dispatcher_mutex->release_c(dispatcher->dispatcher_mutex);
    return count;
  }
  __LIST_TRAVERSE__(l_e);

  __LIST_NEXT_ENTRY__(l_e, &event_listener->l_e);

  // dispatcher->dispatcher_mutex->release_c(dispatcher->dispatcher_mutex);
  return count;
}
static int deregister_event_listener(EventDispatcher *dispatcher, int id) {
  list_element *l_e = __LIST_FIRST_ENTRY__(dispatcher->head);
  list_element *prev = 0;

  // dispatcher->dispatcher_mutex->acquire_c(dispatcher->dispatcher_mutex);

  int found = 0;
  while (__NOT_NULL__(l_e)) {
    EventListener *event_listener =
        (EventListener *)container_of(l_e, EventListener, l_e);
    if (event_listener->listener_id == id) {
      found = 1;
      if (__IS_NULL__(prev)) {
        __LIST_ADD_FIRST_ENTRY__(dispatcher->head, l_e->l_e);
      } else {
        __LIST_NEXT_ENTRY__(prev, l_e->l_e);
      }
    }
  }
  // dispatcher->dispatcher_mutex->release_c(dispatcher->dispatcher_mutex);
  __IF_CUSTOM__(found, __MAP_REMOVE__(dispatcher->event_listener_map, void*,
    &id); return id);

  return __RET_E__;
}

static EventListener *getListener(EventDispatcher *dispatcher, int id) {
  return __MAP_GET__(dispatcher->event_listener_map, EventListener*, &id);
}

static int reg_callback(EventDispatcher *dispatcher, event_callback callback,
                 uint16_t event_listener_type, void* buffer) {
  // dispatcher->dispatcher_mutex->acquire_c(dispatcher->dispatcher_mutex);

  list_element *l_e = __LIST_FIRST_ENTRY__(dispatcher->head);

  while (l_e != (void *)0) {
    EventListener *listener =
        (EventListener *)container_of(l_e, EventListener, l_e);
    if (listener->type_of(listener) == event_listener_type) {
      listener->register_event_callback(listener, callback, buffer);
      // dispatcher->dispatcher_mutex->release_c(dispatcher->dispatcher_mutex);
      return 1;
    }
    l_e = l_e->l_e;
  }

  // dispatcher->dispatcher_mutex->release_c(dispatcher->dispatcher_mutex);

  return -1;
}

static int dereg_callback(EventDispatcher *dispatcher, event_callback callback,
                   uint16_t event_listener_type) {
  // dispatcher->dispatcher_mutex->acquire_c(dispatcher->dispatcher_mutex);

  list_element *l_e = dispatcher->head.l_e;

  while (l_e != (void *)0) {
    EventListener *listener =
        (EventListener *)container_of(l_e, EventListener, l_e);
    if (listener->type_of(listener) == event_listener_type) {
      listener->deregister_event_callback(listener, callback);
      // dispatcher->dispatcher_mutex->release_c(dispatcher->dispatcher_mutex);
      return 1;
    }
    l_e = l_e->l_e;
  }

  // dispatcher->dispatcher_mutex->release_c(dispatcher->dispatcher_mutex);

  return __RET_E__;
}

void new_event_dispatcher(EventDispatcher *event_dispatcher) {
  event_dispatcher->event_listener_map = 0;
  event_dispatcher->event_listener_counter = 0;
  event_dispatcher->head.l_e = 0;

  event_dispatcher->dispatcher_mutex =
      (Mutex_C *)interface_allocateMemory(sizeof(Mutex_C), 0);
  __STRUCT_INIT__(event_dispatcher->dispatcher_mutex, new_mutex, new_mutex);
  __INIT_EVENT_DISPATCHER(event_dispatcher);
}

static void init_event_dispatcher_map(EventDispatcher *event_dispatcher) {
  EventMap *map = (EventMap *)interface_allocateMemory(sizeof(EventMap), 0);
  map->new_map = &newEventMap;
  map->new_map(map, "Map<int,EventListener*>");

  event_dispatcher->event_listener_map = (SuperMap *)map;
}