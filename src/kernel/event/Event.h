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

#ifndef __Event_include__
#define __Event_include__

#include <cstdint>
#include "lib/string/String.h"

namespace Kernel {

/**
 * Base class for events, that are distributed by the EventBus.
 *
 * @author Filip Krakowski
 * @date 2018
 */
class Event {

public:

    /**
     * Constructor.
     */
    explicit Event() = default;

    /**
     * Destructor.
     */
    virtual ~Event() = default;

    /**
     * Check if an event is of a specific type.
     *
     * @param type The type to check the event against
     *
     * @return true, if the event if of the specified type
     */
    bool is(const String &type);

    /**
     * Get the event's type. This should be an easy to memorize name (e.g. KeyEvent for handling Keyboard input).
     */
    virtual String getType() const = 0;

};

}

#endif
