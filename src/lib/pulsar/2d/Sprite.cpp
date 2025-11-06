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
 *
 * The game engine is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 *
 * It has been enhanced with 3D-capabilities during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 *
 * The 3D-rendering has been rewritten using OpenGL (TinyGL) during a bachelor's thesis by Kevin Weber
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-keweb100
 *
 * The 2D particle system is based on a bachelor's thesis, written by Abdulbasir Gümüs.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-abgue101
 */

#include "Sprite.h"

#include <stdint.h>

#include "util/graphic/Color.h"
#include "util/graphic/Image.h"
#include "util/graphic/BitmapFile.h"
#include "pulsar/Game.h"
#include "pulsar/Graphics.h"
#include "pulsar/Resources.h"

namespace Pulsar::D2 {

Sprite::Sprite() {
    if (Resources::hasImage("empty")) {
        image = Resources::getImage("empty");
    } else {
        image = new Util::Graphic::Image(0, 0, new Util::Graphic::Color[0]);
        Resources::addImage("empty", image);
    }
}

Sprite::Sprite(const Util::String &path, const double width, const double height) : size(width, height) {
    const auto transformation = Game::getInstance().getScreenTransformation();
    const uint16_t pixelWidth = static_cast<uint16_t>(width * transformation) + 1;
    const uint16_t pixelHeight = static_cast<uint16_t>(height * transformation) + 1;
    const auto key = Util::String::format("%s_%u_%u", static_cast<const char*>(path), pixelWidth, pixelHeight);

    if (Resources::hasImage(key)) {
        image = Resources::getImage(key);
    } else {
        auto *file = Util::Graphic::BitmapFile::open(path);
        image = file->scale(pixelWidth, pixelHeight);
        delete file;

        Resources::addImage(key, image);
    }
}

Sprite::Sprite(const Util::Graphic::Color &color, double width, double height) {
    const auto transformation = Game::getInstance().getScreenTransformation();
    const uint16_t pixelWidth = static_cast<uint16_t>(width * transformation) + 1;
    const uint16_t pixelHeight = static_cast<uint16_t>(height * transformation) + 1;
    const auto key = Util::String::format("color%x_%u_%u", color.getRGB32(), pixelWidth, pixelHeight);

    if (Resources::hasImage(key)) {
        image = Resources::getImage(key);
    } else {
        auto *pixelBuffer = new Util::Graphic::Color[pixelWidth * pixelHeight];
        for (uint16_t y = 0; y < pixelHeight; y++) {
            for (uint16_t x = 0; x < pixelWidth; x++) {
                pixelBuffer[y * pixelWidth + x] = color;
            }
        }

        image = new Util::Graphic::Image(pixelWidth, pixelHeight, pixelBuffer);
        Resources::addImage(key, image);
    }
}

const Util::Graphic::Image& Sprite::getImage() const {
    return *image;
}

const Util::Math::Vector2<double> &Sprite::getOriginalSize() const {
    return size;
}

Util::Math::Vector2<double> Sprite::getSize() const {
    return Util::Math::Vector2<double>(size.getX() * scale.getX(), size.getY() * scale.getY());
}

const Util::Math::Vector2<double> &Sprite::getScale() const {
    return scale;
}

void Sprite::rotate(const double angle) {
    rotationAngle += angle;
}

void Sprite::setRotation(const double angle) {
    rotationAngle = angle;
}

double Sprite::getRotation() const {
    return rotationAngle;
}

void Sprite::setAlpha(const double alpha) {
    Sprite::alpha = alpha;
}

double Sprite::getAlpha() const {
    return alpha;
}

void Sprite::setScale(const double scale) {
    setScale(Util::Math::Vector2<double>(scale, scale));
}

void Sprite::setScale(const Util::Math::Vector2<double> &scale) {
    Sprite::scale = scale;
}

void Sprite::flipX() {
    xFlipped = !xFlipped;
}

void Sprite::setXFlipped(const bool flipped) {
    xFlipped = flipped;
}

void Sprite::draw(const Graphics &graphics, const Util::Math::Vector2<double> &position) const {
    graphics.drawImage2D(position, *image, xFlipped, alpha, scale, rotationAngle);
}

}