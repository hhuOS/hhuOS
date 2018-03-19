
#include "kernel/threads/Scheduler.h"
#include "EventPublisher.h"

EventPublisher::EventPublisher(Receiver &receiver) : Thread("EventPublisher"), eventQueue(), receiver(receiver), lock() {

}

EventPublisher::~EventPublisher() {

}

void EventPublisher::run() {

    while (isRunning) {

        lock.lock();

        notify();

        lock.unlock();

        Scheduler::getInstance()->yield();
    }
}

void EventPublisher::add(const Event &event) {

    lock.lock();

    eventQueue.push(&event);

    lock.unlock();
}

void EventPublisher::notify() {

    while (!eventQueue.isEmpty()) {

        const Event *event = eventQueue.pop();

        receiver.onEvent(*event);
    }
}

void EventPublisher::stop() {
    isRunning = false;
}


