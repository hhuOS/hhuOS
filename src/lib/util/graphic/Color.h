/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

#ifndef HHUOS_LIB_UTIL_GRAPHIC_COLOR_H
#define HHUOS_LIB_UTIL_GRAPHIC_COLOR_H

#include <stdint.h>

namespace Util::Graphic {

/// Represents a 32-bit color with red, green, blue, and alpha components (8 bits each).
/// Colors can be brightened, dimmed and blended with other colors.
/// Furthermore, they can be converted into their 1-, 2-, 4-, 8-, 15-, 16-, 24-, and 32-bit binary forms:
///
///     32-bit:
///      Alpha     Red     Green     Blue
///     XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX
///
///     24-bit:
///       Red     Green     Blue
///     XXXXXXXX XXXXXXXX XXXXXXXX
///
///     16-bit:
///      Red  Green  Blue
///     XXXXX XXXXXX XXXXX
///
///     15-bit:
///      Red  Green Blue
///     XXXXX XXXXX XXXXX
///
///     8-bit:
///     Red Green Blue
///     XX  XXX   XXX
///
///     4-bit:
///     Brightness Red Green Blue
///         X       X    X     X
///
///     2-bit:
///     Blue/Green Red
///         X       X
///
///     1-bit:
///     Black/White
///          X
///
/// The class stores colors in RGBA order (red first, blue last) in memory and is packed,
/// which means it already has the correct layout for direct usage in 32-bit RGBA frame buffers.
///
/// ### Example
/// ```c++
/// const auto red = Util::Graphic::Color(255, 0, 0);
/// const auto green = Util::Graphic::Color::fromRGB24(0x00FF00);
/// const auto dimmed = green.dim(); // dimmed = (170, 170, 0)
/// const auto rgb16 = red.getRGB16(); // rgb16 = 0xF800
/// ```
class Color {

public:

    Color() = default;

    Color(const uint8_t red, const uint8_t green, const uint8_t blue) :
        blue(blue), green(green), red(red) {}

    Color(const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha) :
        blue(blue), green(green), red(red), alpha(alpha) {}

    bool operator==(const Color &other) const {
        return red == other.red && green == other.green && blue == other.blue &&  alpha == other.alpha;
    }

    bool operator!=(const Color &other) const {
        return red != other.red || green != other.green || blue != other.blue || alpha != other.alpha;
    }

    [[nodiscard]] static Color fromRGB(uint32_t rgb, uint8_t depth);

    [[nodiscard]] static Color fromRGB32(uint32_t rgba);

    [[nodiscard]] static Color fromRGB24(uint32_t rgb);

    [[nodiscard]] static Color fromRGB16(uint16_t rgb);

    [[nodiscard]] static Color fromRGB15(uint16_t rgb);

    [[nodiscard]] static Color fromRGB8(uint8_t rgb);

    [[nodiscard]] static Color fromRGB4(uint8_t rgb);

    [[nodiscard]] static Color fromRGB2(uint8_t rgb);

    [[nodiscard]] static Color fromRGB1(uint8_t rgb);

    [[nodiscard]] Color bright() const;

    [[nodiscard]] Color dim() const;

    [[nodiscard]] Color withSaturation(uint8_t percentage) const;

    [[nodiscard]] Color blend(const Color &color) const;

    [[nodiscard]] Color withAlpha(const uint8_t alpha) const {
        return Color(red, green, blue, alpha);
    }

    [[nodiscard]] uint32_t getRGB32() const {
        return *reinterpret_cast<const uint32_t*>(this);
    }

    [[nodiscard]] uint32_t getRGB24() const {
        return *reinterpret_cast<const uint32_t*>(this) & 0x00ffffff;
    }

    [[nodiscard]] uint16_t getRGB16() const {
        return blue >> 3 | (green >> 2) << 5 | (red >> 3) << 11;
    }

    [[nodiscard]] uint16_t getRGB15() const {
        return blue >> 3 | (green >> 3) << 5 | (red >> 3) << 10;
    }

    [[nodiscard]] uint8_t getRGB8() const {
        return blue >> 6 | (green >> 5) << 2 | (red >> 5) << 5;
    }

    [[nodiscard]] uint8_t getRGB4() const {
        const uint8_t ret = blue >> 7 | (green >> 7) << 1 | (red >> 7) << 2;
        const auto brightness = (red + green + blue) / 3;

        // Special case for grey
        if (ret == 0 && brightness > 42) {
            return ret + 8;
        }

        // Special case for light grey
        if (ret == 7 && brightness < 212) {
            return ret;
        }

        if (brightness > 127) {
            return ret + 8;
        }

        return ret;
    }

    [[nodiscard]] uint8_t getRGB2() const {
        const uint8_t r = this->red >> 7;
        const uint8_t g = this->green >> 7;
        const uint8_t b = this->blue >> 7;

        // Black
        if (!b && !g && !r) {
            return 0;
        }

        // Cyan or Green
        if ((b || g) && !r) {
            return 1;
        }

        // Magenta or Green
        if ((b || r) && !g) {
            return 2;
        }

        // White or Yellow
        return 3;
    }

    [[nodiscard]] uint8_t getRGB1() const {
        if (red >> 7 || green >> 7 || blue >> 7) {
            return 1;
        }

        return 0;
    }

    [[nodiscard]] uint32_t getColorForDepth(const uint8_t depth) const {
        switch (depth) {
            case 32:
                return getRGB32();
            case 24:
                return getRGB24();
            case 16:
                return getRGB16();
            case 15:
                return getRGB15();
            case 8:
                return getRGB8();
            case 4:
                return getRGB4();
            case 2:
                return getRGB2();
            case 1:
                return getRGB1();
            default:
                return getRGB32();
        }
    }

    [[nodiscard]] uint8_t getRed() const {
        return red;
    }

    [[nodiscard]] uint8_t getGreen() const {
        return green;
    }

    [[nodiscard]] uint8_t getBlue() const {
        return blue;
    }

    [[nodiscard]] uint8_t getAlpha() const {
        return alpha;
    }

private:

    uint8_t blue = 0;
    uint8_t green = 0;
    uint8_t red = 0;
    uint8_t alpha = 255;

    static constexpr uint8_t BRIGHTNESS_SHIFT = 85;

} __attribute__((packed));

}

#endif