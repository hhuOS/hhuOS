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

#ifndef HHUOS_LIB_PULSAR_RESOURCES_H
#define HHUOS_LIB_PULSAR_RESOURCES_H

#include "pulsar/3d/ObjectFile.h"
#include "pulsar/3d/Texture.h"
#include "audio/AudioBuffer.h"
#include "util/graphic/Image.h"
#include "util/base/String.h"
#include "util/collection/HashMap.h"
#include "tinygl/include/GL/gl.h"

/// Provides resource management for game assets like images, 3D object files, textures, and audio buffers.
/// Resources can be added, retrieved, checked for existence, and deleted using string keys.
/// They are stored in internal hash maps for efficient access.
/// This way, resources can be reused across the game without redundant loading.
/// These functions are used internally by the game engine and should not be called directly by game code.
/// The engine provides higher-level abstractions for the resources managed here
/// (e.g., `D2::Sprite` for images, `D3::Model` for 3D objects or `AudioTrack` for audio buffers).
namespace Pulsar {
namespace Resources {

static Util::HashMap<Util::String, const Util::Graphic::Image*> images;
static Util::HashMap<Util::String, const D3::ObjectFile*> objectFiles;
static Util::HashMap<Util::String, const D3::Texture*> textures;
static Util::HashMap<Util::String, const AudioBuffer*> audioBuffers;

/// Add an image resource with the specified key.
/// The image object must be allocated on the heap and the Resources namespace takes ownership of it.
/// This means it will be automatically deleted when the resource is removed or when `clear()` is called.
static void addImage(const Util::String &key, const Util::Graphic::Image *image) {
    images.put(key, image);
}

/// Check if an image resource with the specified key exists.
static bool hasImage(const Util::String &key) {
    return images.containsKey(key);
}

/// Retrieve the image resource associated with the specified key.
static const Util::Graphic::Image* getImage(const Util::String &key) {
    return images.get(key);
}

/// Delete the image resource associated with the specified key.
/// This also frees the heap memory allocated for the image.
static void deleteImage(const Util::String &key) {
    if (images.containsKey(key)) {
        delete images.remove(key);
    }
}

/// Add a 3D object file resource with the specified key.
/// The object file object must be allocated on the heap and the Resources namespace takes ownership of it.
/// This means it will be automatically deleted when the resource is removed or when `clear()` is called.
static void addObjectFile(const Util::String &key, const D3::ObjectFile *objectFile) {
    objectFiles.put(key, objectFile);
}

/// Check if a 3D object file resource with the specified key exists.
static bool hasObjectFile(const Util::String &key) {
    return objectFiles.containsKey(key);
}

/// Retrieve the 3D object file resource associated with the specified key.
static const D3::ObjectFile* getObjectFile(const Util::String &key) {
    return objectFiles.get(key);
}

/// Delete the 3D object file resource associated with the specified key.
/// This also frees the heap memory allocated for the object file.
static void deleteObjectFile(const Util::String &key) {
    objectFiles.remove(key);
}

/// Add an OpenGL texture resource with the specified key.
/// The texture object must be allocated on the heap and the Resources namespace takes ownership of it.
/// This means it will be automatically deleted when the resource is removed or when `clear()` is called.
static void addTexture(const Util::String &key, const D3::Texture *texture) {
    textures.put(key, texture);
}

/// Check if an OpenGL texture resource with the specified key exists.
static bool hasTexture(const Util::String &key) {
    return textures.containsKey(key);
}

/// Retrieve the OpenGL texture resource associated with the specified key.
static const D3::Texture* getTexture(const Util::String &key) {
    return textures.get(key);
}

/// Delete the OpenGL texture resource associated with the specified key.
/// This also frees the heap memory allocated for the texture object and disables it in OpenGL.
static void deleteTexture(const Util::String &key) {
    if (textures.containsKey(key)) {
        const auto *texture = textures.remove(key);
        glDisable(static_cast<GLint>(texture->getTextureID()));
        delete texture;
    }
}

/// Add an audio buffer resource with the specified key.
/// The audio buffer object must be allocated on the heap and the Resources namespace takes ownership of it.
/// This means it will be automatically deleted when the resource is removed or when `clear()` is called.
static void addAudioBuffer(const Util::String &key, const AudioBuffer *buffer) {
    audioBuffers.put(key, buffer);
}

/// Check if an audio buffer resource with the specified key exists.
static bool hasAudioBuffer(const Util::String &key) {
    return audioBuffers.containsKey(key);
}

/// Retrieve the audio buffer resource associated with the specified key.
static const AudioBuffer* getAudioBuffer(const Util::String &key) {
    return audioBuffers.get(key);
}

/// Delete the audio buffer resource associated with the specified key.
/// This also frees the heap memory allocated for the audio buffer.
static void deleteAudioBuffer(const Util::String &key) {
    if (audioBuffers.containsKey(key)) {
        audioBuffers.remove(key);
    }
}

/// Clear all resources managed by this namespace.
/// This deletes all images, object files, textures and audio buffers, freeing their allocated memory.
static void clear() {
    for (const auto *image : images.getValues()) {
        delete image;
    }
    images.clear();


    for (const auto *objectFile : objectFiles.getValues()) {
        delete objectFile;
    }
    objectFiles.clear();
}

}
}

#endif
