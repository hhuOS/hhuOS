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

#ifndef HHUOS_RESOURCEMANAGER_H
#define HHUOS_RESOURCEMANAGER_H

#include "lib/util/base/String.h"
#include "lib/tinygl/include/GL/gl.h"

namespace Util {
namespace Graphic {
class Image;
}  // namespace Graphic
template <typename K, typename V> class HashMap;
namespace Game {
namespace D3 {
class ObjectFile;
}  // namespace D3
}  // namespace Game
}  // namespace Util

namespace Util::Game {

class ResourceManager {

public:
    /**
     * Default Constructor.
     * Deleted, as this class has only static members.
     */
    ResourceManager() = delete;

    /**
     * Copy Constructor.
     */
    ResourceManager(const ResourceManager &other) = delete;

    /**
     * Assignment operator.
     */
    ResourceManager &operator=(const ResourceManager &other) = delete;

    /**
     * Destructor.
     */
    ~ResourceManager() = default;

    static void addImage(const String &key, Graphic::Image *image);

    static bool hasImage(const String &key);

    static Graphic::Image* getImage(const String &key);

    static void deleteImage(const String &key);

    static void addObjectFile(const String &key, D3::ObjectFile *objectFile);

    static bool hasObjectFile(const String &key);

    static D3::ObjectFile * getObjectFile(const String &key);

    static void deleteObjectFile(const String &key);

    static void addTexture(const String &key, GLuint texture);

    static bool hasTexture(const String &key);

    static GLuint getTexture(const String &key);

    static void deleteTexture(const String &key);

    static void clear();

private:

    static HashMap<String, Graphic::Image*> images;
    static HashMap<String, D3::ObjectFile*> objectFiles;
    static HashMap<String, GLuint> textures;
};

}

#endif
