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
 * The game engine has been enhanced with lighting capabilities during a bachelor's thesis by Kevin Weber
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-keweb100
 */

#ifndef HHUOS_LIGHT_H
#define HHUOS_LIGHT_H

#include <stdint.h>

#include "lib/util/math/Vector3.h"
#include "lib/util/graphic/Color.h"

namespace Util::Game::D3 {

class Light {

public:

    enum Type {
        DIRECTIONAL = 0,
        POINT = 1
    };

    /**
     * Constructor.
     */
    Light(uint8_t index, Type type, const Math::Vector3<double> &position, const Graphic::Color &diffuseColor, const Graphic::Color &specularColor);

    /**
     * Copy Constructor.
     */
    Light(const Light &other) = delete;

    /**
     * Assignment operator.
     */
    Light &operator=(const Light &other) = delete;

    /**
     * Destructor.
     */
    ~Light() = default;

    [[nodiscard]] uint8_t getIndex() const;

    [[nodiscard]] Type getType() const;

    [[nodiscard]] const Math::Vector3<double>& getPosition() const;

    [[nodiscard]] const Graphic::Color& getDiffuseColor() const;

    [[nodiscard]] const Graphic::Color& getSpecularColor() const;

    void setPosition(const Math::Vector3<double> &position);

    void setDiffuseColor(const Graphic::Color &diffuseColor);

    void setSpecularColor(const Graphic::Color &specularColor);

private:

    const uint8_t index;
    const Type type;

    Math::Vector3<double> position;
    Graphic::Color diffuseColor;
    Graphic::Color specularColor;
};

}

#endif
