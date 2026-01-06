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
 *
 * The UDP/IP stack is based on a bachelor's thesis, written by Hannes Feil.
 * The original source code can be found here: https://github.com/hhuOS/hhuOS/tree/legacy/network
 */

#include "EthernetHeader.h"

#include "../../io/stream/NumberUtil.h"

namespace Util {
namespace Network {
namespace Ethernet {

void EthernetHeader::read(Io::InputStream &stream) {
    destinationAddress.read(stream);
    sourceAddress.read(stream);
    etherType = static_cast<EtherType>(Io::NumberUtil::readUnsigned16BitValue(stream));
}

void EthernetHeader::write(Io::OutputStream &stream) const {
    destinationAddress.write(stream);
    sourceAddress.write(stream);
    Io::NumberUtil::writeUnsigned16BitValue(etherType, stream);
}

}
}
}