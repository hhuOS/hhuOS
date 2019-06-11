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

#ifndef HHUOS_BLUESCREENLFBTEXT_H
#define HHUOS_BLUESCREENLFBTEXT_H

#include "device/graphic/text/LfbText.h"
#include "kernel/multiboot/Structure.h"
#include "BlueScreen.h"

namespace Kernel {

/**
 * Implementation of a bluescreen for a lfb-compatible graphics mode.
 *
 * @author Fabian Ruhland
 * @date 2019
 */
class BlueScreenLfb : public BlueScreen {

public:

    static Multiboot::FrameBufferInfo fbInfo;

public:

    /**
     * Constructor.
     */
    BlueScreenLfb();

    /**
     * Copy-constructor.
     */
    BlueScreenLfb(const BlueScreenLfb &copy) = delete;

    /**
     * Assignment operator.
     */
    BlueScreenLfb &operator=(const BlueScreenLfb &other) = delete;

    /**
     * Destructor.
     */
    ~BlueScreenLfb() override = default;

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

    LinearFrameBuffer lfb;

    Font &font = std_font_8x16;

    uint8_t charWidth = font.get_char_width();
    uint8_t charHeight = font.get_char_height();

};

}

#endif
