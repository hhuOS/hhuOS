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

#include "ClientWindow.h"

#include "util/graphic/Colors.h"

const Util::Graphic::Font &ClientWindow::TITLE_FONT = Util::Graphic::Fonts::TERMINAL_8x8;

ClientWindow::ClientWindow(size_t id, size_t processId, uint16_t posX, uint16_t posY, uint16_t width, uint16_t height, const Util::String &title, Util::Async::SharedMemory *buffer) :
    id(id), posX(posX), posY(posY), width(width), height(height), title(title), buffer(buffer), mouseOutputStream(Util::String::format("/process/%u/pipes/mouse-%u", processId, id)) {}

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

ClientWindow::MouseCoordinates ClientWindow::containsPoint(const uint16_t x, const uint16_t y) const {
    const auto mouseX = x - (posX + 1);
    const auto mouseY = y - (posY + TITLE_FONT.getCharHeight() + 5);

    if (mouseX >= 0 && mouseX < width && mouseY >= 0 && mouseY < height) {
        return MouseCoordinates{static_cast<uint16_t>(mouseX), static_cast<uint16_t>(mouseY), true};
    }

    return MouseCoordinates{0, 0, false};
}

void ClientWindow::sendMouseHoverEvent(const Kepler::Event::MouseHover &event) {
    event.writeToStream(mouseOutputStream);
}

void ClientWindow::sendMouseClickEvent(const Kepler::Event::MouseClick &event) {
    event.writeToStream(mouseOutputStream);
}

void ClientWindow::drawFrame(const Util::Graphic::LinearFrameBuffer &lfb, const Util::Graphic::Color &color) const {
    lfb.drawRectangle(posX, posY, width + 2, height + TITLE_FONT.getCharHeight() + 5, color);
    lfb.fillRectangle(posX, posY, width + 2, TITLE_FONT.getCharHeight() + 5, color);

    const auto titleWidth = static_cast<uint16_t>(title.length() * TITLE_FONT.getCharWidth());
    const auto titlePosX = posX + (width + 2 - titleWidth) / 2;
    lfb.drawString(TITLE_FONT, titlePosX, posY + 2, static_cast<const char*>(title),
        Util::Graphic::Colors::BLACK, color);
}

void ClientWindow::flush(const Util::Graphic::LinearFrameBuffer &lfb) const {
    const auto bytesPerPixel = (lfb.getColorDepth() + 7) / 8;
    auto sourceAddress = buffer->getAddress();
    auto targetAddress = lfb.getBuffer().add(
        (posY + TITLE_FONT.getCharHeight() + 4) * lfb.getPitch() + (posX + 1) * bytesPerPixel);

    for (uint16_t y = 0; y < height; y++) {
        targetAddress.copyRange(sourceAddress, width * bytesPerPixel);
        targetAddress = targetAddress.add(lfb.getPitch());
        sourceAddress = sourceAddress.add(width * bytesPerPixel);
    }
}
