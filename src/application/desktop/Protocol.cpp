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

#include "Protocol.h"

#include "async/Process.h"
#include "io/stream/NumberUtil.h"

bool CreateWindowRequest::writeToStream(Util::Io::OutputStream &stream) const {
    Util::Io::NumberUtil::writeUnsigned16BitValue(resX, stream);
    Util::Io::NumberUtil::writeUnsigned16BitValue(resY, stream);

    Util::Io::NumberUtil::writeUnsigned32BitValue(processId, stream);

    Util::Io::NumberUtil::writeUnsigned32BitValue(pipeName.length(), stream);
    return stream.write(static_cast<const uint8_t*>(pipeName), 0, pipeName.length()) == pipeName.length();
}

Util::String CreateWindowRequest::getPipePath() const {
    return Util::String::format("/process/%u/pipes/%s", processId, static_cast<const char*>(pipeName));
}

CreateWindowRequest CreateWindowRequest::readFromStream(Util::Io::InputStream &stream) {
    const auto resX = Util::Io::NumberUtil::readUnsigned16BitValue(stream);
    const auto resY = Util::Io::NumberUtil::readUnsigned16BitValue(stream);

    const auto processId = Util::Io::NumberUtil::readUnsigned32BitValue(stream);

    const auto nameLength = Util::Io::NumberUtil::readUnsigned32BitValue(stream);
    auto *nameBuffer = new char[nameLength + 1];
    stream.read(reinterpret_cast<uint8_t*>(nameBuffer), 0, nameLength);
    nameBuffer[nameLength] = '\0';
    const auto pipeName = Util::String(nameBuffer);
    delete[] nameBuffer;

    return CreateWindowRequest{resX, resY, processId, pipeName};
}

void CreateWindowResponse::writeToStream(Util::Io::OutputStream &stream) const {
    Util::Io::NumberUtil::writeUnsigned16BitValue(resX, stream);
    Util::Io::NumberUtil::writeUnsigned16BitValue(resY, stream);
    Util::Io::NumberUtil::writeUnsigned8BitValue(colorDepth, stream);

    Util::Io::NumberUtil::writeUnsigned32BitValue(sharedBufferId, stream);
    Util::Io::NumberUtil::writeUnsigned32BitValue(sharedBufferPageCount, stream);
}

CreateWindowResponse CreateWindowResponse::readFromStream(Util::Io::InputStream &stream) {
    const auto resX = Util::Io::NumberUtil::readUnsigned16BitValue(stream);
    const auto resY = Util::Io::NumberUtil::readUnsigned16BitValue(stream);
    const auto colorDepth = Util::Io::NumberUtil::readUnsigned8BitValue(stream);

    const auto sharedBufferId = Util::Io::NumberUtil::readUnsigned32BitValue(stream);
    const auto sharedBufferPageCount = Util::Io::NumberUtil::readUnsigned32BitValue(stream);

    return CreateWindowResponse{resX, resY, colorDepth, sharedBufferId, sharedBufferPageCount};
}
