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

#ifndef __EventPublisher_include__
#define __EventPublisher_include__

#include <lib/util/BlockingQueue.h>
#include "kernel/threads/Thread.h"
#include "kernel/events/Event.h"
#include "kernel/events/Receiver.h"
#include "kernel/lock/Spinlock.h"

/**
 * @author Filip Krakowski
 */
class EventPublisher : public Thread {

public:

    explicit EventPublisher(Receiver &receiver);

    EventPublisher(const EventPublisher &other) = delete;

    EventPublisher &operator=(const EventPublisher &other) = delete;

    ~EventPublisher();

    void run() override;

    void add(const Event &event);

    void stop();

private:

    Util::BlockingQueue<const Event*> eventQueue;

    Receiver &receiver;

    Spinlock lock;

    bool isRunning = true;

    void notify();
};


#endif
