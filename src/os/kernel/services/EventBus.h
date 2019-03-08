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

#ifndef __EventBus_include__
#define __EventBus_include__


#include <lib/util/ArrayList.h>
#include <lib/util/HashMap.h>
#include <lib/util/RingBuffer.h>
#include "lib/lock/Spinlock.h"
#include "kernel/threads/Thread.h"
#include "kernel/KernelService.h"
#include "kernel/events/Receiver.h"
#include <kernel/events/EventPublisher.h>
#include "lib/util/Pair.h"

class LinearFrameBuffer;
class Scheduler;

/**
 * Publishes Events to previously registered Receivers.
 *
 * @author Filip Krakowski
 */
class EventBus : public Thread, public KernelService {

public:

    EventBus();

    EventBus(const EventBus &other) = delete;

    EventBus &operator=(const EventBus &other) = delete;

    ~EventBus() override = default;

    /**
     * Subscribes to a certain type of Event.
     *
     * @param receiver The Receiver
     * @param type The Event type
     */
    void subscribe(Receiver &receiver, const String &type);

    /**
     * Unsubscribes from a certain type of Event.
     *
     * @param receiver The Receiver
     * @param type The Event type
     */
    void unsubscribe(Receiver &receiver, const String &type);

    /**
     * Publishes an Event.
     *
     * @param event The Event
     */
    void publish(Util::SmartPointer<Event> event);

    void run () override;

    static const constexpr char* SERVICE_NAME = "EventBus";

private:

    Util::HashMap<String, Util::ArrayList<EventPublisher*>*> publishers;

    Util::HashMap<Util::Pair<Receiver*, String>, EventPublisher*> receiverMap;

    Util::ThreadSafeBlockingQueue<Util::SmartPointer<Event>> eventBuffer;

    Spinlock lock;

    Scheduler &scheduler;

    uint32_t registeredPublishers = 0;

    bool isRunning = true;

    void notify();

    bool isInitialized = false;
};


#endif
