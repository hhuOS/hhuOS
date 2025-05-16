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

#ifndef HHUOS_MODEL_H
#define HHUOS_MODEL_H

#include <stdint.h>

#include "lib/util/graphic/Color.h"
#include "Entity.h"
#include "Texture.h"
#include "lib/util/base/String.h"
#include "lib/util/graphic/Colors.h"

namespace Util {
namespace Math {
template <typename T> class Vector3;
}  // namespace Math
template <typename T> class Array;

namespace Game {
namespace D3 {
class ObjectFile;
}  // namespace D3
}  // namespace Game
}  // namespace Util

namespace Util::Game::D3 {

class Model : public Entity {

public:
    /**
     * Constructor.
     */
    Model(uint32_t tag, const String &modelPath, const Math::Vector3<double> &position, const Math::Vector3<double> &rotation, const Math::Vector3<double> &scale, const Graphic::Color &color = Graphic::Colors::WHITE);

    /**
     * Constructor.
     */
    Model(uint32_t tag, const String &modelPath, const String &texturePath, const Math::Vector3<double> &position, const Math::Vector3<double> &rotation, const Math::Vector3<double> &scale);

    /**
     * Copy Constructor.
     */
    Model(const Model &other) = delete;

    /**
     * Assignment operator.
     */
    Model &operator=(const Model &other) = delete;

    /**
     * Destructor.
     */
    ~Model() override = default;

    void initialize() override;

    void draw(Graphics &graphics) override;

    [[nodiscard]] const Array<Math::Vector3<double>>& getVertices() const;

    [[nodiscard]] const Array<Math::Vector3<double>>& getVertexNormals() const;

    [[nodiscard]] const Array<Math::Vector3<double>>& getVertexTextures() const;

    [[nodiscard]] const Array<uint32_t>& getVertexDrawOrder() const;

    [[nodiscard]] const Array<uint32_t>& getNormalDrawOrder() const;

    [[nodiscard]] const Array<uint32_t>& getTextureDrawOrder() const;

    [[nodiscard]] const Texture& getTexture() const;

protected:

    [[nodiscard]] const Graphic::Color& getColor() const;

private:

    String modelPath;
    String texturePath;

    Texture texture;
    const Graphic::Color color = Graphic::Color(255, 255, 255);

    ObjectFile *objectFile = nullptr;
};

}

#endif
