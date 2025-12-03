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
 *
 * The UDP/IP stack is based on a bachelor's thesis, written by Hannes Feil.
 * The original source code can be found here: https://github.com/hhuOS/hhuOS/tree/legacy/network
 */

#include "lib/util/network/udp/UdpHeader.h"

#include "../../io/stream/NumberUtil.h"

namespace Util {
namespace Network {
namespace Udp {

void UdpHeader::read(Io::InputStream &stream) {
    sourcePort = Io::NumberUtil::readUnsigned16BitValue(stream);
    destinationPort = Io::NumberUtil::readUnsigned16BitValue(stream);
    datagramLength = Io::NumberUtil::readUnsigned16BitValue(stream);
    checksum = Io::NumberUtil::readUnsigned16BitValue(stream);
}

void UdpHeader::write(Io::OutputStream &stream) const {
    Io::NumberUtil::writeUnsigned16BitValue(sourcePort, stream);
    Io::NumberUtil::writeUnsigned16BitValue(destinationPort, stream);
    Io::NumberUtil::writeUnsigned16BitValue(datagramLength, stream);
    Io::NumberUtil::writeUnsigned16BitValue(checksum, stream);
}

}
}
}