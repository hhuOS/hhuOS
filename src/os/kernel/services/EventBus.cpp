/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include <devices/graphics/lfb/LinearFrameBuffer.h>
#include <kernel/Kernel.h>
#include <lib/util/Pair.h>
#include "kernel/threads/Scheduler.h"
#include "EventBus.h"

EventBus::EventBus() : Thread("EventBus"), receiverMap(7691), eventBuffer(1024 * 64), lock() {

    isInitialized = true;
}

Scheduler *scheduler = nullptr;

void EventBus::subscribe(Receiver &receiver, uint32_t type) {

    auto *publisher = new EventPublisher(receiver);

    Util::Pair<Receiver*, uint32_t> key(&receiver, type);

    lock.acquire();

    if (receiverMap.containsKey(key)) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE);
    }

    receiverMap.put(key, publisher);

    publishers[type].add(publisher);

    registeredPublishers++;

    lock.release();

    publisher->start();
}

void EventBus::unsubscribe(Receiver &receiver, uint32_t type) {

    Util::Pair<Receiver*, uint32_t> key(&receiver, type);

    lock.acquire();

    if(!receiverMap.containsKey(key)) {

        lock.release();

        return;
    }

    EventPublisher *publisher = receiverMap.get(key);

    scheduler->kill(*publisher);

    publishers[type].remove(publisher);

    receiverMap.remove(key);

    delete publisher;

    registeredPublishers--;

    lock.release();
}

void EventBus::run() {

    g2d = Kernel::getService<GraphicsService>()->getLinearFrameBuffer();

    scheduler = Scheduler::getInstance();

    while (isRunning) {

        lock.acquire();

        notify();

        lock.release();

        Scheduler::getInstance()->yield();
    }
}

void EventBus::notify() {

    const Event *event = nullptr;

    while (!eventBuffer.isEmpty()) {

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
