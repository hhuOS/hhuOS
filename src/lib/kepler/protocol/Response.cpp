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

#include "Response.h"

#include <util/io/stream/NumberUtil.h>

namespace Kepler {
namespace Response {

bool BasicResponse::writeToStream(Util::Io::OutputStream &stream) const {
    return Util::Io::NumberUtil::writeUnsigned8BitValue(success ? 1 : 0, stream);
}

bool BasicResponse::readFromStream(Util::Io::InputStream &stream) {
    success = Util::Io::NumberUtil::readUnsigned8BitValue(stream) != 0;
    return true;
}

bool CreateWindow::writeToStream(Util::Io::OutputStream &stream) const {
    return BasicResponse::writeToStream(stream) &&
        Util::Io::NumberUtil::writeUnsigned32BitValue(id, stream) &&
        Util::Io::NumberUtil::writeUnsigned16BitValue(width, stream) &&
        Util::Io::NumberUtil::writeUnsigned16BitValue(height, stream) &&
        Util::Io::NumberUtil::writeUnsigned8BitValue(colorDepth, stream);
}

bool CreateWindow::readFromStream(Util::Io::InputStream &stream) {
    BasicResponse::readFromStream(stream);
    id = Util::Io::NumberUtil::readUnsigned32BitValue(stream);
    width = Util::Io::NumberUtil::readUnsigned16BitValue(stream);
    height = Util::Io::NumberUtil::readUnsigned16BitValue(stream);
    colorDepth = Util::Io::NumberUtil::readUnsigned8BitValue(stream);

    return true;
}

}
}
