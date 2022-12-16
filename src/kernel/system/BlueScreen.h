/*
 * Copyright (C) 2018-2021 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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

#ifndef HHUOS_BLUESCREEN_H
#define HHUOS_BLUESCREEN_H

#include <cstdint>

namespace Util {
namespace Graphic {
class PixelDrawer;
class StringDrawer;
}  // namespace Graphic
}  // namespace Util

namespace Kernel {
struct InterruptFrame;

class BlueScreen {

public:
    /**
     * Default Constructor.
     */
    BlueScreen() = default;

    /**
     * Copy Constructor.
     */
    BlueScreen(const BlueScreen &other) = delete;

    /**
     * Assignment operator.
     */
    BlueScreen &operator=(const BlueScreen &other) = delete;

    /**
     * Destructor.
     */
    ~BlueScreen() = default;

    static void setLfbMode(uint32_t address, uint16_t resolutionX, uint16_t resolutionY, uint8_t colorDepth, uint16_t pitch);

    static void setCgaMode(uint32_t address, uint16_t columns, uint16_t rows);

    static void show(const InterruptFrame &frame);

private:

    static void clear(Util::Graphic::PixelDrawer &pixelDrawer);

    static void print(Util::Graphic::StringDrawer &stringDrawer, const char *string);

    static void printLine(Util::Graphic::StringDrawer &stringDrawer, const char *string);

    static void printDecNumber(Util::Graphic::StringDrawer &stringDrawer, uint32_t number);

    static void printHexNumber(Util::Graphic::StringDrawer &stringDrawer, uint32_t number);

    static void putCharCga(char c);

    enum Mode : uint8_t {
        LFB, CGA
    };

    static Mode mode;

    static uint32_t fbAddress;
    static uint16_t fbResX;
    static uint16_t fbResY;
    static uint16_t fbColorDepth;
    static uint16_t fbPitch;

    static uint16_t posX;
    static uint16_t posY;
    static uint32_t maxStacktraceSize;

    static const constexpr uint16_t OFFSET_X = 4;
    static const constexpr uint16_t OFFSET_Y = 2;
};

}

#endif