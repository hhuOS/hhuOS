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

#ifndef __EventPublisher_include__
#define __EventPublisher_include__

#include "lib/util/SmartPointer.h"
#include "lib/util/LinkedBlockingQueue.h"
#include "kernel/thread/Thread.h"
#include "kernel/event/Event.h"
#include "kernel/event/Receiver.h"
#include "lib/async/Spinlock.h"

namespace Kernel {

/**
 * A thread with the sole purpose of forwarding events to a single receiver.
 *
 * Each receiver, that is subscribed at the EventBus, gets its own EventPublisher
 * to handle incoming events asynchronously.
 *
 * New events are first pushed into a queue (each EventPublisher has its own queue).
 * While this queue is not empty, events are popped from it and forwarded to the receiver one after another.
 *
 * @author Filip Krakowski, Fabian Ruhland
 * @date 2018
 */
class EventPublisher : public Thread {

public:

    /**
     * Constructor.
     *
     * @param receiver The receiver to forward events to.
     */
    explicit EventPublisher(Receiver &receiver);

    /**
     * Copy-constructor.
     */
    EventPublisher(const EventPublisher &other) = delete;

    /**
     * Assignment operator.
     */
    EventPublisher &operator=(const EventPublisher &other) = delete;

    /**
     * Destructor.
     */
    ~EventPublisher() override = default;

    /**
     * Overriding function from Thread.
     */
    void run() override;

    /**
     * Add new event to the queue.
     */
    void add(Util::SmartPointer<Event> event);

    /**
     * Stop the publisher. This will terminate the thread.
     */
    void stop();

private:

    Util::LinkedBlockingQueue<Util::SmartPointer<Event>> eventQueue;

    Receiver &receiver;

    bool isRunning = true;
};

}

#endif
