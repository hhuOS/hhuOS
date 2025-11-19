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

#include "ClientWindow.h"

#include "util/graphic/Colors.h"

ClientWindow::ClientWindow(size_t id, Util::Async::SharedMemory *buffer, uint16_t posX, uint16_t posY, uint16_t width, uint16_t height, const Util::String &title) :
    id(id), buffer(buffer), posX(posX), posY(posY), width(width), height(height), title(title) {}

ClientWindow::~ClientWindow() {
    delete buffer;
}

size_t ClientWindow::getId() const {
    return id;
}

Util::Async::SharedMemory& ClientWindow::getBuffer() const {
    return *buffer;
}

uint16_t ClientWindow::getPosX() const {
    return posX;
}

uint16_t ClientWindow::getPosY() const {
    return posY;
}

uint16_t ClientWindow::getWidth() const {
    return width;
}

uint16_t ClientWindow::getHeight() const {
    return height;
}

Util::String ClientWindow::getTitle() const {
    return title;
}

bool ClientWindow::isDirty() const {
    return dirty;
}

void ClientWindow::setDirty(const bool dirty) {
    ClientWindow::dirty = dirty;
}

void ClientWindow::drawFrame(const Util::Graphic::LinearFrameBuffer &lfb) const {
    lfb.drawRectangle(posX, posY, width + 2, height + TITLE_FONT.getCharHeight() + 5,
        Util::Graphic::Colors::WHITE);
    lfb.fillRectangle(posX, posY, width + 2, TITLE_FONT.getCharHeight() + 5, Util::Graphic::Colors::WHITE);

    const auto titleWidth = static_cast<uint16_t>(title.length() * TITLE_FONT.getCharWidth());
    const auto titlePosX = posX + (width + 2 - titleWidth) / 2;
    lfb.drawString(TITLE_FONT, titlePosX, posY + 2, title,
        Util::Graphic::Colors::BLACK, Util::Graphic::Colors::WHITE);
}

void ClientWindow::flush(const Util::Graphic::LinearFrameBuffer &lfb) const {
    auto sourceAddress = buffer->getAddress();
    auto targetAddress = lfb.getBuffer().add(
        (posY + TITLE_FONT.getCharHeight() + 4) * lfb.getPitch() + (posX + 1) * lfb.getBytesPerPixel());

    for (uint16_t y = 0; y < height; y++) {
        targetAddress.copyRange(sourceAddress, width * lfb.getBytesPerPixel());
        targetAddress = targetAddress.add(lfb.getPitch());
        sourceAddress = sourceAddress.add(width * lfb.getBytesPerPixel());
    }
}
