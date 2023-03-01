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

#include "lib/util/base/Exception.h"
#include "Color.h"
#include "Colors.h"

namespace Util::Graphic {

Color::Color() : red(0), green(0), blue(0), alpha(255) {}

Color::Color(uint8_t red, uint8_t green, uint8_t blue) : red(red), green(green), blue(blue), alpha(255) {}

Color::Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) : red(red), green(green), blue(blue), alpha(alpha) {}

Color Color::fromRGB(uint32_t rgb, uint8_t depth) {
    switch (depth) {
        case 32:
            return fromRGB32(rgb);
        case 24:
            return fromRGB24(rgb);
        case 16:
            return fromRGB16(rgb);
        case 15:
            return fromRGB15(rgb);
        case 8:
            return fromRGB8(rgb);
        case 4:
            return fromRGB4(rgb);
        case 2:
            return fromRGB2(rgb);
        case 1:
            return fromRGB1(rgb);
        default:
            Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Color: Invalid depth!");
    }
}

uint32_t Color::getRGB32() const {
    return (alpha << 24) + (red << 16) + (green << 8) + blue;
}

uint32_t Color::getRGB24() const {
    return (red << 16) + (green << 8) + blue;
}

uint16_t Color::getRGB16() const {
    return (blue >> 3) | ((green >> 2) << 5) | ((red >> 3) << 11);
}

uint16_t Color::getRGB15() const {
    return (blue >> 3) | ((green >> 3) << 5) | ((red >> 3) << 10);
}

uint8_t Color::getRGB8() const {
    return (blue >> 6) | ((green >> 5) << 2) | ((red >> 5) << 5);
}

uint8_t Color::getRGB4() const {
    uint8_t ret = (blue >> 7) | ((green >> 7) << 1) | ((red >> 7) << 2);
    auto brightness = (red + green + blue) / 3;

    //Special case for gray
    if (ret == 0 && brightness > 42) {
        return ret + 8;
    }

    //Special case for light gray
    if (ret == 7 && brightness < 212) {
        return ret;
    }

    if (brightness > 127) {
        return ret + 8;
    }

    return ret;
}

uint8_t Color::getRGB2() const {
    uint8_t r = this->red >> 7;
    uint8_t g = this->green >> 7;
    uint8_t b = this->blue >> 7;

    //BLACK
    if (!b && !g && !r) {
        return 0;
    }

    //CYAN or GREEN
    if ((b || g) && !r) {
        return 1;
    }

    //MAGENTA or GREEN
    if ((b || r) && !g) {
        return 2;
    }

    //WHITE or YELLOW
    return 3;
}

uint8_t Color::getRGB1() const {
    if ((red >> 7) || (green >> 7) || (blue >> 7)) {
        return 1;
    }

    return 0;
}

Color Color::fromRGB32(uint32_t rgba) {
    uint8_t alpha = rgba >> 24;
    uint8_t red = rgba >> 16;
    uint8_t green = (rgba & 65280) >> 8;
    uint8_t blue = rgba & 255;

    return {red, green, blue, alpha};
}

Color Color::fromRGB24(uint32_t rgb) {
    uint8_t red = rgb >> 16;
    uint8_t green = (rgb & 65280) >> 8;
    uint8_t blue = rgb & 255;

    return {red, green, blue, 255};
}

Color Color::fromRGB16(uint16_t rgb) {
    uint8_t red = (rgb >> 11) * (256 / 32);
    uint8_t green = ((rgb & 2016) >> 5) * (256 / 64);
    uint8_t blue = (rgb & 31) * (256 / 32);

    return {red, green, blue, 255};
}

Color Color::fromRGB15(uint16_t rgb) {
    uint8_t red = (rgb >> 10) * (256 / 32);
    uint8_t green = ((rgb & 992) >> 5) * (256 / 32);
    uint8_t blue = (rgb & 31) * (256 / 32);

    return {red, green, blue, 255};
}

Color Color::fromRGB8(uint8_t rgb) {
    uint8_t red = (rgb >> 5) * (256 / 8);
    uint8_t green = ((rgb & 28) >> 2) * (256 / 8);
    uint8_t blue = (rgb & 3) * (256 / 4);

    return {red, green, blue, 255};
}

Color Color::fromRGB4(uint8_t rgb) {
    uint8_t red = (rgb & 4) ? 170 : 0;
    uint8_t green = (rgb & 2) ? 170 : 0;
    uint8_t blue = (rgb & 1) ? 170 : 0;

    if (rgb & 8) {
        red = 85;
        green = 85;
        blue = 85;
    }

    return {red, green, blue, 255};
}

Color Color::fromRGB2(uint8_t rgb) {
    uint8_t red = (rgb & 2) ? 170 : 0;
    uint8_t blue = ((rgb & 1) | (rgb & 2)) ? 170 : 0;
    uint8_t green = (rgb & 1) ? 170 : 0;

    return {red, green, blue, 255};
}

Color Color::fromRGB1(uint8_t rgb) {
    uint8_t color = rgb ? 255 : 0;

    return {color, color, color, 255};
}

uint8_t Color::getRed() const {
    return red;
}

uint8_t Color::getGreen() const {
    return green;
}

uint8_t Color::getBlue() const {
    return blue;
}

uint8_t Color::getAlpha() const {
    return alpha;
}

uint32_t Color::getColorForDepth(uint8_t depth) const {
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

Color Color::bright() const {
    uint8_t r = red + BRIGHTNESS_SHIFT;
    uint8_t g = green + BRIGHTNESS_SHIFT;
    uint8_t b = blue + BRIGHTNESS_SHIFT;

    if (r < red) {
        r = 255;
    }

    if (g < green) {
        g = 255;
    }

    if (b < blue) {
        b = 255;
    }

    return {r, g, b};
}

Color Color::dim() const {
    uint8_t r = red - BRIGHTNESS_SHIFT;
    uint8_t g = green - BRIGHTNESS_SHIFT;
    uint8_t b = blue - BRIGHTNESS_SHIFT;

    if (r > red) {
        r = 255;
    }

    if (g > green) {
        g = 255;
    }

    if (b > blue) {
        b = 255;
    }

    return {r, g, b};
}

Color Color::blend(const Color &color) const {
    if (color.alpha == 0) {
        return {red, green, blue, alpha};
    }

    if (color.alpha == 255) {
        return {color.red, color.green, color.blue, color.alpha};
    }

    if (alpha == 0) {
        return Colors::BLACK.blend(color);
    }

    double alpha1 = (static_cast<float>(color.alpha) / 255);
    double alpha2 = (static_cast<float>(alpha) / static_cast<float>(255));
    double alpha3 = alpha1 + (1 - alpha1) * alpha2;

    auto r = static_cast<uint8_t>((1 / alpha3) * (alpha1 * color.red + (1 - alpha1) * alpha2 * red));
    auto g = static_cast<uint8_t>((1 / alpha3) * (alpha1 * color.green + (1 - alpha1) * alpha2 * green));
    auto b = static_cast<uint8_t>((1 / alpha3) * (alpha1 * color.blue + (1 - alpha1) * alpha2 * blue));
    auto a = static_cast<uint8_t>(alpha3 * 255);

    return {r, g, b, a};
}

bool Color::operator==(const Color &other) const {
    return red == other.red && green == other.green && blue == other.blue;
}

}