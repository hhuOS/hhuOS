
#include <devices/graphics/lfb/LinearFrameBuffer.h>
#include <kernel/Kernel.h>
#include "kernel/threads/Scheduler.h"
#include "EventBus.h"

EventBus::EventBus() : Thread("EventBus"), receiverMap(7691), eventBuffer(1024 * 64), lock() {

    //g2d = Kernel::getService<LinearFrameBuffer>();

    isInitialized = true;
}

void EventBus::subscribe(Receiver &receiver, uint32_t type) {

    EventPublisher *publisher = new EventPublisher(receiver);

    lock.lock();

    receiverMap.put(&receiver, publisher);

    publishers[type].add(publisher);

    lock.unlock();

    publisher->start();
}

void EventBus::unsubscribe(const Receiver &receiver, uint32_t type) {

    EventPublisher* publisher = receiverMap.get(&receiver);

    lock.lock();

    receiverMap.remove(&receiver);

    publishers[type].remove(publisher);

    lock.unlock();
}

void EventBus::run() {

    while (isRunning) {

        lock.lock();

        notify();

        lock.unlock();

        Scheduler::getInstance()->yield();
    }
}

void EventBus::notify() {

    //g2d->placeFilledRect(10, 10, 5, 80, Colors::RED);

    const Event *event = nullptr;

    while (!eventBuffer.isEmpty()) {

        //g2d->placeFilledRect(85, 10, 5, 80, Colors::GREEN);

        event = eventBuffer.pop();

        Util::ArrayList<EventPublisher*> &publisherList = publishers[event->getType()];

        for (EventPublisher *publisher : publisherList) {

            publisher->add(*event);
        }
    }

}

void EventBus::publish(const Event &event) {

    if (!isInitialized) {
        return;
    }

    eventBuffer.push(&event);

    // TODO(krakowski)
    //  Wake up EventBus thread with priority boost
}
