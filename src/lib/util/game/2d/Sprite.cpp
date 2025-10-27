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
 */

#include "Sprite.h"

#include <stdint.h>
#include <game/Game.h>

#include "lib/util/graphic/BitmapFile.h"
#include "lib/util/graphic/Image.h"
#include "lib/util/graphic/Color.h"
#include "lib/util/game/Resources.h"
#include "lib/util/game/Graphics.h"

namespace Util::Game::D2 {

Sprite::Sprite() : size(0, 0) {
    if (Resources::hasImage("empty")) {
        image = Resources::getImage("empty");
    } else {
        image = new Graphic::Image(0, 0, new Graphic::Color[0]);
        Resources::addImage("empty", image);
    }
}

Sprite::Sprite(const String &path, double width, double height) : size(width, height) {
    auto transformation = Game::getInstance().getScreenTransformation();
    auto pixelWidth = static_cast<uint16_t>(width * transformation) + 1;
    auto pixelHeight = static_cast<uint16_t>(height * transformation) + 1;
    auto key = String::format("%s_%u_%u", static_cast<const char*>(path), pixelWidth, pixelHeight);

    if (Resources::hasImage(key)) {
        image = Resources::getImage(key);
    } else {
        auto *file = Graphic::BitmapFile::open(path);
        image = file->scale(pixelWidth, pixelHeight);
        delete file;

        Resources::addImage(key, image);
    }
}

const Graphic::Image& Sprite::getImage() const {
    return *image;
}

const Math::Vector2<double> &Sprite::getOriginalSize() const {
    return size;
}

Math::Vector2<double> Sprite::getSize() const {
    return Math::Vector2<double>(size.getX() * scale.getX(), size.getY() * scale.getY());
}

const Math::Vector2<double> &Sprite::getScale() const {
    return scale;
}

double Sprite::getRotation() const {
    return rotationAngle;
}

double Sprite::getAlpha() const {
    return alpha;
}

void Sprite::setScale(const Math::Vector2<double> &scale) {
    Sprite::scale = scale;
}

void Sprite::setScale(double scale) {
    Sprite::scale = Math::Vector2<double>(scale, scale);
}

void Sprite::setRotation(double angle) {
    rotationAngle = angle;
}

void Sprite::setAlpha(double alpha) {
    Sprite::alpha = alpha;
}

void Sprite::rotate(double angle) {
    rotationAngle += angle;
}

void Sprite::flipX() {
    xFlipped = !xFlipped;
}

void Sprite::draw(const Graphics &graphics, const Math::Vector2<double> &position) const {
    graphics.drawImage2D(position, *image, xFlipped, alpha, scale, rotationAngle);
}

}