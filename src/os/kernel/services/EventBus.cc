/*
 * Copyright (C) 2018  Filip Krakowski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
