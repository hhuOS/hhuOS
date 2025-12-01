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

#ifndef HHUOS_LIB_PULSAR_3D_MODEL_H
#define HHUOS_LIB_PULSAR_3D_MODEL_H

#include <stddef.h>

#include "util/base/String.h"
#include "util/graphic/Color.h"
#include "util/graphic/Colors.h"
#include "util/math/Vector3.h"
#include "pulsar/3d/Entity.h"
#include "pulsar/3d/Texture.h"
#include "pulsar/3d/ObjectFile.h"

namespace Pulsar::D3 {

/// A 3D model entity that can be rendered in a 3D scene.
/// The model is loaded from an object file (.obj) and can optionally have a texture applied.
/// The texture is loaded from a Bitmap file (.bmp). If no texture is provided,
/// the model will be rendered with a solid color. The object and texture files are cached
/// via the `Resources` class to avoid redundant loading.
/// All vertices are normalized to fit within a unit cube (1x1x1) centered at the origin (0,0,0).
/// The size of the model can be adjusted using the scale parameter when creating the model instance.
/// A sphere collider is automatically created, with the radius matching the largest scale dimension.
class Model : public Entity {

public:
    /// Create a new 3D model instance, loaded from the given object file path, with no texture, but a solid color.
    Model(size_t tag, const Util::String &modelPath, const Util::Math::Vector3<float> &position,
        const Util::Math::Vector3<float> &rotation, const Util::Math::Vector3<float> &scale,
        const Util::Graphic::Color &color = Util::Graphic::Colors::WHITE);

    /// Create a new 3D model instance, loaded from the given object file path, with the specified texture (.bmp file).
    Model(size_t tag, const Util::String &modelPath, const Util::String &texturePath,
        const Util::Math::Vector3<float> &position, const Util::Math::Vector3<float> &rotation,
        const Util::Math::Vector3<float> &scale);

    /// Initialize the model by loading the object file and texture (if provided).
    /// This method is called automatically by the engine when the model is added to a scene.
    void initialize() override;

    /// Draw the model using the provided graphics context.
    /// This method is called automatically by the engine during the rendering process.
    void draw(Graphics &graphics) const override;

    /// Get the vertices, that define the shape of the model.
    /// These are the 3D points that define the shape of the model.
    const Util::Array<Util::Math::Vector3<float>>& getVertices() const;

    /// Get the vertex normals, that define the orientation of the model's surfaces.
    /// These normals are used for lighting calculations to determine how light interacts with the model's surface.
    const Util::Array<Util::Math::Vector3<float>>& getVertexNormals() const;

    /// Get the vertex texture coordinates of the model.
    /// These coordinates map the texture image onto the model's surface.
    const Util::Array<Util::Math::Vector3<float>>& getVertexTextures() const;

    /// Get the draw order of the model's vertices.
    /// Each entry in the returned array is an index into the vertices array.
    const Util::Array<size_t>& getVertexDrawOrder() const;

    /// Get the draw order of the model's vertex normals.
    /// Each entry in the returned array is an index into the vertex normals array.
    const Util::Array<size_t>& getNormalDrawOrder() const;

    /// Get the draw order of the model's texture coordinates.
    /// Each entry in the returned array is an index into the vertex textures array.
    const Util::Array<size_t>& getTextureDrawOrder() const;

    /// Get the texture applied to the model.
    const Texture& getTexture() const;

    /// Get the solid color of the model (used if no texture is applied).
    const Util::Graphic::Color& getColor() const;

private:

    Util::String modelPath;
    Util::String texturePath;

    const ObjectFile *objectFile = nullptr;
    const Texture *texture = nullptr;

    Util::Graphic::Color color;
};

}

#endif
