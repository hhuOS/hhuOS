/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef __Color_include__
#define __Color_include__

#include <cstdint>

namespace Util::Graphic {

/**
 * Convert RGB-colors into their 1-, 2-, 4-, 8-, 15-, 16-, 24-, and 32-Bit representations.
 * Provides the possibility to blend to transparent colors.
 *
 * 32-Bit:
 *  Alpha     Red     Green     Blue
 * XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX
 *
 * 24-Bit:
 *   Red     Green     Blue
 * XXXXXXXX XXXXXXXX XXXXXXXX
 *
 * 16-Bit:
 *  Red  Green  Blue
 * XXXXX XXXXXX XXXXX
 *
 * 15-Bit:
 *  Red  Green Blue
 * XXXXX XXXXX XXXXX
 *
 * 8-Bit:
 * Red Green Blue
 * XX  XXX   XXX
 *
 * 4-Bit:
 * Brightness Red Green Blue
 *     X       X    X     X
 *
 * 2-Bit:
 * Blue/Green Red
 *     X       X
 *
 * 1-Bit:
 * Black/White
 *      X
 *
 *
 * Once an RGB-value has been calculated, it will be cached, e.g. the first call to RGB32() will calculate and the
 * 32-bit value and store it in a variable. Every subsequent call to RGB32() will just return the cached value (if the
 * color attributes have not been changed since the last call).
 */
class Color {

public:
    /**
     * Default-Constructor. Initializes the color as black.
     */
    Color();

    /**
     * RGB-constuctor.
     *
     * @param red The value for red
     * @param green The value green
     * @param blue The value for blue
     */
    Color(uint8_t red, uint8_t green, uint8_t blue);

    /**
     * RGBA-Constructor.
     *
     * @param red The value for red
     * @param green The value green
     * @param blue The value for blue
     * @param alpha The value fpr alpha
     */
    Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);

    /**
     * Copy Constructor.
     */
    Color(const Color &other) = default;

    /**
     * Assignment operator.
     */
    Color &operator=(const Color &other) = default;

    /**
     * Destructor.
     */
    ~Color() = default;

    bool operator==(const Color &other) const;

    [[nodiscard]] static Color fromRGB(uint32_t rgb, uint8_t depth);

    [[nodiscard]] static Color fromRGB32(uint32_t rgb);

    [[nodiscard]] static Color fromRGB24(uint32_t rgb);

    [[nodiscard]] static Color fromRGB16(uint16_t rgb);

    [[nodiscard]] static Color fromRGB15(uint16_t rgb);

    [[nodiscard]] static Color fromRGB8(uint8_t rgb);

    [[nodiscard]] static Color fromRGB4(uint8_t rgb);

    [[nodiscard]] static Color fromRGB2(uint8_t rgb);

    [[nodiscard]] static Color fromRGB1(uint8_t rgb);

    /**
     * Get the red-value.
     */
    [[nodiscard]] uint8_t getRed() const;

    /**
     * Get the green-value.
     */
    [[nodiscard]] uint8_t getGreen() const;

    /**
     * Get the blue-value.
     */
    [[nodiscard]] uint8_t getBlue() const;

    /**
     * Get the alpha-value.
     */
    [[nodiscard]] uint8_t getAlpha() const;

    /**
     * Get the RGB32-value.
     */
    [[nodiscard]] uint32_t getRGB32() const;

    /**
     * Get the RGB24-value.
     */
    [[nodiscard]] uint32_t getRGB24() const;

    /**
     * Get the RGB16-value.
     */
    [[nodiscard]] uint16_t getRGB16() const;

    /**
     * Get the RGB15-value.
     */
    [[nodiscard]] uint16_t getRGB15() const;

    /**
     * Get the RGB8-value.
     */
    [[nodiscard]] uint8_t getRGB8() const;

    /**
     * Get the RB4-value.
     */
    [[nodiscard]] uint8_t getRGB4() const;

    /**
     * Get the RGB2-value.
     */
    [[nodiscard]] uint8_t getRGB2() const;

    /**
     * Get the RGB1-value.
     */
    [[nodiscard]] uint8_t getRGB1() const;

    /**
     * Get the RGB-value for a given color-colorDepth.
     *
     * @param depth The color-colorDepth
     */
    [[nodiscard]] uint32_t getColorForDepth(uint8_t depth) const;

    /**
     * Get a brighter version of the color.
     */
    [[nodiscard]] Color bright() const;

    /**
     * Get a dimmer version of the color
     */
    [[nodiscard]] Color dim() const;

    /**
     * Blend this color with another color.
     *
     * @param color The color to blend with.
     */
    [[nodiscard]] Color blend(const Color &color) const;

private:

    uint8_t red, green, blue, alpha;

    static const constexpr uint8_t BRIGHTNESS_SHIFT = 85;

};

}

#endif