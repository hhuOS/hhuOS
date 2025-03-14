/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_COLORGRAPHICSADAPTER_H
#define HHUOS_COLORGRAPHICSADAPTER_H

#include <stdint.h>

#include "lib/util/graphic/Terminal.h"
#include "device/cpu/IoPort.h"
#include "lib/util/base/Address.h"

namespace Util {
namespace Graphic {
class Color;
}  // namespace Graphic
}  // namespace Util

namespace Device::Graphic {

class ColorGraphicsAdapter : public Util::Graphic::Terminal {

public:

    enum VideoCardType : uint8_t {
        NO_DISPLAY = 0x00,
        MONOCHROME = 0x01,
        CGA_COLOR = 0x02,
        EGA_COLOR = 0x04,
        EGA_MONOCHROME = 0x05,
        PGA_COLOR = 0x06,
        VGA_MONOCHROME = 0x07,
        VGA_COLOR = 0x08,
        MCGA_COLOR_DIGITAL = 0x0a,
        MCGA_MONOCHROME = 0x0b,
        MCGA_COLOR = 0x0c,
        UNKNOWN = 0xff
    };

    enum Mode : uint8_t {
        TEXT_40_COLUMNS = 0x01,
        TEXT_80_COLUMNS = 0x03,
    };

    ColorGraphicsAdapter(uint16_t columns, uint16_t rows);

    ColorGraphicsAdapter(const ColorGraphicsAdapter &copy) = delete;

    ColorGraphicsAdapter &operator=(const ColorGraphicsAdapter &other) = delete;

    ~ColorGraphicsAdapter() override;

    [[nodiscard]] static VideoCardType getVideoCardType();

    static void setMode(Mode mode);

    void putChar(char c, const Util::Graphic::Color &foregroundColor, const Util::Graphic::Color &backgroundColor) override;

    void clear(const Util::Graphic::Color &foregroundColor, const Util::Graphic::Color &backgroundColor, uint16_t startColumn, uint32_t startRow, uint16_t endColumn, uint16_t endRow) override;

    void setPosition(uint16_t column, uint16_t row) override;

    void setCursor(bool enabled) override;

    [[nodiscard]] uint16_t getCurrentColumn() const override;

    [[nodiscard]] uint16_t getCurrentRow() const override;

    [[nodiscard]] Util::Address getAddress();

private:

    void updateCursorPosition();

    void scrollUp();

    static Util::Address mapBuffer(void *physicalAddress, uint16_t columns, uint16_t rows);

    uint16_t currentColumn = 0;
    uint16_t currentRow = 0;

    Util::Address cgaMemory;
    IoPort indexPort = IoPort(0x3d4);
    IoPort dataPort = IoPort(0x3d5);

    static const constexpr uint8_t BYTES_PER_CHARACTER = 2;
    static const constexpr uint16_t CURSOR_LOW_BYTE = 0x0f;
    static const constexpr uint16_t CURSOR_HIGH_BYTE = 0x0e;
    static const constexpr uint16_t CURSOR_START_INDEX = 0x0a;
    static const constexpr uint16_t CURSOR_END_INDEX = 0x0b;
    static const constexpr uint32_t CGA_START_ADDRESS = 0x000b8000;
    static const constexpr uint16_t BIOS_FUNCTION_CHECK_VIDEO_CARD = 0x1a00;
};

}

#endif