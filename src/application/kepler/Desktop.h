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

#ifndef HHUOS_DESKTOP_H
#define HHUOS_DESKTOP_H

#include "DesktopEntry.h"

#include <util/collection/ArrayList.h>
#include <util/graphic/Image.h>
#include <util/graphic/LinearFrameBuffer.h>

#include "util/graphic/font/Acorn8x8.h"

class Desktop {

public:

    Desktop(size_t width, size_t height);

    ~Desktop();

    void draw(const Util::Graphic::LinearFrameBuffer &lfb) const;

    void addEntry(const Util::String &name, const Util::String &executable, const Util::Array<Util::String> &args, const Util::String &iconPath) {
        desktopEntries.add(new DesktopEntry(name, executable, args, iconPath));
    }

    void handleMouseClick(uint16_t x, uint16_t y) const;

private:

    Util::Graphic::Image *background = nullptr;
    Util::ArrayList<DesktopEntry*> desktopEntries;

    const int32_t rows;
    const int32_t columns;

    static const Util::Graphic::Font &FONT;

    static const int32_t DESKTOP_ENTRY_WIDTH;
    static const int32_t DESKTOP_ENTRY_HEIGHT;
    static constexpr int32_t DESKTOP_ENTRY_TEXT_SPACING = 4;
    static constexpr int32_t DESKTOP_ENTRY_SPACING = 8;
};

#endif
