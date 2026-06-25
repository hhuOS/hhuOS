/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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

#include "Event.h"

#include <util/io/stream/NumberUtil.h>

namespace Kepler {
namespace Event {

bool BasicEvent::writeToStream(Util::Io::OutputStream &stream) const {
    return Util::Io::NumberUtil::writeUnsigned32BitValue(windowId, stream);
}

bool BasicEvent::readFromStream(Util::Io::InputStream &stream) {
    windowId = Util::Io::NumberUtil::readUnsigned32BitValue(stream);
    return true;
}

bool MouseHover::writeToStream(Util::Io::OutputStream &stream) const {
    return Util::Io::NumberUtil::writeUnsigned8BitValue(MOUSE_HOVER, stream) &&
        BasicEvent::writeToStream(stream) &&
        Util::Io::NumberUtil::writeUnsigned16BitValue(posX, stream) &&
        Util::Io::NumberUtil::writeUnsigned16BitValue(posY, stream);
}

bool MouseHover::readFromStream(Util::Io::InputStream &stream) {
    BasicEvent::readFromStream(stream);
    posX = Util::Io::NumberUtil::readUnsigned16BitValue(stream);
    posY = Util::Io::NumberUtil::readUnsigned16BitValue(stream);

    return true;
}

bool MouseClick::writeToStream(Util::Io::OutputStream &stream) const {
    return Util::Io::NumberUtil::writeUnsigned8BitValue(MOUSE_CLICK, stream) &&
        BasicEvent::writeToStream(stream) &&
        Util::Io::NumberUtil::writeUnsigned16BitValue(posX, stream) &&
        Util::Io::NumberUtil::writeUnsigned16BitValue(posY, stream) &&
        Util::Io::NumberUtil::writeUnsigned8BitValue(button, stream) &&
        Util::Io::NumberUtil::writeUnsigned8BitValue(action, stream);
}

bool MouseClick::readFromStream(Util::Io::InputStream &stream) {
    BasicEvent::readFromStream(stream);
    posX = Util::Io::NumberUtil::readUnsigned16BitValue(stream);
    posY = Util::Io::NumberUtil::readUnsigned16BitValue(stream);
    button = static_cast<Button>(Util::Io::NumberUtil::readUnsigned8BitValue(stream));
    action = static_cast<Action>(Util::Io::NumberUtil::readUnsigned8BitValue(stream));

    return true;
}

bool KeyEvent::writeToStream(Util::Io::OutputStream &stream) const {
    return Util::Io::NumberUtil::writeUnsigned8BitValue(KEY_EVENT, stream) &&
        BasicEvent::writeToStream(stream) &&
        key.writeToStream(stream);
}

bool KeyEvent::readFromStream(Util::Io::InputStream &stream) {
    return BasicEvent::readFromStream(stream) &&
        key.readFromStream(stream);
}

bool WindowCloseEvent::writeToStream(Util::Io::OutputStream &stream) const {
    return Util::Io::NumberUtil::writeUnsigned8BitValue(WINDOW_CLOSE, stream) &&
        BasicEvent::writeToStream(stream);
}

bool WindowCloseEvent::readFromStream(Util::Io::InputStream &stream) {
    return BasicEvent::readFromStream(stream);
}

}
}
