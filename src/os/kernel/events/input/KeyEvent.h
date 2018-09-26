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

#ifndef __KeyEvent_include__
#define __KeyEvent_include__


#include "devices/input/Key.h"
#include "kernel/events/Event.h"

class KeyEvent : public Event {

public:

    enum Keys {
        ESCAPE = 1,
        RETURN = 28,
        PLUS = 43,
        MINUS = 45,
        PRINT = 55,
        SPACE = 57,
        UP = 72,
        LEFT = 75,
        RIGHT = 77,
        DOWN = 80
    };

    KeyEvent();

    explicit KeyEvent(Key key);

    KeyEvent(const KeyEvent &other);

    char* getName() override;

    static const uint32_t TYPE   = 0x00000001;

    Key getKey();

private:

    Key key;

};


#endif
