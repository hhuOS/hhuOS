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

#ifndef HHUOS_LFBDRIVERCHANGEDEVENT_H
#define HHUOS_LFBDRIVERCHANGEDEVENT_H

#include <devices/graphics/lfb/LinearFrameBuffer.h>
#include "kernel/events/Event.h"

class LfbDriverChangedEvent : public Event {

public:

    LfbDriverChangedEvent();

    explicit LfbDriverChangedEvent(LinearFrameBuffer *lfb);

    LfbDriverChangedEvent(const LfbDriverChangedEvent &other);

    char* getName() override;

    LinearFrameBuffer *getLinearFrameBuffer();

    static const uint32_t TYPE   = 0x00000009;

private:

    LinearFrameBuffer *lfb = nullptr;

};

#endif
