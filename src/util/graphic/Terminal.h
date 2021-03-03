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

#ifndef HHUOS_TERMINAL_H
#define HHUOS_TERMINAL_H

#include <cstdint>
#include "Color.h"

namespace Util::Graphic {

class Terminal {

public:

    Terminal(uint16_t columns, uint16_t rows);

    Terminal(const Terminal &copy) = delete;

    Terminal &operator=(const Terminal &other) = delete;

    virtual ~Terminal() = default;

    virtual void putChar(char c) = 0;

    virtual void putString(const char *string);

    virtual void clear() = 0;

    virtual void setPosition(uint16_t column, uint16_t row) = 0;

    virtual void setForegroundColor(Color &color) = 0;

    virtual void setBackgroundColor(Color &color) = 0;

    [[nodiscard]] uint16_t getColumns() const;

    [[nodiscard]] uint16_t getRows() const;

private:

    const uint16_t columns;
    const uint16_t rows;

};

}

#endif
