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

#include "Color.h"

namespace Util::Graphic {

Color::Color() : red(0), green(0), blue(0), alpha(255) {

}

Color::Color(uint8_t red, uint8_t green, uint8_t blue) : red(red), green(green), blue(blue), alpha(255) {

}


Color::Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) : red(red), green(green), blue(blue),
                                                                        alpha(alpha) {

}

Color::Color(uint32_t rgb, uint8_t depth) : red(0), green(0), blue(0), alpha(255) {
    switch (depth) {
        case 32:
            setRGB32(rgb);
            break;
        case 24:
            setRGB24(rgb);
            break;
        case 16:
            setRGB16(static_cast<uint16_t>(rgb));
            break;
        case 15:
            setRGB15(static_cast<uint16_t>(rgb));
            break;
        case 8:
            setRGB8(static_cast<uint8_t>(rgb));
            break;
        case 4:
            setRGB4(static_cast<uint8_t>(rgb));
            break;
        case 2:
            setRGB2(static_cast<uint8_t>(rgb));
            break;
        case 1:
            setRGB1(static_cast<uint8_t>(rgb));
            break;
        default:
            setRGB32(rgb);
            break;
    }
}

void Color::invalidate() {
    rgb32 = -1;
    rgb24 = -1;
    rgb16 = -1;
    rgb15 = -1;
    rgb8 = -1;
    rgb4 = -1;
    rgb2 = -1;
    rgb1 = -1;
}

uint32_t Color::calcRGB32() const {
    return (uint32_t) ((alpha << 24) + (red << 16) + (green << 8) + blue);
}

uint32_t Color::calcRGB24() const {
    return (uint32_t) ((red << 16) + (green << 8) + blue);
}

uint16_t Color::calcRGB16() const {
    return (uint16_t) ((blue >> 3) | ((green >> 2) << 5) | ((red >> 3) << 11));
}

uint16_t Color::calcRGB15() const {
    return (uint16_t) ((blue >> 3) | ((green >> 3) << 5) | ((red >> 3) << 10));
}

uint8_t Color::calcRGB8() const {
    return (uint8_t) ((blue >> 6) | ((green >> 5) << 2) | ((red >> 5) << 5));
}

uint8_t Color::calcRGB4() const {
    uint8_t ret = ((blue >> 7) | ((green >> 7) << 1) | ((red >> 7) << 2));

    auto brightness = static_cast<uint16_t>((red + green + blue) / 3);

    //Special case for gray
    if (ret == 0 && brightness > 42)
        return static_cast<uint8_t>(ret + 8);

    //Special case for light gray
    if (ret == 7 && brightness < 212)
        return ret;

    if (brightness > 127)
        return static_cast<uint8_t>(ret + 8);

    return ret;
}

uint8_t Color::calcRGB2() const {
    uint8_t red = this->red >> 7;
    uint8_t green = this->green >> 7;
    uint8_t blue = this->blue >> 7;

    //BLACK
    if (!blue && !green && !red)
        return 0;

    //CYAN or GREEN
    if ((blue || green) && !red)
        return 1;

    //MAGENTA or GREEN
    if ((blue || red) && !green)
        return 2;

    //WHITE or YELLOW
    return 3;
}

uint8_t Color::calcRGB1() const {
    if ((red >> 7) || (green >> 7) || (blue >> 7))
        return 1;

    return 0;
}

void Color::setRed(uint8_t red) {
    this->red = red;
    invalidate();
}

void Color::setGreen(uint8_t green) {
    this->green = green;
    invalidate();
}

void Color::setBlue(uint8_t blue) {
    this->blue = blue;
    invalidate();
}

void Color::setAlpha(uint8_t alpha) {
    this->alpha = alpha;
    invalidate();
}

void Color::setRGB(uint8_t red, uint8_t green, uint8_t blue) {
    this->red = red;
    this->green = green;
    this->blue = blue;
    invalidate();
}

void Color::setRGBA(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
    this->red = red;
    this->green = green;
    this->blue = blue;
    this->alpha = 0;
    invalidate();
}

void Color::setRGB(uint32_t rgb, uint8_t depth) {
    switch (depth) {
        case 32:
            setRGB32(rgb);
        case 24:
            setRGB24(rgb);
            break;
        case 16:
            setRGB16(static_cast<uint16_t>(rgb));
            break;
        case 15:
            setRGB15(static_cast<uint16_t>(rgb));
            break;
        case 8:
            setRGB8(static_cast<uint8_t>(rgb));
            break;
        case 4:
            setRGB4(static_cast<uint8_t>(rgb));
            break;
        case 2:
            setRGB2(static_cast<uint8_t>(rgb));
            break;
        case 1:
            setRGB1(static_cast<uint8_t>(rgb));
            break;
        default:
            setRGB32(rgb);
            break;
    }
}

void Color::setRGB32(uint32_t rgba) {
    alpha = static_cast<uint8_t>(rgba >> 24);
    red = static_cast<uint8_t>(rgba >> 16);
    green = static_cast<uint8_t>((rgba & 65280) >> 8);
    blue = static_cast<uint8_t>(rgba & 255);

    invalidate();
}

void Color::setRGB24(uint32_t rgb) {
    red = static_cast<uint8_t>(rgb >> 16);
    green = static_cast<uint8_t>((rgb & 65280) >> 8);
    blue = static_cast<uint8_t>(rgb & 255);
    alpha = 255;

    invalidate();
}

void Color::setRGB16(uint16_t rgb) {
    red = static_cast<uint8_t>((rgb >> 11) * (256 / 32));
    green = static_cast<uint8_t>(((rgb & 2016) >> 5) * (256 / 64));
    blue = static_cast<uint8_t>((rgb & 31) * (256 / 32));
    alpha = 255;

    invalidate();
}

void Color::setRGB15(uint16_t rgb) {
    red = static_cast<uint8_t>((rgb >> 10) * (256 / 32));
    green = static_cast<uint8_t>(((rgb & 992) >> 5) * (256 / 32));
    blue = static_cast<uint8_t>((rgb & 31) * (256 / 32));
    alpha = 255;

    invalidate();
}

void Color::setRGB8(uint8_t rgb) {
    red = static_cast<uint8_t>((rgb >> 5) * (256 / 8));
    green = static_cast<uint8_t>(((rgb & 28) >> 2) * (256 / 8));
    blue = static_cast<uint8_t>((rgb & 3) * (256 / 4));
    alpha = 255;

    invalidate();
}

void Color::setRGB4(uint8_t rgb) {
    red = static_cast<uint8_t>((rgb & 4) ? 170 : 0);
    green = static_cast<uint8_t>((rgb & 2) ? 170 : 0);
    blue = static_cast<uint8_t>((rgb & 1) ? 170 : 0);
    alpha = 255;

    if (rgb & 8) {
        red = 85;
        green = 85;
        blue = 85;
    }

    invalidate();
}

void Color::setRGB2(uint8_t rgb) {
    red = static_cast<uint8_t>((rgb & 2) ? 170 : 0);
    blue = static_cast<uint8_t>(((rgb & 1) | (rgb & 2)) ? 170 : 0);
    green = static_cast<uint8_t>((rgb & 1) ? 170 : 0);
    alpha = 255;

    invalidate();
}

void Color::setRGB1(uint8_t rgb) {
    red = static_cast<uint8_t>(rgb ? 255 : 0);
    green = red;
    blue = red;
    alpha = 255;

    invalidate();
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

uint32_t Color::getRGB32() {
    if (rgb32 == -1)
        rgb32 = calcRGB32();

    return static_cast<uint32_t>(rgb32);
}

uint32_t Color::getRGB24() {
    if (rgb24 == -1)
        rgb24 = calcRGB24();

    return static_cast<uint32_t>(rgb24);
}

uint16_t Color::getRGB16() {
    if (rgb16 == -1)
        rgb16 = calcRGB16();

    return static_cast<uint16_t>(rgb16);
}

uint16_t Color::getRGB15() {
    if (rgb15 == -1)
        rgb15 = calcRGB15();

    return static_cast<uint16_t>(rgb15);
}

uint8_t Color::getRGB8() {
    if (rgb8 == -1)
        rgb8 = calcRGB8();

    return static_cast<uint8_t>(rgb8);
}

uint8_t Color::getRGB4() {
    if (rgb4 == -1)
        rgb4 = calcRGB4();

    return static_cast<uint8_t>(rgb4);
}

uint8_t Color::getRGB2() {
    if (rgb2 == -1)
        rgb2 = calcRGB2();

    return static_cast<uint8_t>(rgb2);
}

uint8_t Color::getRGB1() {
    if (rgb1 == -1)
        rgb1 = calcRGB1();

    return static_cast<uint8_t>(rgb1);
}

uint32_t Color::getColorForDepth(uint8_t depth) {
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

void Color::blendWith(Color color) {
    if (color.alpha == 0) {
        return;
    }

    if (alpha == 0 || color.alpha == 255) {
        red = color.red;
        green = color.green;
        blue = color.blue;
        alpha = color.alpha;

        invalidate();
        return;
    }

    double alpha1 = (static_cast<double>(color.alpha) / 255);
    double alpha2 = (static_cast<double>(alpha) / static_cast<float>(255));
    double alpha3 = alpha1 + (1 - alpha1) * alpha2;

    red = static_cast<uint8_t>((1 / alpha3) * (alpha1 * color.red + (1 - alpha1) * alpha2 * red));
    green = static_cast<uint8_t>((1 / alpha3) * (alpha1 * color.green + (1 - alpha1) * alpha2 * green));
    blue = static_cast<uint8_t>((1 / alpha3) * (alpha1 * color.blue + (1 - alpha1) * alpha2 * blue));
    alpha = static_cast<uint8_t>(alpha3 * 255);

    invalidate();
}

}