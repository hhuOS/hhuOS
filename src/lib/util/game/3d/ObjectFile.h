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

#ifndef HHUOS_OBJECTFILE_H
#define HHUOS_OBJECTFILE_H

#include "lib/util/base/String.h"
#include "lib/util/math/Vector3D.h"

namespace Util::Game::D3 {

class ObjectFile {

public:
    /**
     * Copy Constructor.
     */
    ObjectFile(const ObjectFile &other) = delete;

    /**
     * Assignment operator.
     */
    ObjectFile &operator=(const ObjectFile &other) = delete;

    /**
     * Destructor.
     */
    ~ObjectFile() = default;

    static ObjectFile* open(const String &path);

    [[nodiscard]] const Array<Math::Vector3D>& getVertices() const;

    [[nodiscard]] const Array<Math::Vector2D>& getEdges() const;

    ObjectFile(const Array<Math::Vector3D> &vertices, const Array<Math::Vector2D> &edges);

private:

    Array<Math::Vector3D> vertices;
    Array<Math::Vector2D> edges;
};

}

#endif
