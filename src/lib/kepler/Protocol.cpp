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

#include "util/async/Process.h"
#include "util/io/stream/NumberUtil.h"

namespace Kepler {

namespace Request {

Connect::Connect(const size_t processId, const Util::String &pipeName) : processId(processId), pipeName(pipeName) {}

bool Connect::writeToStream(Util::Io::OutputStream &stream) const {
    return Util::Io::NumberUtil::writeUnsigned8BitValue(CONNECT, stream) &&
        Util::Io::NumberUtil::writeUnsigned32BitValue(processId, stream) &&
        pipeName.writeToStream(stream);
}

bool Connect::readFromStream(Util::Io::InputStream &stream) {
    processId = Util::Io::NumberUtil::readUnsigned32BitValue(stream);
    return pipeName.readFromStream(stream);
}

Util::String Connect::getPipePath() const {
    return Util::String::format("/process/%u/pipes/%s", processId, static_cast<const char*>(pipeName));
}

CreateWindow::CreateWindow(const uint16_t sizeX, const uint16_t sizeY, const Util::String &title) :
    sizeX(sizeX), sizeY(sizeY), title(title) {}

bool CreateWindow::writeToStream(Util::Io::OutputStream &stream) const {
    return Util::Io::NumberUtil::writeUnsigned8BitValue(CREATE_WINDOW, stream) &&
        Util::Io::NumberUtil::writeUnsigned16BitValue(sizeX, stream) &&
        Util::Io::NumberUtil::writeUnsigned16BitValue(sizeY, stream) &&
        title.writeToStream(stream);
}

bool CreateWindow::readFromStream(Util::Io::InputStream &stream) {
    sizeX = Util::Io::NumberUtil::readUnsigned16BitValue(stream);
    sizeY = Util::Io::NumberUtil::readUnsigned16BitValue(stream);
    return title.readFromStream(stream);
}

uint16_t CreateWindow::getSizeX() const {
    return sizeX;
}

uint16_t CreateWindow::getSizeY() const {
    return sizeY;
}

Util::String CreateWindow::getTitle() const {
    return title;
}

bool Flush::writeToStream(Util::Io::OutputStream &stream) const {
    return Util::Io::NumberUtil::writeUnsigned8BitValue(FLUSH, stream);
}

bool Flush::readFromStream([[maybe_unused]] Util::Io::InputStream &stream) {
    return true;
}

}

namespace Response {

CreateWindow::CreateWindow(const uint16_t sizeX, const uint16_t sizeY, const uint8_t colorDepth,
    const size_t sharedBufferId) : sizeX(sizeX), sizeY(sizeY), colorDepth(colorDepth), sharedBufferId(sharedBufferId) {}

bool CreateWindow::writeToStream(Util::Io::OutputStream &stream) const {
    return Util::Io::NumberUtil::writeUnsigned16BitValue(sizeX, stream) &&
        Util::Io::NumberUtil::writeUnsigned16BitValue(sizeY, stream) &&
        Util::Io::NumberUtil::writeUnsigned8BitValue(colorDepth, stream) &&
        Util::Io::NumberUtil::writeUnsigned32BitValue(sharedBufferId, stream);
}

bool CreateWindow::readFromStream(Util::Io::InputStream &stream) {
    sizeX = Util::Io::NumberUtil::readUnsigned16BitValue(stream);
    sizeY = Util::Io::NumberUtil::readUnsigned16BitValue(stream);
    colorDepth = Util::Io::NumberUtil::readUnsigned8BitValue(stream);
    sharedBufferId = Util::Io::NumberUtil::readUnsigned32BitValue(stream);

    return true;
}

uint16_t CreateWindow::getSizeX() const {
    return sizeX;
}

uint16_t CreateWindow::getSizeY() const {
    return sizeY;
}

uint8_t CreateWindow::getColorDepth() const {
    return colorDepth;
}

size_t CreateWindow::getSharedBufferId() const {
    return sharedBufferId;
}

}

}