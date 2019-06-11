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

#ifndef HHUOS_MOUSECLICKEDEVENT_H
#define HHUOS_MOUSECLICKEDEVENT_H


#include <cstdint>
#include "kernel/event/Event.h"

namespace Kernel {

class MouseClickedEvent : public Event {

public:

    MouseClickedEvent();

    explicit MouseClickedEvent(uint8_t bitmask);

    String getType() const override;

    bool isLeftClicked();

    bool isRightClicked();

    bool isMiddleClicked();

    static const constexpr char *TYPE = "MouseClickedEvent";

private:

    /*
     * 0: left clicked
     * 1: right clicked
     * 2: middle clicked
     */
    uint8_t bitmask;
};

}

#endif
