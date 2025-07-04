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

#include "ResourceManager.h"

#include "lib/util/collection/Array.h"
#include "lib/util/collection/HashMap.h"
#include "lib/util/graphic/Image.h"
#include "lib/util/game/3d/ObjectFile.h"
#include "sound/WaveFile.h"

namespace Util::Game {

HashMap<String, Graphic::Image*> ResourceManager::images;
HashMap<String, D3::ObjectFile*> ResourceManager::objectFiles;
HashMap<String, GLuint> ResourceManager::textures;
HashMap<String, AudioBuffer*> ResourceManager::audioBuffers;

void ResourceManager::addImage(const String &key, Graphic::Image *image) {
    images.put(key, image);
}

bool ResourceManager::hasImage(const String &key) {
    return images.containsKey(key);
}

Graphic::Image* ResourceManager::getImage(const Util::String &key) {
    return images.get(key);
}

void ResourceManager::deleteImage(const Util::String &key) {
    if (images.containsKey(key)) {
        delete images.remove(key);
    }
}

void ResourceManager::clear() {
    for (auto *image : images.values()) {
        delete image;
    }
    images.clear();


    for (auto *objectFile : objectFiles.values()) {
        delete objectFile;
    }
    objectFiles.clear();
}

void ResourceManager::addObjectFile(const String &key, D3::ObjectFile *objectFile) {
    objectFiles.put(key, objectFile);
}

bool ResourceManager::hasObjectFile(const String &key) {
    return objectFiles.containsKey(key);
}

D3::ObjectFile * ResourceManager::getObjectFile(const String &key) {
    return objectFiles.get(key);
}

void ResourceManager::deleteObjectFile(const String &key) {
    objectFiles.remove(key);
}

void ResourceManager::addTexture(const String &key, GLuint texture) {
    textures.put(key, texture);
}

bool ResourceManager::hasTexture(const String &key) {
    return textures.containsKey(key);
}

GLuint ResourceManager::getTexture(const String &key) {
    return textures.get(key);
}

void ResourceManager::deleteTexture(const String &key) {
    if (textures.containsKey(key)) {
        glDisable(textures.remove(key));
    }
}

void ResourceManager::addAudioBuffer(const String &key, AudioBuffer *buffer) {
    audioBuffers.put(key, buffer);
}

bool ResourceManager::hasAudioBuffer(const String &key) {
    return audioBuffers.containsKey(key);
}

AudioBuffer* ResourceManager::getAudioBuffer(const String &key) {
    return audioBuffers.get(key);
}

void ResourceManager::deleteAudioBuffer(const String &key) {
    if (audioBuffers.containsKey(key)) {
        audioBuffers.remove(key);
    }
}
}
