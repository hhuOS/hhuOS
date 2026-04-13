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

#ifndef HHUOS_CLIENTWINDOW_H
#define HHUOS_CLIENTWINDOW_H

#include <stddef.h>

#include "MouseInputHandler.h"
#include "WindowStack.h"
#include "util/graphic/LinearFrameBuffer.h"
#include "util/graphic/font/Terminal8x8.h"
#include "util/io/stream/FileOutputStream.h"
#include "util/async/SharedMemory.h"
#include "kepler/Protocol.h"

class ClientWindow {

public:

    enum WindowArea {
        CONTENT,
        TITLE_BAR,
        BORDER,
        NONE
    };

    struct MouseEvent {
        WindowArea area = NONE;
        int32_t contentPosX = 0;
        int32_t contentPosY = 0;
    };

    ClientWindow(size_t id, size_t processId, uint16_t posX, uint16_t posY, uint16_t width, uint16_t height, const Util::String &title, Util::Async::SharedMemory *buffer);

    ~ClientWindow();

    [[nodiscard]] size_t getId() const;

    [[nodiscard]] Util::Async::SharedMemory& getBuffer() const;

    [[nodiscard]] int32_t getPosX() const;

    void setPosX(const int32_t posX) {
        ClientWindow::posX = posX;
    }

    [[nodiscard]] int32_t getPosY() const;

    void setPosY(const int32_t posY) {
        ClientWindow::posY = posY;
    }

    [[nodiscard]] uint16_t getWidth() const;

    [[nodiscard]] uint16_t getHeight() const;

    [[nodiscard]] Util::String getTitle() const;

    [[nodiscard]] bool isDirty() const;

    void setDirty(bool dirty);

    MouseEvent containsPoint(uint16_t x, uint16_t y) const;

    bool overlapsWith(const ClientWindow &other) const;

    void sendMouseHoverEvent(const Kepler::Event::MouseHover &event);

    void sendMouseClickEvent(const Kepler::Event::MouseClick &event);

    void sendKeyEvent(const Kepler::Event::KeyEvent &event);

    void drawBorder(const Util::Graphic::LinearFrameBuffer &lfb, const Util::Graphic::Color &color) const;

    void drawBorderAt(int32_t x, int32_t y, const Util::Graphic::LinearFrameBuffer &lfb, const Util::Graphic::Color &color) const;

    void drawTitleBar(const Util::Graphic::LinearFrameBuffer &lfb, const Util::Graphic::Color &color) const;

    void flush(const Util::Graphic::LinearFrameBuffer &lfb) const;

private:

    size_t id;

    int32_t posX = 0;
    int32_t posY = 0;
    uint16_t width = 0;
    uint16_t height = 0;

    Util::String title;

    Util::Async::SharedMemory *buffer = nullptr;
    Util::Io::FileOutputStream mouseOutputStream;

    bool dirty = true;

    uint16_t titleOffsetX;

    static const Util::Graphic::Font &TITLE_FONT;
    static const uint16_t TITLE_BAR_HEIGHT;
    static const uint16_t TITLE_OFFSET_Y;
    static constexpr uint16_t BORDER_WIDTH = 1;
};

#endif