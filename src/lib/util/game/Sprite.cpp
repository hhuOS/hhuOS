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

#include "Sprite.h"

#include <stdint.h>

#include "GameManager.h"
#include "lib/util/graphic/BitmapFile.h"
#include "lib/util/graphic/Image.h"

namespace Util::Game {

Sprite::Sprite(const Util::String &path, double width, double height) {
    auto *file = Graphic::BitmapFile::open(path);
    auto transformation = GameManager::getTransformation();

    image = file->scale(static_cast<uint16_t>(width * transformation), static_cast<uint16_t>(height * transformation));
    delete file;
}

Sprite::~Sprite() {
    delete image;
}

const Graphic::Image &Sprite::getImage() const {
    return *image;
}

double Sprite::getWidth() const {
    return image->getWidth();
}

double Sprite::getHeight() const {
    return image->getHeight();
}

}