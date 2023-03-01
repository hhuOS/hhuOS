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
 *
 * The network stack is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-maseh100
 */

#include "Sprite.h"

#include <cstdint>

#include "GameManager.h"
#include "lib/util/graphic/BitmapFile.h"
#include "lib/util/graphic/Image.h"
#include "lib/util/graphic/Color.h"
#include "ResourceManager.h"

namespace Util::Game {

Sprite::Sprite() : width(0), height(0) {
    if (ResourceManager::hasImage("empty")) {
        image = ResourceManager::getImage("empty");
    } else {
        image = new Graphic::Image(0, 0, new Graphic::Color[0]);
        ResourceManager::addImage("empty", image);
    }
}

Sprite::Sprite(const Util::String &path, double width, double height) : width(width), height(height) {
    auto key = String::format("%s_%x_%x", static_cast<const char*>(path), width, height);
    if (ResourceManager::hasImage(key)) {
        image = ResourceManager::getImage(key);
    } else {
        auto *file = Graphic::BitmapFile::open(path);
        auto transformation = GameManager::getTransformation();

        image = file->scale(static_cast<uint16_t>(width * transformation) + 1, static_cast<uint16_t>(height * transformation) + 1);
        delete file;

        ResourceManager::addImage(key, image);
    }
}

const Graphic::Image &Sprite::getImage() const {
    return *image;
}

double Sprite::getWidth() const {
    return width;
}

double Sprite::getHeight() const {
    return height;
}

}