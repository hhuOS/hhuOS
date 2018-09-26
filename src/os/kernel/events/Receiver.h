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
 * @author Filip Krakowski
 */
class Receiver {

public:

    Receiver() = default;

    virtual ~Receiver() = default;

    Receiver(const Receiver &other) = delete;

    virtual void onEvent(const Event &event) = 0;

    bool operator==(const Receiver &other) {
        return this == &other;
    }

    bool operator!=(const Receiver &other) {
        return this != &other;
    }

    uint32_t hashCode() const {
        return (uint32_t) this;
    }
};


#endif
