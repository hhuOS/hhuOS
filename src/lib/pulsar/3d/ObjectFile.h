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

#ifndef HHUOS_LIB_PULSAR_3D_OBJECTFILE_H
#define HHUOS_LIB_PULSAR_3D_OBJECTFILE_H

#include <stddef.h>

#include "util/base/String.h"
#include "util/math/Vector3.h"
#include "util/collection/Array.h"

namespace Pulsar {
namespace D3 {

/// A class to load and store 3D object data from an OBJ file.
/// The class parses the file and stores vertices, vertex normals, vertex textures, and their respective draw orders.
class ObjectFile {

public:
    /// Create a new object file instance by loading the OBJ file from the specified path.
    explicit ObjectFile(const Util::String &path);

    /// Get the array of vertices, that define the shape of the 3D object.
    const Util::Array<Util::Math::Vector3<float>>& getVertices() const {
        return vertices;
    }

    /// Get the array of vertex normals, that define the orientation of the 3D object's surfaces.
    /// These are used for lighting calculations during rendering.
    const Util::Array<Util::Math::Vector3<float>>& getVertexNormals() const {
        return vertexNormals;
    }

    /// Get the array of texture coordinates, that map 2D textures onto the 3D object's surfaces.
    const Util::Array<Util::Math::Vector3<float>>& getVertexTextures() const {
        return vertexTextures;
    }

    /// Get the draw order for the vertices, defining how the vertices are connected to form faces.
    /// Each entry in the returned array is an index into the vertices array.
    const Util::Array<size_t>& getVertexDrawOrder() const {
        return vertexDrawOrder;
    }

    /// Get the draw order for the vertex normals.
    /// Each entry in the returned array is an index into the vertex normals array.
    const Util::Array<size_t>& getNormalDrawOrder() const {
        return normalDrawOrder;
    }

    /// Get the draw order for the texture coordinates.
    /// Each entry in the returned array is an index into the vertex textures array.
    const Util::Array<size_t>& getTextureDrawOrder() const {
        return textureDrawOrder;
    }

private:

    Util::Array<Util::Math::Vector3<float>> vertices;
    Util::Array<Util::Math::Vector3<float>> vertexNormals;
    Util::Array<Util::Math::Vector3<float>> vertexTextures;
    Util::Array<uint32_t> vertexDrawOrder;
    Util::Array<uint32_t> normalDrawOrder;
    Util::Array<uint32_t> textureDrawOrder;
};

}
}

#endif
