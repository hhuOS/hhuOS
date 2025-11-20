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

#include "Resources.h"

#include "util/collection/Array.h"
#include "util/collection/HashMap.h"
#include "util/graphic/Image.h"
#include "pulsar/3d/ObjectFile.h"

namespace Pulsar::Resources {

static Util::HashMap<Util::String, const Util::Graphic::Image*> images;
static Util::HashMap<Util::String, const D3::ObjectFile*> objectFiles;
static Util::HashMap<Util::String, const D3::Texture*> textures;
static Util::HashMap<Util::String, const AudioBuffer*> audioBuffers;

void addImage(const Util::String &key, const Util::Graphic::Image *image) {
    images.put(key, image);
}

bool hasImage(const Util::String &key) {
    return images.containsKey(key);
}

const Util::Graphic::Image* getImage(const Util::String &key) {
    return images.get(key);
}

void deleteImage(const Util::String &key) {
    if (images.containsKey(key)) {
        delete images.remove(key);
    }
}

void clear() {
    for (const auto *image : images.getValues()) {
        delete image;
    }
    images.clear();


    for (const auto *objectFile : objectFiles.getValues()) {
        delete objectFile;
    }
    objectFiles.clear();
}

void addObjectFile(const Util::String &key, const D3::ObjectFile *objectFile) {
    objectFiles.put(key, objectFile);
}

bool hasObjectFile(const Util::String &key) {
    return objectFiles.containsKey(key);
}

const D3::ObjectFile* getObjectFile(const Util::String &key) {
    return objectFiles.get(key);
}

void deleteObjectFile(const Util::String &key) {
    objectFiles.remove(key);
}

void addTexture(const Util::String &key, const D3::Texture *texture) {
    textures.put(key, texture);
}

bool hasTexture(const Util::String &key) {
    return textures.containsKey(key);
}

const D3::Texture* getTexture(const Util::String &key) {
    return textures.get(key);
}

void deleteTexture(const Util::String &key) {
    if (textures.containsKey(key)) {
        const auto *texture = textures.remove(key);
        glDisable(static_cast<GLint>(texture->getTextureID()));
        delete texture;
    }
}

void addAudioBuffer(const Util::String &key, const AudioBuffer *buffer) {
    audioBuffers.put(key, buffer);
}

bool hasAudioBuffer(const Util::String &key) {
    return audioBuffers.containsKey(key);
}

const AudioBuffer* getAudioBuffer(const Util::String &key) {
    return audioBuffers.get(key);
}

void deleteAudioBuffer(const Util::String &key) {
    if (audioBuffers.containsKey(key)) {
        audioBuffers.remove(key);
    }
}

}
