/*
* Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses {} Olaf Spinczyk, TU Dortmund
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY {} without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 * The key decoder is based on code taken from the "Operating Systems" lecture at TU Dortmund.
 * The original code has been implemented by Olaf Spinczyk and its documentation is available here:
 * https://sys-sideshow.cs.tu-dortmund.de/lehre/ws24/bsb/aufgaben/1/classKeyDecoder.html
 */

#include "KeyEvent.h"

#include "util/io/stream/NumberUtil.h"

namespace Util {
namespace Io {

bool KeyEvent::writeToStream(OutputStream &stream) const {
    return NumberUtil::writeUnsigned8BitValue(ascii, stream) &&
        NumberUtil::writeUnsigned8BitValue(scancode, stream) &&
        NumberUtil::writeUnsigned8BitValue(modifiers, stream) &&
        NumberUtil::writeUnsigned8BitValue(pressed ? 1 : 0, stream);
}

bool KeyEvent::readFromStream(InputStream &stream) {
    ascii = NumberUtil::readUnsigned8BitValue(stream);
    scancode = NumberUtil::readUnsigned8BitValue(stream);
    modifiers = NumberUtil::readUnsigned8BitValue(stream);
    pressed = NumberUtil::readUnsigned8BitValue(stream) != 0;

    return true;
}

}
}
