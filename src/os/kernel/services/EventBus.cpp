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
#include <kernel/threads/Scheduler.h>
#include "EventBus.h"

EventBus::EventBus() : Thread("EventBus", 0xff), receiverMap(), scheduler(Scheduler::getInstance()) {

    isInitialized = true;
}

void EventBus::subscribe(Receiver &receiver, const String &type) {

    auto *publisher = new EventPublisher(receiver);

    Util::Pair<Receiver*, String> key(&receiver, type);

    lock.acquire();

    if (receiverMap.containsKey(key)) {

        const char *errorMessage = (const char*) String::format("EventBus: Receiver is already subscribed for events of type '%s'!", (const char*) type);

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE, errorMessage);
    }

    receiverMap.put(key, publisher);

    if(!publishers.containsKey(type)) {
        publishers.put(type, new Util::ArrayList<EventPublisher*>);
    }

    publishers.get(type)->add(publisher);

    registeredPublishers++;

    lock.release();

    publisher->start();
}

void EventBus::unsubscribe(Receiver &receiver, const String &type) {

    Util::Pair<Receiver*, String> key(&receiver, type);

    lock.acquire();

    if(!receiverMap.containsKey(key)) {

        lock.release();

        return;
    }

    EventPublisher *publisher = receiverMap.get(key);

    scheduler.kill(*publisher);

    publishers.get(type)->remove(publisher);

    if(publishers.get(type)->isEmpty()) {
        publishers.remove(type);
    }

    receiverMap.remove(key);

    delete publisher;

    registeredPublishers--;

    lock.release();
}

void EventBus::run() {

    while (isRunning) {

        lock.acquire();

        notify();

        lock.release();

        yield();
    }
}

void EventBus::notify() {

    while (!eventBuffer.isEmpty()) {

        Util::SmartPointer<Event> event = eventBuffer.pop();

        if(!publishers.containsKey(event->getType())) {
            return;
        }

        Util::ArrayList<EventPublisher*> &publisherList = *publishers.get(event->getType());

        for (EventPublisher *publisher : publisherList) {

            publisher->add(event);
        }
    }

}

void EventBus::publish(Util::SmartPointer<Event> event) {

    if (!isInitialized) {
        return;
    }

    eventBuffer.push(event);

    // TODO(krakowski)
    //  Wake up EventBus thread with priority boost
}
