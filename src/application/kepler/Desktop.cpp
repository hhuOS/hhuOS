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

#include "Desktop.h"

#include "util/async/Process.h"
#include "util/graphic/BitmapFile.h"
#include "util/graphic/Colors.h"
#include "util/graphic/font/Terminal8x8.h"

const Util::Graphic::Font &Desktop::FONT = Util::Graphic::Fonts::TERMINAL_8x8;
const int32_t Desktop::DESKTOP_ENTRY_WIDTH = FONT.getCharWidth() * 8;
const int32_t Desktop::DESKTOP_ENTRY_HEIGHT = DesktopEntry::ICON_SIZE + FONT.getCharHeight() + DESKTOP_ENTRY_TEXT_SPACING;

Desktop::Desktop(const size_t width, const size_t height) :
    rows(height / (DESKTOP_ENTRY_HEIGHT + DESKTOP_ENTRY_SPACING)),
    columns (width / (DESKTOP_ENTRY_WIDTH + DESKTOP_ENTRY_SPACING))
{
    background = Util::Graphic::BitmapFile::open("/user/kepler/background.bmp");
    if (background != nullptr) {
        if (background->getWidth() > width || background->getHeight() > height) {
            background = background->scale(width, height);
        }
    }
}

Desktop::~Desktop() {
    delete background;

    for (const auto *entry : desktopEntries) {
        delete entry;
    }
}

void Desktop::draw(const Util::Graphic::LinearFrameBuffer &lfb) const {
    lfb.clear();

    if (background != nullptr) {
        lfb.drawImage(*background, (lfb.getResolutionX() - background->getWidth()) / 2,
                    (lfb.getResolutionY() - background->getHeight()) / 2);
    }

    int32_t x = 0;
    int32_t y = 0;
    for (const auto *entry : desktopEntries) {
        const int32_t posX = x * (DESKTOP_ENTRY_WIDTH + DESKTOP_ENTRY_TEXT_SPACING);
        const int32_t posY = y * (DESKTOP_ENTRY_HEIGHT + DESKTOP_ENTRY_SPACING);
        const auto title = entry->getName().substring(0, 8);
        const auto stringPosX = posX + (DESKTOP_ENTRY_WIDTH - FONT.getCharWidth() * title.length()) / 2;

        lfb.drawImage(entry->getIcon(), posX + (DESKTOP_ENTRY_WIDTH - DesktopEntry::ICON_SIZE) / 2, posY);
        lfb.drawString(FONT, stringPosX, posY + DesktopEntry::ICON_SIZE + DESKTOP_ENTRY_TEXT_SPACING, title, Util::Graphic::Colors::WHITE, Util::Graphic::Colors::INVISIBLE);

        if (++y >= rows) {
            if (++x >= columns) {
                break;
            }

            y = 0;
        }
    }
}

void Desktop::handleMouseClick(const uint16_t clickX, const uint16_t clickY) const {
    int32_t x = 0;
    int32_t y = 0;
    for (const auto *entry : desktopEntries) {
        const int32_t posX = x * (DESKTOP_ENTRY_WIDTH + DESKTOP_ENTRY_TEXT_SPACING);
        const int32_t posY = y * (DESKTOP_ENTRY_HEIGHT + DESKTOP_ENTRY_SPACING);

        if (clickX >= posX && clickX < posX + DESKTOP_ENTRY_WIDTH && clickY >= posY && clickY < posY + DESKTOP_ENTRY_HEIGHT) {
            const Util::Io::File executable(entry->getExecutable());
            if (executable.exists()) {
                Util::Async::Process::execute(executable, Util::Io::File("/device/null"),
                    Util::Io::File("/device/null"), Util::Io::File("/device/null"),
                    entry->getName(), entry->getArgs());
            }
        }

        if (++y >= rows) {
            if (++x >= columns) {
                break;
            }

            y = 0;
        }
    }
}
