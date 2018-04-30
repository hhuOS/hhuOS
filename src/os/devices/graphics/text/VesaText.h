/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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

#ifndef __VesaText_include__
#define __VesaText_include__

#include "TextDriver.h"
#include "devices/graphics/lfb/VesaGraphics.h"
#include "kernel/Kernel.h"
#include "lib/Colors.h"
#include "devices/graphics/text/fonts/Fonts.h"

#include <cstdint>

/**
 * Implementation of TextDriver for graphics cards, that are compatible with the VBE-standard.
 * Compatible devices need to support at least VBE 2.0.
 *
 * This implementation works on a pixel-based frame-buffer.
 */
class VesaText : public TextDriver, private VesaGraphics {

private:
    uint16_t pos = 0;

    Font *font = nullptr;

    uint8_t *shadowBuf = nullptr;

    Util::ArrayList<TextResolution> resolutions;

    /**
     * Overriding virtual function from VesaGraphics.
     */
    void drawPixel(uint16_t x, uint16_t y, Color color) override;

    /**
     * Overriding virtual function from TextDriver.
     */
    bool setResolution(TextResolution resolution) override;
    
public:
    /**
     * Constructor.
     */
    VesaText();

    /**
     * Copy-constructor.
     */
    VesaText(VesaText &copy) = delete;

    /**
     * Destructor.
     */
    ~VesaText() override = default;

    /**
     * Overriding virtual function from TextDriver.
     */
    bool isAvailable() override;

    /**
     * Overriding virtual function from TextDriver.
     */
    Util::Array<TextResolution> getTextResolutions() override;

    /**
     * Overriding virtual function from TextDriver.
     */
    String getVendorName() override;

    /**
     * Overriding virtual function from TextDriver.
     */
    String getDeviceName() override;

    /**
     * Overriding virtual function from TextDriver.
     */
    uint32_t getVideoMemorySize() override;

    /**
     * Overriding virtual function from TextDriver.
     */
    void setpos (uint16_t x, uint16_t y) override;

    /**
     * Overriding virtual function from TextDriver.
     */
    void getpos (uint16_t& x, uint16_t& y) override;

    /**
     * Overriding virtual function from TextDriver.
     */
    void show(uint16_t x, uint16_t y, char c, Color fgColor = Colors::LIGHT_GRAY, Color bgColor = Colors::INVISIBLE) override;

    /**
     * Overriding virtual function from TextDriver.
     */
    void putc(char c, Color fgColor = Colors::LIGHT_GRAY, Color bgColor = Colors::INVISIBLE) override;

    /**
     * Overriding virtual function from TextDriver.
     */
    void puts(const char *s, uint32_t n, Color fgColor = Colors::LIGHT_GRAY, Color bgColor = Colors::INVISIBLE) override;

    /**
     * Overriding virtual function from TextDriver.
     */
    void scrollup() override;

    /**
     * Overriding virtual function from TextDriver.
     */
    void clear() override;
};

#endif