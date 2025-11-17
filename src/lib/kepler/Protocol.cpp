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

CreateWindow::CreateWindow(const uint16_t width, const uint16_t height, const Util::String &title) :
    width(width), height(height), title(title) {}

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

uint16_t CreateWindow::getWidth() const {
    return width;
}

uint16_t CreateWindow::getHeight() const {
    return height;
}

Util::String CreateWindow::getTitle() const {
    return title;
}

Flush::Flush(const size_t windowId) : windowId(windowId) {}

bool Flush::writeToStream(Util::Io::OutputStream &stream) const {
    return Util::Io::NumberUtil::writeUnsigned8BitValue(FLUSH, stream) &&
        Util::Io::NumberUtil::writeUnsigned32BitValue(windowId, stream);
}

bool Flush::readFromStream(Util::Io::InputStream &stream) {
    windowId = Util::Io::NumberUtil::readUnsigned32BitValue(stream);
    return true;
}

size_t Flush::getWindowId() const {
    return windowId;
}

}

namespace Response {

CreateWindow::CreateWindow(const size_t id, const uint16_t width, const uint16_t height, const uint8_t colorDepth) :
    id(id), width(width), height(height), colorDepth(colorDepth) {}

bool CreateWindow::writeToStream(Util::Io::OutputStream &stream) const {
    return Util::Io::NumberUtil::writeUnsigned32BitValue(id, stream) &&
        Util::Io::NumberUtil::writeUnsigned16BitValue(width, stream) &&
        Util::Io::NumberUtil::writeUnsigned16BitValue(height, stream) &&
        Util::Io::NumberUtil::writeUnsigned8BitValue(colorDepth, stream);
}

bool CreateWindow::readFromStream(Util::Io::InputStream &stream) {
    id = Util::Io::NumberUtil::readUnsigned32BitValue(stream);
    width = Util::Io::NumberUtil::readUnsigned16BitValue(stream);
    height = Util::Io::NumberUtil::readUnsigned16BitValue(stream);
    colorDepth = Util::Io::NumberUtil::readUnsigned8BitValue(stream);

    return true;
}

size_t CreateWindow::getId() const {
    return id;
}

uint16_t CreateWindow::getSizeX() const {
    return width;
}

uint16_t CreateWindow::getSizeY() const {
    return height;
}

uint8_t CreateWindow::getColorDepth() const {
    return colorDepth;
}

Flush::Flush(const bool success) : success(success) {}

bool Flush::writeToStream(Util::Io::OutputStream &stream) const {
    return Util::Io::NumberUtil::writeUnsigned8BitValue(success ? 1 : 0, stream);
}

bool Flush::readFromStream(Util::Io::InputStream &stream) {
    success = Util::Io::NumberUtil::readUnsigned8BitValue(stream) != 0;
    return true;
}

bool Flush::isSuccess() const {
    return success;
}

}

}