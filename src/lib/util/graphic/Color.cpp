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

#include "Color.h"

#include "util/base/Panic.h"
#include "util/graphic/Colors.h"

namespace Util::Graphic {

Color Color::fromRGB(const uint32_t rgb, const uint8_t depth) {
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
            Util::Panic::fire(Panic::INVALID_ARGUMENT, "Color: Invalid color depth!");
    }
}

Color Color::fromRGB32(const uint32_t rgba) {
    const uint8_t alpha = rgba >> 24;
    const uint8_t red = (rgba >> 16) & 0x000000ff;
    const uint8_t green = (rgba >> 8) & 0x000000ff;
    const uint8_t blue = rgba & 0x000000ff;

    return Color(red, green, blue, alpha);
}

Color Color::fromRGB24(const uint32_t rgb) {
    const uint8_t red = rgb >> 16;
    const uint8_t green = (rgb >> 8) & 0x000000ff;
    const uint8_t blue = rgb & 0x000000ff;

    return Color(red, green, blue, 255);
}

Color Color::fromRGB16(const uint16_t rgb) {
    const uint8_t red = (rgb >> 11) << 3;
    const uint8_t green = ((rgb >> 5) & 0x003f) << 2;
    const uint8_t blue = (rgb & 0x001f) << 2;

    return Color(red, green, blue, 255);
}

Color Color::fromRGB15(const uint16_t rgb) {
    const uint8_t red = (rgb >> 10) << 3;
    const uint8_t green = ((rgb >> 5) & 0x001f) << 3;
    const uint8_t blue = (rgb & 0x001f) << 3;

    return Color(red, green, blue, 255);
}

Color Color::fromRGB8(const uint8_t rgb) {
    const uint8_t red = (rgb >> 5) << 5;
    const uint8_t green = ((rgb >> 2) & 0x07) << 5;
    const uint8_t blue = (rgb & 0x03) << 6;

    return Color(red, green, blue, 255);
}

Color Color::fromRGB4(const uint8_t rgb) {
    const uint8_t red = (rgb & 0x4) ? 170 : 0;
    const uint8_t green = (rgb & 0x2) ? 170 : 0;
    const uint8_t blue = (rgb & 0x1) ? 170 : 0;

    if (rgb & 8) {
        return Color(red, green, blue, 255).bright();
    }

    return Color(red, green, blue, 255);
}

Color Color::fromRGB2(const uint8_t rgb) {
    const uint8_t red = (rgb & 0x2) ? 170 : 0;
    const uint8_t blue = (rgb & 0x1) | (rgb & 2) ? 170 : 0;
    const uint8_t green = (rgb & 0x1) ? 170 : 0;

    return Color(red, green, blue, 255);
}

Color Color::fromRGB1(const uint8_t rgb) {
    uint8_t color = rgb ? 255 : 0;

    return {color, color, color, 255};
}

Color Color::bright() const {
    const uint8_t r = red + BRIGHTNESS_SHIFT;
    const uint8_t g = green + BRIGHTNESS_SHIFT;
    const uint8_t b = blue + BRIGHTNESS_SHIFT;

    return Color(r < red ? static_cast<uint8_t>(255) : r,
        g < green ? static_cast<uint8_t>(255) : g,
        b < blue ? static_cast<uint8_t>(255) : b);
}

Color Color::dim() const {
    const uint8_t r = red - BRIGHTNESS_SHIFT;
    const uint8_t g = green - BRIGHTNESS_SHIFT;
    const uint8_t b = blue - BRIGHTNESS_SHIFT;

    return Color(r > red ? static_cast<uint8_t>(255) : r,
        g > green ? static_cast<uint8_t>(255) : g,
        b > blue ? static_cast<uint8_t>(255) : b);
}

Color Color::withSaturation(uint8_t percentage) const {
    if (percentage > 100) {
        percentage = 100;
    }

    return {
        static_cast<uint8_t>(255 - (255 - red) * percentage / 100),
        static_cast<uint8_t>(255 - (255 - green) * percentage / 100),
        static_cast<uint8_t>(255 - (255 - blue) * percentage / 100),
    };
}

Color Color::blend(const Color &color) const {
    if (color.alpha == 0) {
        return Color(red, green, blue, alpha);
    }

    if (color.alpha == 255) {
        return Color(color.red, color.green, color.blue, color.alpha);
    }

    if (alpha == 0) {
        return Colors::BLACK.blend(color);
    }

    const auto alpha1 = (static_cast<float>(color.alpha) / 255);
    const auto alpha2 = (static_cast<float>(alpha) / static_cast<float>(255));
    const auto alpha3 = alpha1 + (1 - alpha1) * alpha2;

    auto r = static_cast<uint8_t>((1 / alpha3) * (alpha1 * color.red + (1 - alpha1) * alpha2 * red));
    auto g = static_cast<uint8_t>((1 / alpha3) * (alpha1 * color.green + (1 - alpha1) * alpha2 * green));
    auto b = static_cast<uint8_t>((1 / alpha3) * (alpha1 * color.blue + (1 - alpha1) * alpha2 * blue));
    auto a = static_cast<uint8_t>(alpha3 * 255);

    return Color(r, g, b, a);
}

}