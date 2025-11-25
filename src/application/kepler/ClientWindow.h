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

#ifndef HHUOS_CLIENTWINDOW_H
#define HHUOS_CLIENTWINDOW_H

#include <stddef.h>

#include "util/graphic/LinearFrameBuffer.h"
#include "util/graphic/font/Terminal8x8.h"
#include "util/io/stream/FileInputStream.h"
#include "util/async/SharedMemory.h"

class ClientWindow {

public:

    ClientWindow(size_t id, Util::Async::SharedMemory *buffer, uint16_t posX, uint16_t posY, uint16_t width, uint16_t height, const Util::String &title);

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

    void drawFrame(const Util::Graphic::LinearFrameBuffer &lfb, const Util::Graphic::Color &color) const;

    void flush(const Util::Graphic::LinearFrameBuffer &lfb) const;

private:

    size_t id;
    Util::Async::SharedMemory *buffer = nullptr;
    uint16_t posX = 0;
    uint16_t posY = 0;
    uint16_t width = 0;
    uint16_t height = 0;
    Util::String title;

    bool dirty = true;

    static constexpr Util::Graphic::Font &TITLE_FONT = Util::Graphic::Fonts::TERMINAL_8x8;
};

#endif