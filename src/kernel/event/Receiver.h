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

#ifndef __Receiver_include__
#define __Receiver_include__

#include "Event.h"

/**
 * Interface for classes, that want to receive events from the EventBus.
 *
 * Each receiver must call EventBus::subscribe() to subscribe itself for an arbitrary number of event types.
 * Every time an event of one of the subscribed type occurs, the receive is notified about this by Receiver::onEvent(),
 * which is called asynchronously (see EventPublisher for more detail).
 *
 * @author Filip Krakowski
 * @date 2018
 */
class Receiver {

public:

    /**
     * Constructor.
     */
    Receiver() = default;

    /**
     * Copy-constructor.
     */
    Receiver(const Receiver &other) = delete;

    /**
     * Assignment operator.
     */
    Receiver &operator=(const Receiver &other) = delete;

    /**
     * Destructor.
     */
    virtual ~Receiver() = default;

    /**
     * Handle an event. Event::is() can be used to determine the event's type.
     *
     * @param event The event to handle
     */
    virtual void onEvent(const Event &event) = 0;

    /**
     * Equal operator.
     */
    bool operator==(const Receiver &other) {
        return this == &other;
    }

    /**
     * Not equal operator.
     */
    bool operator!=(const Receiver &other) {
        return this != &other;
    }
};


#endif
