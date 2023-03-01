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

#include "ResourceManager.h"

#include "lib/util/collection/Array.h"
#include "lib/util/collection/Collection.h"
#include "lib/util/collection/HashMap.h"
#include "lib/util/collection/Iterator.h"
#include "lib/util/graphic/Image.h"

namespace Util::Game {

HashMap<String, Graphic::Image*> ResourceManager::images;

void ResourceManager::addImage(const String &key, Graphic::Image *image) {
    images.put(key, image);
}

bool ResourceManager::hasImage(const String &key) {
    return images.containsKey(key);
}

Util::Graphic::Image *Util::Game::ResourceManager::getImage(const Util::String &key) {
    return images.get(key);
}

void Util::Game::ResourceManager::deleteImage(const Util::String &key) {
    if (images.containsKey(key)) {
        delete images.remove(key);
    }
}

void ResourceManager::clear() {
    for (auto *image : images.values()) {
        delete image;
    }

    images.clear();
}

}