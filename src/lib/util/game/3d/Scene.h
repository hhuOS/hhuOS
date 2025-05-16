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

#ifndef HHUOS_SCENE_3D_H
#define HHUOS_SCENE_3D_H

#include <stdint.h>

#include "lib/tinygl/include/GL/gl.h"
#include "lib/util/game/Scene.h"
#include "Light.h"
#include "lib/util/graphic/Colors.h"
#include "lib/util/graphic/Color.h"

namespace Util {
namespace Math {
template <typename T> class Vector3;
}  // namespace Math
}  // namespace Util

namespace Util::Game::D3 {

class Scene : public Util::Game::Scene {

public:

    enum GlRenderStyle : GLint {
        POINTS = GL_POINT,
        LINES = GL_LINE,
        FILL = GL_FILL
    };

    enum GlShadeModel : GLint {
        FLAT = GL_FLAT,
        SMOOTH = GL_SMOOTH
    };

    /**
     * Default Constructor.
     */
    Scene() = default;

    /**
     * Copy Constructor.
     */
    Scene(const Scene &other) = delete;

    /**
     * Assignment operator.
     */
    Scene &operator=(const Scene &other) = delete;

    /**
     * Destructor.
     */
    ~Scene() override = default;

    void initializeScene(Graphics &graphics) final;

    void updateEntities(double delta) final;

    void checkCollisions() final;

    void setAmbientLight(const Graphic::Color &ambientLight);

    Light& addLight(Light::Type type, const Math::Vector3<double> &position, const Graphic::Color &diffuseColor, const Graphic::Color &specularColor);

    void removeLight(const Light &light);

    [[nodiscard]] bool hasLight(uint32_t index) const;

    [[nodiscard]] const Graphic::Color &getAmbientLight() const;

    Light& getLight(uint32_t index);

    [[nodiscard]] bool glEnabled() const;

    [[nodiscard]] GlRenderStyle getGlRenderStyle() const;

    void setGlRenderStyle(GlRenderStyle renderStyle);

    [[nodiscard]] GlShadeModel getGlShadeModel() const;

    void setGlShadeModel(GlShadeModel shadeModel);

    [[nodiscard]] bool isLightEnabled() const;

    void setLightEnabled(bool enabled);

    [[nodiscard]] const Graphic::Color &getBackgroundColor() const;

    void setBackgroundColor(const Graphic::Color &backgroundColor);

private:

    Graphic::Color backgroundColor = Graphic::Colors::BLACK;

    bool lightEnabled = true;
    Graphic::Color ambientLight = Graphic::Colors::WHITE.dim();
    Light* lights[16]{};

    GlRenderStyle renderStyle = FILL;
    GlShadeModel shadeModel = SMOOTH;
};

}

#endif
