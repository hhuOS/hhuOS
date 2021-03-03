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

#ifndef HHUOS_COLORGRAPHICSARRAY_H
#define HHUOS_COLORGRAPHICSARRAY_H

#include <util/graphic/Terminal.h>
#include <device/cpu/IoPort.h>
#include <util/graphic/Colors.h>

namespace Device::Graphic {

class ColorGraphicsArray : public Util::Graphic::Terminal {

public:

    ColorGraphicsArray(uint16_t columns, uint16_t rows);

    ColorGraphicsArray(const ColorGraphicsArray &copy) = delete;

    ColorGraphicsArray &operator=(const ColorGraphicsArray &other) = delete;

    ~ColorGraphicsArray() override = default;

    void putChar(char c) override;

    void clear() override;

    void setPosition(uint16_t column, uint16_t row) override;

    void setForegroundColor(Util::Graphic::Color &color) override;

    void setBackgroundColor(Util::Graphic::Color &color) override;

private:

    void updateCursorPosition();

    void scrollUp();

    Util::Graphic::Color fgColor = Util::Graphic::Colors::TERM_WHITE;
    Util::Graphic::Color bgColor = Util::Graphic::Colors::TERM_BLACK;
    uint16_t currentColumn = 0;
    uint16_t currentRow = 0;

    Util::Memory::Address<uint32_t> cgaMemory;
    IoPort indexPort;
    IoPort dataPort;

    static const constexpr uint8_t BYTES_PER_CHARACTER = 2;
    static const constexpr uint16_t INDEX_PORT_ADDRESS = 0x03d4;
    static const constexpr uint16_t DATA_PORT_ADDRESS = 0x03d5;
    static const constexpr uint16_t CURSOR_LOW_BYTE = 0x0f;
    static const constexpr uint16_t CURSOR_HIGH_BYTE = 0x0e;
};

}

#endif
