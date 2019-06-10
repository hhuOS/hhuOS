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

#include "kernel/event/Event.h"

#ifndef HHUOS_MOUSERELEASEDEVENT_H
#define HHUOS_MOUSERELEASEDEVENT_H


class MouseReleasedEvent : public Event  {

public:
    MouseReleasedEvent ();

    explicit MouseReleasedEvent(uint8_t bitmask);

    String getType() const override;

    bool isLeftReleased();

    bool isRightReleased();

    bool isMiddleReleased();

    static const constexpr char *TYPE = "MouseReleasedEvent";

private:

    /*
     * 0: left released
     * 1: right released
     * 2: middle released
     */
    uint8_t bitmask;
};

#endif