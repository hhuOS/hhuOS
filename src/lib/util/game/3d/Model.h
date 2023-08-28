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

#ifndef HHUOS_MODEL_H
#define HHUOS_MODEL_H

#include "lib/util/math/Vector2D.h"
#include "lib/util/math/Vector3D.h"
#include "lib/util/graphic/Color.h"
#include "lib/util/io/file/File.h"
#include "Entity.h"
#include "ObjectFile.h"

namespace Util::Game::D3 {

class Model : public Entity {

public:
    /**
     * Constructor.
     */
    Model(uint32_t tag, const String &modelPath, const Math::Vector3D &position, const Math::Vector3D &rotation, const Math::Vector3D &scale);

    /**
     * Constructor.
     */
    Model(uint32_t tag, const String &modelPath, const Math::Vector3D &position, const Math::Vector3D &rotation, const Math::Vector3D &scale, const Graphic::Color &color);

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

    void onTransformChange() override;

private:

    void calculateTransformedVertices();

    const String &modelPath;
    const Graphic::Color color = Util::Graphic::Colors::GREEN;

    ObjectFile *objectFile = nullptr;
    Array<Math::Vector3D> transformedBuffer = Util::Array<Math::Vector3D>(0);


};

}

#endif
