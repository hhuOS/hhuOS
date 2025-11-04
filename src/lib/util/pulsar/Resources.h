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

#ifndef HHUOS_LIB_UTIL_GAME_RESOURCES_H
#define HHUOS_LIB_UTIL_GAME_RESOURCES_H

#include "pulsar/3d/ObjectFile.h"
#include "audio/AudioBuffer.h"
#include "graphic/Image.h"
#include "base/String.h"
#include "lib/tinygl/include/GL/gl.h"

/// Provides resource management for game assets like images, 3D object files, textures, and audio buffers.
/// Resources can be added, retrieved, checked for existence, and deleted using string keys.
/// They are stored in internal hash maps for efficient access.
/// This way, resources can be reused across the game without redundant loading.
/// These functions are used internally by the game engine and should not be called directly by game code.
/// The engine provides higher-level abstractions for the resources managed here
/// (e.g., `D2::Sprite` for images, `D3::Model` for 3D objects or `AudioTrack` for audio buffers).
namespace Util::Pulsar::Resources {

/// Add an image resource with the specified key.
void addImage(const String &key, const Graphic::Image *image);

/// Check if an image resource with the specified key exists.
bool hasImage(const String &key);

/// Retrieve the image resource associated with the specified key.
const Graphic::Image* getImage(const String &key);

/// Delete the image resource associated with the specified key.
void deleteImage(const String &key);

/// Add a 3D object file resource with the specified key.
void addObjectFile(const String &key, const D3::ObjectFile *objectFile);

/// Check if a 3D object file resource with the specified key exists.
bool hasObjectFile(const String &key);

/// Retrieve the 3D object file resource associated with the specified key.
const D3::ObjectFile* getObjectFile(const String &key);

/// Delete the 3D object file resource associated with the specified key.
void deleteObjectFile(const String &key);

/// Add an OpenGL texture resource with the specified key.
void addTexture(const String &key, GLuint texture);

/// Check if an OpenGL texture resource with the specified key exists.
bool hasTexture(const String &key);

/// Retrieve the OpenGL texture resource associated with the specified key.
GLuint getTexture(const String &key);

/// Delete the OpenGL texture resource associated with the specified key.
void deleteTexture(const String &key);

/// Add an audio buffer resource with the specified key.
void addAudioBuffer(const String &key, const AudioBuffer *buffer);

/// Check if an audio buffer resource with the specified key exists.
bool hasAudioBuffer(const String &key);

/// Retrieve the audio buffer resource associated with the specified key.
const AudioBuffer* getAudioBuffer(const String &key);

/// Delete the audio buffer resource associated with the specified key.
void deleteAudioBuffer(const String &key);

/// Clear all resources managed by this namespace.
void clear();

}

#endif
