#include "EventDispatcher.h"
#include "../include/UsbInterface.h"
#include "../interfaces/SystemInterface.h"
#include "event/Event.h"
#include "listeners/EventListener.h"

void publish_event(EventDispatcher *dispatcher, GenericEvent *event, int id) {
  EventListener *event_listener = dispatcher->getListener(dispatcher, id);

  if (event_listener == (void *)0)
    return;

  event_listener->call(event_listener,
                       event); // call callbacks registered with event_listener
}

int register_event_listener(EventDispatcher *dispatcher,
                            EventListener *event_listener) {
  int count = dispatcher->event_listener_counter;
  dispatcher->event_listener_map->put_c(dispatcher->event_listener_map, &count,
                                        event_listener);
  dispatcher->event_listener_counter = count + 1;

  // dispatcher->dispatcher_mutex->acquire_c(dispatcher->dispatcher_mutex);

  list_element *l_e = dispatcher->head.l_e;
  if (l_e == (void *)0) {
    dispatcher->head.l_e = &event_listener->l_e;
    // dispatcher->dispatcher_mutex->release_c(dispatcher->dispatcher_mutex);
    return count;
  }
  while (l_e->l_e != (void *)0) {
    l_e = l_e->l_e;
  }

  l_e->l_e = &event_listener->l_e;

  // dispatcher->dispatcher_mutex->release_c(dispatcher->dispatcher_mutex);

  return count;
}
int deregister_event_listener(EventDispatcher *dispatcher, int id) {
  list_element *l_e = dispatcher->head.l_e;
  list_element *prev = 0;

  // dispatcher->dispatcher_mutex->acquire_c(dispatcher->dispatcher_mutex);

  int found = 0;
  while (l_e != (void *)0) {
    EventListener *event_listener =
        (EventListener *)container_of(l_e, EventListener, l_e);
    if (event_listener->listener_id == id) {
      found = 1;
      if (prev == (void *)0) {
        dispatcher->head.l_e = l_e->l_e;
      } else {
        prev->l_e = l_e->l_e;
      }
    }
  }

  // dispatcher->dispatcher_mutex->release_c(dispatcher->dispatcher_mutex);

  if (found) {
    dispatcher->event_listener_map->remove_c(dispatcher->event_listener_map,
                                             &id);
    return id;
  }

  return -1;
}

EventListener *getListener(EventDispatcher *dispatcher, int id) {
  if (dispatcher->event_listener_map->contains_c(dispatcher->event_listener_map,
                                                 &id)) {
    return (EventListener *)dispatcher->event_listener_map->get_c(
        dispatcher->event_listener_map, &id);
  }
  return (void *)0;
}

int reg_callback(EventDispatcher *dispatcher, event_callback callback,
                 uint16_t event_listener_type) {
  // dispatcher->dispatcher_mutex->acquire_c(dispatcher->dispatcher_mutex);

  list_element *l_e = dispatcher->head.l_e;

  while (l_e != (void *)0) {
    EventListener *listener =
        (EventListener *)container_of(l_e, EventListener, l_e);
    if (listener->type_of(listener) == event_listener_type) {
      listener->register_event_callback(listener, callback);
      // dispatcher->dispatcher_mutex->release_c(dispatcher->dispatcher_mutex);
      return 1;
    }
    l_e = l_e->l_e;
  }

  // dispatcher->dispatcher_mutex->release_c(dispatcher->dispatcher_mutex);

  return -1;
}

int dereg_callback(EventDispatcher *dispatcher, event_callback callback,
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

  return -1;
}

void new_event_dispatcher(EventDispatcher *event_dispatcher) {
  event_dispatcher->publish_event = &publish_event;
  event_dispatcher->register_event_listener = &register_event_listener;
  event_dispatcher->deregister_event_listener = &deregister_event_listener;
  event_dispatcher->getListener = &getListener;
  event_dispatcher->reg_callback = &reg_callback;
  event_dispatcher->dereg_callback = dereg_callback;
  event_dispatcher->init_event_dispatcher_map = &init_event_dispatcher_map;
  event_dispatcher->event_listener_map = 0;
  event_dispatcher->event_listener_counter = 0;
  event_dispatcher->head.l_e = 0;

  event_dispatcher->dispatcher_mutex =
      (Mutex_C *)interface_allocateMemory(sizeof(Mutex_C), 0);
  event_dispatcher->dispatcher_mutex->new_mutex = &new_mutex;
  event_dispatcher->dispatcher_mutex->new_mutex(
      event_dispatcher->dispatcher_mutex);
}

void init_event_dispatcher_map(EventDispatcher *event_dispatcher) {
  EventMap *map = (EventMap *)interface_allocateMemory(sizeof(EventMap), 0);
  map->new_map = &newEventMap;
  map->new_map(map, "Map<int,EventListener*>");

  event_dispatcher->event_listener_map = (SuperMap *)map;
}