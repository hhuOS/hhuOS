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

#include "kepler/Window.h"
#include "util/graphic/Colors.h"

const Util::Graphic::Font &ClientWindow::TITLE_FONT = Util::Graphic::Fonts::TERMINAL_8x8;
const uint16_t ClientWindow::TITLE_BAR_HEIGHT = TITLE_FONT.getCharHeight() + 5;
const uint16_t ClientWindow::TITLE_OFFSET_Y = (TITLE_BAR_HEIGHT - TITLE_FONT.getCharHeight()) / 2;

ClientWindow::ClientWindow(const size_t id, const size_t processId, const uint16_t posX, const uint16_t posY,
    const uint16_t width, const uint16_t height, const Util::String &title, Util::Async::SharedMemory *buffer) :
    id(id), posX(posX), posY(posY), width(width), height(height), title(title), buffer(buffer),
    mouseOutputStream(Util::String::format(Kepler::Window::EVENT_PIPE_PATH, processId, id)),
    titleOffsetX((width + 2 * BORDER_WIDTH - title.length() * TITLE_FONT.getCharWidth()) / 2) {}

ClientWindow::~ClientWindow() {
    delete buffer;
}

size_t ClientWindow::getId() const {
    return id;
}

Util::Async::SharedMemory& ClientWindow::getBuffer() const {
    return *buffer;
}

int32_t ClientWindow::getPosX() const {
    return posX;
}

int32_t ClientWindow::getPosY() const {
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

ClientWindow::MouseEvent ClientWindow::containsPoint(const uint16_t x, const uint16_t y) const {
    const auto mouseX = x - posX;
    const auto mouseY = y - posY;

    // Check if mouse is inside title bar
    if (mouseX >= BORDER_WIDTH && mouseX < width && mouseY >= BORDER_WIDTH && mouseY < TITLE_BAR_HEIGHT + BORDER_WIDTH) {
        return MouseEvent{TITLE_BAR, 0, 0};
    }

    // Check if mouse inside the border
    if ((mouseX >= 0 && mouseX < BORDER_WIDTH && mouseY >= 0 && mouseY < width + BORDER_WIDTH * 2) || // Left border
        (mouseX >= width + BORDER_WIDTH && mouseX < width + 2 * BORDER_WIDTH && mouseY >= 0 && mouseY < width + BORDER_WIDTH * 2) || // Right border
        (mouseY >= 0 && mouseY < BORDER_WIDTH && mouseX >= 0 && mouseX < height + 2 * BORDER_WIDTH) || // Upper border
        (mouseY >= height + TITLE_BAR_HEIGHT + BORDER_WIDTH && mouseY < height + TITLE_BAR_HEIGHT + 2 * BORDER_WIDTH && mouseX >= 0 && mouseX < height + 2 * BORDER_WIDTH)) // Lower Border
    {
        return MouseEvent{BORDER, 0, 0};
    }

    // Check if mouse inside the content area
    if (mouseX >= BORDER_WIDTH && mouseX < width + BORDER_WIDTH &&
        mouseY >= TITLE_BAR_HEIGHT + BORDER_WIDTH && mouseY < height + TITLE_BAR_HEIGHT + BORDER_WIDTH)
    {
        return MouseEvent{CONTENT, mouseX - BORDER_WIDTH, mouseY - TITLE_BAR_HEIGHT - BORDER_WIDTH};
    }

    return MouseEvent{NONE, 0, 0};
}

bool ClientWindow::overlapsWith(const ClientWindow &other) const {
    const auto thisRight = posX + width + 2 * BORDER_WIDTH;
    const auto thisBottom = posY + height + TITLE_BAR_HEIGHT + 2 * BORDER_WIDTH;
    const auto otherRight = other.posX + other.width + 2 * BORDER_WIDTH;
    const auto otherBottom = other.posY + other.height + TITLE_BAR_HEIGHT + 2 * BORDER_WIDTH;

    return posX < otherRight && thisRight > other.posX && posY < otherBottom && thisBottom > other.posY;
}

void ClientWindow::sendMouseHoverEvent(const Kepler::Event::MouseHover &event) {
    event.writeToStream(mouseOutputStream);
}

void ClientWindow::sendMouseClickEvent(const Kepler::Event::MouseClick &event) {
    event.writeToStream(mouseOutputStream);
}

void ClientWindow::sendKeyEvent(const Kepler::Event::KeyEvent &event) {
    event.writeToStream(mouseOutputStream);
}

void ClientWindow::drawBorder(const Util::Graphic::LinearFrameBuffer &lfb, const Util::Graphic::Color &color) const {
    drawBorderAt(posX, posY, lfb, color);
}

void ClientWindow::drawBorderAt(const int32_t x, const int32_t y, const Util::Graphic::LinearFrameBuffer &lfb,
    const Util::Graphic::Color &color) const
{
    lfb.drawRectangle(x, y, width + 2 * BORDER_WIDTH, height + TITLE_BAR_HEIGHT + 2 * BORDER_WIDTH, color);
}

void ClientWindow::drawTitleBar(const Util::Graphic::LinearFrameBuffer &lfb, const Util::Graphic::Color &color) const {
    lfb.fillRectangle(posX + BORDER_WIDTH, posY + BORDER_WIDTH, width, TITLE_BAR_HEIGHT, color);

    const auto titlePosX = posX + titleOffsetX;
    const auto titlePosY = posY + TITLE_OFFSET_Y;
    lfb.drawString(TITLE_FONT, titlePosX, titlePosY, static_cast<const char*>(title),
                   Util::Graphic::Colors::BLACK, color);
}

void ClientWindow::flush(const Util::Graphic::LinearFrameBuffer &lfb) const {
    const auto bytesPerPixel = (lfb.getColorDepth() + 7) / 8;

    auto sourceAddress = buffer->getAddress();
    auto targetAddress = lfb.getBuffer().add((posY + TITLE_BAR_HEIGHT + BORDER_WIDTH) * lfb.getPitch()
        + (posX + BORDER_WIDTH) * bytesPerPixel);

    auto flushWidth = width;
    auto flushHeight = height;

    // Content area offsets
    const auto xOffset = posX + BORDER_WIDTH;
    const auto yOffset = posY + TITLE_BAR_HEIGHT + BORDER_WIDTH;

    if (posX - BORDER_WIDTH < 0) {
        // Window is partially outside the screen on the left side
        flushWidth = width + xOffset;
        sourceAddress = sourceAddress.add(-xOffset * bytesPerPixel);
        targetAddress = targetAddress.add(-xOffset * bytesPerPixel);
    } else if (xOffset + width > lfb.getResolutionX()) {
        // Window is partially outside the screen on the right side
        flushWidth = lfb.getResolutionX() - posX - BORDER_WIDTH;
    }

    if (yOffset < 0) {
        // Window is partially out of sight on the top
        flushHeight = height + yOffset;
        sourceAddress = sourceAddress.add(-yOffset * width * bytesPerPixel);
        targetAddress = targetAddress.add(-yOffset * lfb.getPitch());
    } else if (yOffset + height > lfb.getResolutionY()) {
        // Window is partially out of sight on the bottom
        flushHeight = lfb.getResolutionY() - posY - TITLE_BAR_HEIGHT - BORDER_WIDTH;
    }

    for (uint16_t y = 0; y < flushHeight; y++) {
        targetAddress.copyRange(sourceAddress, flushWidth * bytesPerPixel);
        targetAddress = targetAddress.add(lfb.getPitch());
        sourceAddress = sourceAddress.add(width * bytesPerPixel);
    }
}
