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

#ifndef HHUOS_RESOURCEMANAGER_H
#define HHUOS_RESOURCEMANAGER_H

#include "lib/util/base/String.h"

namespace Util {
namespace Graphic {
class Image;
}  // namespace Graphic
template <typename K, typename V> class HashMap;
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

    static void clear();

private:

    static HashMap<String, Graphic::Image*> images;
};

}

#endif
