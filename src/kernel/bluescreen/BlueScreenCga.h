/*
 * Copyright (C) 2019 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
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

#ifndef HHUOS_BLUESCREENCGA_H
#define HHUOS_BLUESCREENCGA_H

#include "BlueScreen.h"

namespace Kernel {

/**
 * Implementation of a bluescreen for the CGA graphics mode.
 *
 * CAUTION: This class will use a BIOS-call to change the graphics mode!
 *
 * @author Fabian Ruhland
 * @date 2019
 *
 */
class BlueScreenCga : public BlueScreen {

public:

    /**
     * Constructor.
     */
    BlueScreenCga();

    /**
     * Copy-constructor.
     */
    BlueScreenCga(const BlueScreenCga &other) = delete;

    /**
     * Assignment operator.
     */
    BlueScreenCga &operator=(const BlueScreenCga &other) = delete;

    /**
     * Overriding function from BlueScreen.
     */
    void initialize() override;

private:

    /**
     * Overriding function from BlueScreen.
     */
    void show(uint16_t x, uint16_t y, char c) override;

private:

    const uint8_t ROWS = 25;

    const uint8_t COLUMNS = 80;

    const uint8_t ATTRIBUTE = 0x1F;

    const char *CGA_START = reinterpret_cast<const char *>(VIRT_CGA_START);

};

}

#endif
