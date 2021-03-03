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
     * Default-constructor. Initializes the color as black.
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
     * RGBA-constructor.
     *
     * @param red The value for red
     * @param green The value green
     * @param blue The value for blue
     * @param alpha The value fpr alpha
     */
    Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);

    /**
     * Constructor, which takes a composed RGB-value and the corresponding color-value
     *
     * @param rgb The RGB-value
     * @param depth The color-colorDepth
     */
    Color(uint32_t rgb, uint8_t depth);

    /**
     * Destructor.
     */
    ~Color() = default;

    /**
     * Set the red value.
     */
    void setRed(uint8_t red);

    /**
     * Set the green-value.
     */
    void setGreen(uint8_t green);

    /**
     * Set the blue-value.
     */
    void setBlue(uint8_t blue);

    /**
     * Set the alpha-value.
     */
    void setAlpha(uint8_t alpha);

    /**
     * Set red, green and blue at once.
     */
    void setRGB(uint8_t red, uint8_t green, uint8_t blue);

    /**
     * Set red, green, blue and alpha at once.
     */
    void setRGBA(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);

    /**
     * Decode a composed RGB-value and set red, green, blue and alpha to the decoded values.
     *
     * @param rgb The composed rgb-value
     * @param depth The color-colorDepth
     */
    void setRGB(uint32_t rgb, uint8_t depth);

    /**
     * Decode a composed RGB32-value and set red, green, blue and alpha to the decoded values.
     *
     * @param rgba The composed RGB-value
     */
    void setRGB32(uint32_t rgba);

    /**
     * Decode a composed RGB24-value and set red, green, blue to the decoded values.
     *
     * @param rgb The composed RGB-value
     */
    void setRGB24(uint32_t rgb);

    /**
     * Decode a composed RGB24-value and set red, green, blue to the decoded values.
     *
     * @param rgb The composed RGB-value
     */
    void setRGB16(uint16_t rgb);

    /**
     * Decode a composed RGB15-value and set red, green, blue to the decoded values.
     *
     * @param rgb The composed RGB-value
     */
    void setRGB15(uint16_t rgb);

    /**
     * Decode a composed RGB8-value and set red, green, blue to the decoded values.
     *
     * @param rgb The composed RGB-value
     */
    void setRGB8(uint8_t rgb);

    /**
     * Decode a composed RGB4-value and set red, green, blue to the decoded values.
     *
     * @param rgb The composed RGB-value
     */
    void setRGB4(uint8_t rgb);

    /**
     * Decode a composed RGB2-value and set red, green, blue to the decoded values.
     *
     * @param rgb The composed RGB-value
     */
    void setRGB2(uint8_t rgb);

    /**
     * Decode a composed RGB1-value and set red, green, blue to the decoded values.
     *
     * @param rgb The composed RGB-value
     */
    void setRGB1(uint8_t rgb);

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
    uint32_t getRGB32();

    /**
     * Get the RGB24-value.
     */
    uint32_t getRGB24();

    /**
     * Get the RGB16-value.
     */
    uint16_t getRGB16();

    /**
     * Get the RGB15-value.
     */
    uint16_t getRGB15();

    /**
     * Get the RGB8-value.
     */
    uint8_t getRGB8();

    /**
     * Get the RB4-value.
     */
    uint8_t getRGB4();

    /**
     * Get the RGB2-value.
     */
    uint8_t getRGB2();

    /**
     * Get the RGB1-value.
     */
    uint8_t getRGB1();

    /**
     * Get the RGB-value for a given color-colorDepth.
     *
     * @param depth The color-colorDepth
     */
    uint32_t getColorForDepth(uint8_t depth);

    /**
     * Blend this color with another color.
     *
     * @param color The color to blend with.
     */
    void blendWith(Color color);

private:
    uint8_t red, green, blue, alpha;
    int32_t rgb32 = -1;
    int32_t rgb24 = -1;
    int16_t rgb16 = -1;
    int16_t rgb15 = -1;
    int8_t rgb8 = -1;
    int8_t rgb4 = -1;
    int8_t rgb2 = -1;
    int8_t rgb1 = -1;

    /**
     * Reset the cached color values.
     */
    void invalidate();

    /**
     * Calculate the 32-bit value.
     */
    [[nodiscard]] uint32_t calcRGB32() const;

    /**
     * Calculate the 24-bit value.
     */
    [[nodiscard]] uint32_t calcRGB24() const;

    /**
     * Calculate the 16-bit value.
     */
    [[nodiscard]] uint16_t calcRGB16() const;

    /**
     * Calculate the 15-bit value.
     */
    [[nodiscard]] uint16_t calcRGB15() const;

    /**
     * Calculate the 8-bit value.
     */
    [[nodiscard]] uint8_t calcRGB8() const;

    /**
     * Calculate the 4-bit value.
     */
    [[nodiscard]] uint8_t calcRGB4() const;

    /**
     * Calculate the 2-bit value.
     */
    [[nodiscard]] uint8_t calcRGB2() const;

    /**
     * Calculate the 1-bit value.
     */
    [[nodiscard]] uint8_t calcRGB1() const;
};

}

#endif