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
 */

#ifndef HHUOS_OBJECTFILE_H
#define HHUOS_OBJECTFILE_H

#include <stdint.h>

#include "lib/util/base/String.h"
#include "lib/util/math/Vector3.h"
#include "lib/util/collection/Array.h"

namespace Pulsar::D3 {

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

    static ObjectFile* open(const Util::String &path);

    [[nodiscard]] const Util::Array<Util::Math::Vector3<double>>& getVertices() const;

    [[nodiscard]] const Util::Array<Util::Math::Vector3<double>>& getVertexNormals() const;

    [[nodiscard]] const Util::Array<Util::Math::Vector3<double>>& getVertexTextures() const;

    [[nodiscard]] const Util::Array<uint32_t>& getVertexDrawOrder() const;

    [[nodiscard]] const Util::Array<uint32_t>& getNormalDrawOrder() const;

    [[nodiscard]] const Util::Array<uint32_t>& getTextureDrawOrder() const;

    ObjectFile(const Util::Array<Util::Math::Vector3<double>> &vertices, const Util::Array<Util::Math::Vector3<double>> &vertexNormals, const Util::Array<Util::Math::Vector3<double>> &vertexTextures, const Util::Array<uint32_t> &vertexDrawOrder, const Util::Array<uint32_t> &normalDrawOrder, const Util::Array<uint32_t> &textureDrawOrder);

private:

    Util::Array<Util::Math::Vector3<double>> vertices;
    Util::Array<Util::Math::Vector3<double>> vertexNormals;
    Util::Array<Util::Math::Vector3<double>> vertexTextures;
    Util::Array<uint32_t> vertexDrawOrder;
    Util::Array<uint32_t> normalDrawOrder;
    Util::Array<uint32_t> textureDrawOrder;
};

}

#endif
