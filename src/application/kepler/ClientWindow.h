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

#include "util/graphic/LinearFrameBuffer.h"
#include "util/graphic/font/Terminal8x8.h"
#include "util/io/stream/FileOutputStream.h"
#include "util/async/SharedMemory.h"
#include "kepler/Protocol.h"

class ClientWindow {

public:

    struct MouseCoordinates {
        uint16_t x = 0;
        uint16_t y = 0;
        bool valid = false;
    };

    ClientWindow(size_t id, size_t processId, uint16_t posX, uint16_t posY, uint16_t width, uint16_t height, const Util::String &title, Util::Async::SharedMemory *buffer);

    ~ClientWindow();

    [[nodiscard]] size_t getId() const;

    [[nodiscard]] Util::Async::SharedMemory& getBuffer() const;

    [[nodiscard]] uint16_t getPosX() const;

    [[nodiscard]] uint16_t getPosY() const;

    [[nodiscard]] uint16_t getWidth() const;

    [[nodiscard]] uint16_t getHeight() const;

    [[nodiscard]] Util::String getTitle() const;

    [[nodiscard]] bool isDirty() const;

    void setDirty(bool dirty);

    MouseCoordinates containsPoint(uint16_t x, uint16_t y) const;

    void sendMouseHoverEvent(const Kepler::Event::MouseHover &event);

    void sendMouseClickEvent(const Kepler::Event::MouseClick &event);

    void drawFrame(const Util::Graphic::LinearFrameBuffer &lfb, const Util::Graphic::Color &color) const;

    void flush(const Util::Graphic::LinearFrameBuffer &lfb) const;

private:

    size_t id;

    uint16_t posX = 0;
    uint16_t posY = 0;
    uint16_t width = 0;
    uint16_t height = 0;

    Util::String title;

    Util::Async::SharedMemory *buffer = nullptr;
    Util::Io::FileOutputStream mouseOutputStream;

    bool dirty = true;

    static const Util::Graphic::Font &TITLE_FONT;
};

#endif