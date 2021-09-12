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

#ifndef HHUOS_TERMINALOUTPUTSTREAM_H
#define HHUOS_TERMINALOUTPUTSTREAM_H

#include <lib/util/graphic/Terminal.h>
#include <lib/util/graphic/Colors.h>
#include "OutputStream.h"

namespace Util::Stream  {

class TerminalOutputStream : public OutputStream {

public:

    explicit TerminalOutputStream(Graphic::Terminal &terminal);

    TerminalOutputStream(const TerminalOutputStream &copy) = delete;

    TerminalOutputStream &operator=(const TerminalOutputStream &copy) = delete;

    ~TerminalOutputStream() override = default;

    void write(uint8_t c) override;

    void write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) override;

private:

    [[nodiscard]] int32_t extractNextAnsiCode(uint32_t &index) const;

    [[nodiscard]] Graphic::Color getColor(uint8_t colorCode, const Util::Graphic::Color &defaultColor, uint32_t &index);

    [[nodiscard]] Graphic::Color parseComplexColor(uint32_t &index);

    [[nodiscard]] Graphic::Color parse256Color(uint32_t &index);

    [[nodiscard]] Graphic::Color parseTrueColor(uint32_t &index);

    void parseGraphicRendition(uint8_t code);

    Graphic::Terminal &terminal;

    char currentEscapeCode[64]{};
    uint8_t escapeCodeIndex = 0;
    bool isEscapeActive = false;

    Util::Graphic::Color foregroundColor = Util::Graphic::Colors::WHITE;
    Util::Graphic::Color backgroundColor = Util::Graphic::Colors::BLACK;
    bool brightForeground = false;
    bool brightBackground = false;

    bool invert = false;
    bool bright = false;
    bool dim = false;

};

}

#endif
