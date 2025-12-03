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

#include "util/base/Address.h"
#include "util/base/Panic.h"

namespace Util {
namespace Graphic {

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
    /// Create a new color instance with default values (black, fully opaque).
    ///
    /// ### Example
    /// ```c++
    /// const auto black = Util::Graphic::Color();
    /// const auto black32 = black.getRGB32(); // black32 = 0xff000000
    /// ```
    Color() = default;

    /// Create a new color instance with the given red, green, and blue values.
    /// The alpha value is set to fully opaque (255).
    ///
    /// ### Example
    /// ```c++
    /// const auto yellow = Util::Graphic::Color(255, 255, 0);
    /// const auto yellow32 = yellow.getRGB32(); // yellow32 = 0xffffff00
    /// ```
    Color(const uint8_t red, const uint8_t green, const uint8_t blue) :
        blue(blue), green(green), red(red) {}

    /// Create a new color instance with the given red, green, blue, and alpha values.
    ///
    /// ### Example
    /// ```c++
    /// const auto semiTransparentBlue = Util::Graphic::Color(0, 0, 255, 128);
    /// const auto blue32 = semiTransparentBlue.getRGB32(); // blue32 = 0x800000ff
    /// ```
    Color(const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha) :
        blue(blue), green(green), red(red), alpha(alpha) {}

    /// Compare this color with another one for equality.
    ///
    /// ### Example
    /// ```c++
    /// const auto blue1 = Util::Graphic::Color(0, 0, 255);
    /// const auto blue2 = Util::Graphic::Color::fromRGB24(0x0000ff);
    /// const auto transparentBlue = Util::Graphic::Color(0, 0, 255, 128);
    /// const auto red = Util::Graphic::Color(255, 0, 0);
    ///
    /// const auto equal = (blue1 == blue2); // areEqual = true
    /// const auto notEqual1 = (blue1 != transparentBlue); // notEqual1 = true
    /// const auto notEqual2 = (blue1 == red); // notEqual2 = false
    /// ```
    bool operator==(const Color &other) const {
        return red == other.red && green == other.green && blue == other.blue &&  alpha == other.alpha;
    }

    /// Compare this color with another one for inequality.
    ///
    /// ### Example
    /// ```c++
    /// const auto blue1 = Util::Graphic::Color(0, 0, 255);
    /// const auto blue2 = Util::Graphic::Color::fromRGB24(0x0000ff);
    /// const auto transparentBlue = Util::Graphic::Color(0, 0, 255, 128);
    /// const auto red = Util::Graphic::Color(255, 0, 0);
    ///
    /// const auto notEqual1 = (blue1 != transparentBlue); // notEqual1 = true
    /// const auto notEqual2 = (blue1 != red); // notEqual2 = true
    /// const auto equal = (blue1 != blue2); // equal = false
    /// ```
    bool operator!=(const Color &other) const {
        return red != other.red || green != other.green || blue != other.blue || alpha != other.alpha;
    }

    /// Create a Color instance from a packed RGB value with the specified color depth.
    /// Supported depths are 1, 2, 4, 8, 15, 16, 24, and 32 bits. An unsupported color depth will fire a panic.
    ///
    /// ### Example
    /// ```c++
    /// const auto blue16 = Util::Graphic::Color::fromRGB(0x001f, 16); // Blue color from 16-bit RGB
    /// const auto blue24 = Util::Graphic::Color::fromRGB(0x0000ff, 24); // Blue color from 24-bit RGB
    /// const auto blue32 = Util::Graphic::Color::fromRGB(0xff0000ff, 32); // Blue color from 32-bit RGB
    ///
    /// const auto equal1 = (blue16 == blue24); // equal1 = true
    /// const auto equal2 = (blue24 == blue32); // equal2 = true
    /// const auto equal3 = (blue16 == blue32); // equal3 = true
    ///
    /// const auto invalid = Util::Graphic::Color::fromRGB(0x00ff00, 12); // Panic due to unsupported depth
    /// ```
    static Color fromRGB(uint32_t rgb, uint8_t depth);

    /// Create a Color instance from a packed 32-bit RGBA value.
    ///
    /// ### Example
    /// ```c++
    /// const auto semiTransparentBlue = Util::Graphic::Color::fromRGB32(0x800000ff);
    /// const auto blue32 = semiTransparentBlue.getRGB32(); // blue32 = 0x800000ff
    /// ```
    static Color fromRGB32(const uint32_t rgba) {
        Color color;
        Address(&color).copyRange(Address(&rgba), sizeof(uint32_t));
        return color;
    }

    /// Create a Color instance from a packed 24-bit RGB value.
    /// The alpha value is set to fully opaque (255).
    ///
    /// ### Example
    /// ```c++
    /// const auto blue = Util::Graphic::Color::fromRGB24(0x0000ff);
    /// const auto blue32 = blue.getRGB32(); // blue32 = 0xff0000ff
    /// ```
    static Color fromRGB24(const uint32_t rgb) {
        return fromRGB32(rgb | 0xff000000);
    }

    /// Create a Color instance from a packed 16-bit RGB value
    /// with 5 bits for red, 6 bits for green, and 5 bits for blue
    /// The alpha value is set to fully opaque (255).
    ///
    /// ### Example
    /// ```c++
    /// const auto blue = Util::Graphic::Color::fromRGB16(0x001f);
    /// const auto blue32 = blue.getRGB32(); // blue32 = 0xff0000ff
    /// ```
    static Color fromRGB16(const uint16_t rgb) {
        const uint8_t red = (rgb >> 11) << 3;
        const uint8_t green = ((rgb >> 5) & 0x003f) << 2;
        const uint8_t blue = (rgb & 0x001f) << 3;

        return Color(red, green, blue, 255);
    }

    /// Create a Color instance from a packed 15-bit RGB value with 5 bits for each color.
    /// The alpha value is set to fully opaque (255).
    ///
    /// ### Example
    /// ```c++
    /// const auto blue = Util::Graphic::Color::fromRGB15(0x001f);
    /// const auto blue32 = blue.getRGB32(); // blue32 = 0xff0000ff
    /// ```
    static Color fromRGB15(const uint16_t rgb) {
        const uint8_t red = (rgb >> 10) << 3;
        const uint8_t green = ((rgb >> 5) & 0x001f) << 3;
        const uint8_t blue = (rgb & 0x001f) << 3;

        return Color(red, green, blue, 255);
    }

    /// Create a Color instance from a packed 8-bit RGB value
    /// with 2 bits for red, 3 bits for green, and 3 bits for blue.
    /// The alpha value is set to fully opaque (255).
    ///
    /// ### Example
    /// ```c++
    /// const auto blue = Util::Graphic::Color::fromRGB8(0x07);
    /// const auto blue32 = blue.getRGB32(); // blue32 = 0xff0000ff
    /// ```
    static Color fromRGB8(const uint8_t rgb) {
        const uint8_t red = (rgb >> 5) << 5;
        const uint8_t green = ((rgb >> 2) & 0x07) << 5;
        const uint8_t blue = (rgb & 0x03) << 6;

        return Color(red, green, blue, 255);
    }

    /// Create a Color instance from a packed 4-bit RGB value with 1 bit for each color and 1 bit for brightness.
    /// This results in colors with either 0 or 170 for each color component,
    /// which can be brightened to 255 if the brightness bit is set.
    /// This matches the standard 16-color palette used in CGA text mode.
    /// The alpha value is set to fully opaque (255).
    ///
    /// ### Example
    /// ```c++
    /// const auto blue = Util::Graphic::Color::fromRGB4(0x1);
    /// const auto blue32 = blue.getRGB32(); // blue32 = 0xff0000aa
    ///
    /// const auto brightBlue = Util::Graphic::Color::fromRGB4(0x9);
    /// const auto brightBlue32 = brightBlue.getRGB32(); // brightBlue32 = 0xff0000ff
    /// ```
    static Color fromRGB4(const uint8_t rgb) {
        const uint8_t brightness = (rgb >> 3) * 85;
        const uint8_t red = (rgb & 0x4) ? 170 + brightness : 0;
        const uint8_t green = (rgb & 0x2) ? 170 + brightness : 0;
        const uint8_t blue = (rgb & 0x1) ? 170 + brightness : 0;

        return Color(red, green, blue, 255);
    }

    /// Create a Color instance from a packed 2-bit RGB value with 1 bit for red and 1 bit for blue/green.
    /// This results in colors with either 0 or 170 for each color component.
    /// This matches both 4-color palettes used in CGA graphics mode.
    ///
    /// ### Example
    /// ```c++
    /// const auto red = Util::Graphic::Color::fromRGB2(0x2);
    /// const auto red32 = green.getRGB32(); // green32 = 0xff00aaa0
    ///
    /// const auto green = Util::Graphic::Color::fromRGB2(0x1);
    /// const auto green32 = green.getRGB32(); // green32 = 0xff0000aa
    ///
    /// const auto blue = Util::Graphic::Color::fromRGB2(0x1);
    /// const auto blue32 = blue.getRGB32(); // blue32 = 0xff0000aa
    /// ```
    static Color fromRGB2(const uint8_t rgb) {
        const uint8_t red = (rgb & 0x2) ? 170 : 0;
        const uint8_t blue = (rgb & 0x1) | (rgb & 2) ? 170 : 0;
        const uint8_t green = (rgb & 0x1) ? 170 : 0;

        return Color(red, green, blue, 255);
    }

    /// Create a Color instance from a packed 1-bit RGB value.
    /// This results in either black (0) or white (255) for each color component.
    /// The alpha value is set to fully opaque (255).
    ///
    /// ### Example
    /// ```c++
    /// const auto black = Util::Graphic::Color::fromRGB1(0x0);
    /// const auto black32 = black.getRGB32(); // black32 = 0xff000000
    ///
    /// const auto white = Util::Graphic::Color::fromRGB1(0x1);
    /// const auto white32 = white.getRGB32(); // white32 = 0xffffffff
    /// ```
    static Color fromRGB1(const uint8_t rgb) {
        uint8_t color = rgb ? 255 : 0;

        return {color, color, color, 255};
    }

    /// Get the packed 32-bit RGBA value of this color.
    /// Since the class is stored in RGBA order in memory, this is a simple reinterpret cast.
    ///
    /// ### Example
    /// ```c++
    /// const auto semiTransparentBlue = Util::Graphic::Color(0, 0, 255, 128);
    /// const auto blue32 = semiTransparentBlue.getRGB32(); // blue32 = 0x800000ff
    /// ```
    uint32_t getRGB32() const {
        return *reinterpret_cast<const uint32_t*>(this);
    }

    /// Get the packed 24-bit RGB value of this color.
    /// Since the class is stored in RGBA order in memory,
    /// this is a simple reinterpret cast with masking (discarding alpha value).
    ///
    /// ### Example
    /// ```c++
    /// const auto blue = Util::Graphic::Color(0, 0, 255);
    /// const auto blue24 = blue.getRGB24(); // blue24 = 0x0000ff
    /// ```
    uint32_t getRGB24() const {
        return *reinterpret_cast<const uint32_t*>(this) & 0x00ffffff;
    }

    /// Get the packed 16-bit RGB value of this color
    /// with 5 bits for red, 6 bits for green, and 5 bits for blue.
    ///
    /// ### Example
    /// ```c++
    /// const auto blue = Util::Graphic::Color(0, 0, 255);
    /// const auto blue16 = blue.getRGB16(); // blue16 = 0x001f
    /// ```
    uint16_t getRGB16() const {
        return blue >> 3 | (green >> 2) << 5 | (red >> 3) << 11;
    }

    /// Get the packed 15-bit RGB value of this color with 5 bits for each color.
    ///
    /// ### Example
    /// ```c++
    /// const auto blue = Util::Graphic::Color(0, 0, 255);
    /// const auto blue15 = blue.getRGB15(); // blue15 = 0x001f
    /// ```
    uint16_t getRGB15() const {
        return blue >> 3 | (green >> 3) << 5 | (red >> 3) << 10;
    }

    /// Get the packed 8-bit RGB value of this color
    /// with 2 bits for red, 3 bits for green, and 3 bits for blue.
    ///
    /// ### Example
    /// ```c++
    /// const auto blue = Util::Graphic::Color(0, 0, 255);
    /// const auto blue8 = blue.getRGB8(); // blue8 = 0x07
    /// ```
    uint8_t getRGB8() const {
        return blue >> 6 | (green >> 5) << 2 | (red >> 5) << 5;
    }

    /// Get the packed 4-bit RGB value of this color with 1 bit for each color and 1 bit for brightness.
    /// This matches the standard 16-color palette used in CGA text mode.
    ///
    /// ### Example
    /// ```c++
    /// const auto blue = Util::Graphic::Color(0, 0, 170);
    /// const auto blue4 = blue.getRGB4(); // blue4 = 0x1
    ///
    /// const auto brightBlue = Util::Graphic::Color(0, 0, 255); // Same as blue.bright()
    /// const auto brightBlue4 = brightBlue.getRGB4(); // brightBlue4 = 0x9
    /// ```
    uint8_t getRGB4() const {
        const uint8_t color = blue >> 7 | (green >> 7) << 1 | (red >> 7) << 2;
        const auto brightness = (red + green + blue) / 3;

        // Special case for gray (black if brightness <= 42, grey otherwise)
        if (color == 0 && brightness > 42) {
            return color | 8;
        }

        // Special case for light gray (white if brightness >= 212, light gray otherwise)
        if (color == 7 && brightness < 212) {
            return color;
        }

        return brightness > 127 ? (color | 8) : color;
    }

    /// Get the packed 2-bit RGB value of this color with 1 bit for red and 1 bit for blue/green.
    /// This matches both 4-color palettes used in CGA graphics mode.
    ///
    /// ### Example
    /// ```c++
    /// const auto red = Util::Graphic::Color(255, 0, 0);
    /// const auto red2 = red.getRGB2(); // red2 = 0x2
    ///
    /// const auto green = Util::Graphic::Color(0, 255, 0);
    /// const auto green2 = green.getRGB2(); // green2 = 0x1
    ///
    /// const auto blue = Util::Graphic::Color(0, 0, 255);
    /// const auto blue2 = blue.getRGB2(); // blue2 = 0x1
    /// ```
    uint8_t getRGB2() const {
        return red >> 6 | green >> 7 | blue >> 7;
    }

    /// Get the packed 1-bit RGB value of this color.
    /// This results in either 0 (black), if all color components are below 128,
    /// or 1 (white), if at least one color component is 128 or above.
    ///
    /// ### Example
    /// ```c++
    /// const auto black = Util::Graphic::Color(0, 0, 0);
    /// const auto black1 = black.getRGB1(); // black1 = 0
    ///
    /// const auto white = Util::Graphic::Color(255, 255, 255);
    /// const auto white1 = white.getRGB1(); // white1 = 1
    ///
    /// const auto gray = Util::Graphic::Color(100, 150, 100);
    /// const auto gray1 = gray.getRGB1(); // gray1 = 1
    /// ```
    uint8_t getRGB1() const {
        return red >> 7 | green >> 7 | blue >> 7 ? 1 : 0;
    }

    /// Get the packed RGB value of this color with the specified color depth.
    /// Supported depths are 1, 2, 4, 8, 15, 16, 24, and 32 bits. An unsupported color depth will fire a panic.
    ///
    /// ### Example
    /// ```c++
    /// const auto blue = Util::Graphic::Color(0, 0, 255);
    /// const auto blue16 = blue.getColorForDepth(16); // blue16 = 0x001f
    /// const auto blue24 = blue.getColorForDepth(24); // blue24 = 0x0000ff
    /// const auto blue32 = blue.getColorForDepth(32); // blue32 = 0xff0000ff
    /// ```
    uint32_t getColorForDepth(uint8_t depth) const;

    /// Get the red component of this color (0-255).
    uint8_t getRed() const {
        return red;
    }

    /// Get a brightened version of this color.
    /// Each color component is increased by 85, up to a maximum of 255.
    /// This matches the brightness levels used in CGA text mode.
    ///
    /// ### Example
    /// ```c++
    /// const auto green = Util::Graphic::Color(0, 100, 0);
    /// const auto brightGreen = green.bright(); // brightGreen = (85, 185, 85)
    /// ```
    Color bright() const {
        const uint8_t r = red + BRIGHTNESS_SHIFT;
        const uint8_t g = green + BRIGHTNESS_SHIFT;
        const uint8_t b = blue + BRIGHTNESS_SHIFT;

        return Color(r < red ? static_cast<uint8_t>(255) : r,
            g < green ? static_cast<uint8_t>(255) : g,
            b < blue ? static_cast<uint8_t>(255) : b);
    }

    /// Get a dimmed version of this color.
    /// Each color component is decreased by 85, down to a minimum of 0.
    /// This matches the brightness levels used in CGA text mode.
    ///
    /// ### Example
    /// ```c++
    /// const auto green = Util::Graphic::Color(0, 200, 0);
    /// const auto dimmedGreen = green.dim(); // dimmedGreen = (0, 115, 0)
    /// ```
    Color dim() const {
        const uint8_t r = red - BRIGHTNESS_SHIFT;
        const uint8_t g = green - BRIGHTNESS_SHIFT;
        const uint8_t b = blue - BRIGHTNESS_SHIFT;

        return Color(r > red ? static_cast<uint8_t>(255) : r,
            g > green ? static_cast<uint8_t>(255) : g,
            b > blue ? static_cast<uint8_t>(255) : b);
    }

    /// Get a version of this color with adjusted saturation.
    /// The percentage parameter specifies the desired saturation level (0-100).
    /// A percentage of 0 results in a fully desaturated color (white),
    /// while a percentage of 100 retains the original color.
    ///
    /// ### Example
    /// ```c++
    /// const auto green = Util::Graphic::Color(0, 255, 0);
    /// const auto desaturatedGreen = green.withSaturation(0); // desaturatedGreen = (255, 255, 255)
    /// const auto halfSaturatedGreen = green.withSaturation(50); // halfSaturatedGreen = (128, 255, 128)
    /// const auto fullSaturatedGreen = green.withSaturation(100); // fullSaturatedGreen = (0, 255, 0)
    /// ```
    Color withSaturation(uint8_t percentage) const {
        if (percentage > 100) {
            percentage = 100;
        }

        return Color( static_cast<uint8_t>(255 - (255 - red) * percentage / 100),
            static_cast<uint8_t>(255 - (255 - green) * percentage / 100),
            static_cast<uint8_t>(255 - (255 - blue) * percentage / 100)
        );
    }

    /// Get a version of this color with the specified alpha value.
    ///
    /// ### Example
    /// ```c++
    /// const auto blue = Util::Graphic::Color(0, 0, 255);
    /// const auto semiTransparentBlue = blue.withAlpha(128); // semiTransparentBlue = (0, 0, 255, 128)
    /// ```
    Color withAlpha(const uint8_t alpha) const {
        return Color(red, green, blue, alpha);
    }

    /// Blend this color with another color, taking into account their alpha values.
    /// The resulting color is a combination of both colors based on their transparency.
    /// If the other color is fully transparent (alpha = 0), this color is returned.
    /// If the other color is fully opaque (alpha = 255), the other color is returned.
    /// If this color is fully transparent (alpha = 0), the other color is blended with a fully opaque black color.
    ///
    /// ### Example
    /// ```c++
    /// const auto red = Util::Graphic::Color(255, 0, 0, 128);
    /// const auto blue = Util::Graphic::Color(0, 0, 255, 128);
    /// const auto blended = red.blend(blue); // blended = (85, 0, 170, 192)
    /// ```
    Color blend(const Color &other) const {
        if (other.alpha == 0) {
            return *this;
        }

        if (other.alpha == 255) {
            return other;
        }

        if (alpha == 0) {
            return Color().blend(other);
        }

        const auto alpha1 = static_cast<float>(other.alpha) / 255;
        const auto alpha2 = static_cast<float>(alpha) / static_cast<float>(255);
        const auto alpha3 = alpha1 + (1 - alpha1) * alpha2;

        const auto r = static_cast<uint8_t>((1 / alpha3) * (alpha1 * other.red + (1 - alpha1) * alpha2 * red));
        const auto g = static_cast<uint8_t>((1 / alpha3) * (alpha1 * other.green + (1 - alpha1) * alpha2 * green));
        const auto b = static_cast<uint8_t>((1 / alpha3) * (alpha1 * other.blue + (1 - alpha1) * alpha2 * blue));
        const auto a = static_cast<uint8_t>(alpha3 * 255);

        return Color(r, g, b, a);
    }

    /// Get the green component of this color (0-255).
    uint8_t getGreen() const {
        return green;
    }

    /// Get the blue component of this color (0-255).
    uint8_t getBlue() const {
        return blue;
    }

    /// Get the alpha component of this color (0-255).
    uint8_t getAlpha() const {
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
}

#endif