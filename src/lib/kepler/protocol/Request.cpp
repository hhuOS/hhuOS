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

#include "Request.h"

#include <util/io/stream/NumberUtil.h>

namespace Kepler {
namespace Request {

bool Connect::writeToStream(Util::Io::OutputStream &stream) const {
    return Util::Io::NumberUtil::writeUnsigned8BitValue(CONNECT, stream) &&
        Util::Io::NumberUtil::writeUnsigned32BitValue(processId, stream) &&
        pipeName.writeToStream(stream);
}

bool Connect::readFromStream(Util::Io::InputStream &stream) {
    processId = Util::Io::NumberUtil::readUnsigned32BitValue(stream);
    return pipeName.readFromStream(stream);
}

bool CreateWindow::writeToStream(Util::Io::OutputStream &stream) const {
    return Util::Io::NumberUtil::writeUnsigned8BitValue(CREATE_WINDOW, stream) &&
        Util::Io::NumberUtil::writeUnsigned16BitValue(width, stream) &&
        Util::Io::NumberUtil::writeUnsigned16BitValue(height, stream) &&
        title.writeToStream(stream);
}

bool CreateWindow::readFromStream(Util::Io::InputStream &stream) {
    width = Util::Io::NumberUtil::readUnsigned16BitValue(stream);
    height = Util::Io::NumberUtil::readUnsigned16BitValue(stream);
    return title.readFromStream(stream);
}

bool SetWindowTitle::writeToStream(Util::Io::OutputStream &stream) const {
    return Util::Io::NumberUtil::writeUnsigned8BitValue(SET_WINDOW_TITLE, stream) &&
        title.writeToStream(stream);
}

bool SetWindowTitle::readFromStream(Util::Io::InputStream &stream) {
    return title.readFromStream(stream);
}

bool BasicWindowRequest::writeToStream(Util::Io::OutputStream &stream) const {
    return Util::Io::NumberUtil::writeUnsigned8BitValue(command, stream) &&
        Util::Io::NumberUtil::writeUnsigned32BitValue(windowId, stream);
}

bool BasicWindowRequest::readFromStream(Util::Io::InputStream &stream) {
    windowId = Util::Io::NumberUtil::readUnsigned32BitValue(stream);
    return true;
}

}
}
