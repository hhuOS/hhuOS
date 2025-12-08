/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

#include "MouseRunnable.h"

#include "Protocol.h"
#include "util/io/stream/NumberUtil.h"

void Kepler::MouseRunnable::run() {
    while (isRunning) {
        const auto eventType = static_cast<Event::Type>(Util::Io::NumberUtil::readUnsigned8BitValue(mouseInputStream));
        switch (eventType) {
            case Event::MOUSE_HOVER: {
                auto event = Event::MouseHover();
                event.readFromStream(mouseInputStream);

                if (listener != nullptr) {
                    listener->onMouseHover(event.getPosX(), event.getPosY());
                }

                break;
            }
            case Event::MOUSE_CLICK: {
                auto event = Event::MouseClick();
                event.readFromStream(mouseInputStream);

                if (listener != nullptr) {
                    listener->onMouseClick(event.getPosX(), event.getPosY(), event.getButton(), event.getAction());
                }

                break;
            }
            default:
                Util::Panic::fire(Util::Panic::INVALID_ARGUMENT, "MouseRunnable: Unknown mouse event type received");
        }
    }
}
