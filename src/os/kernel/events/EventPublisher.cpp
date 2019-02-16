/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.d
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include "kernel/threads/Scheduler.h"
#include "EventPublisher.h"

EventPublisher::EventPublisher(Receiver &receiver) : Thread("EventPublisher", 0xff), eventQueue(), receiver(receiver) {

}

void EventPublisher::run() {

    while (isRunning) {

        while (!eventQueue.isEmpty()) {

            const Event *event = eventQueue.pop();

            receiver.onEvent(*event);
        }

        yield();
    }
}

void EventPublisher::add(const Event &event) {

    eventQueue.push(&event);
}

void EventPublisher::stop() {
    isRunning = false;
}


