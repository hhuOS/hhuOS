/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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

#ifndef HHUOS_LIB_PULSAR_3D_SCENE_H
#define HHUOS_LIB_PULSAR_3D_SCENE_H

#include "util/graphic/Color.h"
#include "util/graphic/Colors.h"
#include "util/math/Vector3.h"
#include "pulsar/Scene.h"
#include "pulsar/3d/Entity.h"
#include "pulsar/3d/Light.h"
#include "tinygl/include/GL/gl.h"

namespace Pulsar {
namespace D3 {

/// Base class for 3D scenes used in the game engine.
/// It extends the base `Pulsar::Scene` class with 3D-specific behaviors (e.g. lighting).
/// 3D scenes manage 3D entities with sphere colliders and check for collisions between them.
/// They also manage OpenGL specific settings like render styles and shading models, which
/// can be adjusted at runtime:
/// - Press F2 to toggle between OpenGL render styles (points, lines, fill).
/// - Press F3 to toggle lighting on and off.
/// - Press F4 to toggle between flat and smooth shading models.
///
/// To create a new 3D scene for your game, derive from this class and implement the required methods:
/// - `initialize()`: Initialize the scene (load resources, set up entities, etc.).
/// - `update(float delta)`: Update the scene logic (optional).
/// - `keyPressed(const Util::Io::Key &key)`: Handle key press events (optional).
/// - `keyReleased(const Util::Io::Key &key)`: Handle key release events (optional).
/// - `mouseButtonPressed(Util::Io::MouseDecoder::Button button)`: Handle mouse button press events (optional).
/// - `mouseButtonReleased(Util::Io::MouseDecoder::Button button)`: Handle mouse button release events (optional).
/// - `mouseMoved(const Util::Math::Vector2<float> &relativeMovement)`: Handle mouse movement events (optional).
/// - `mouseScrolled(float scrollAmount)`: Handle mouse scroll events (optional).
class Scene : public Pulsar::Scene {

public:
    /// OpenGL render styles for drawing polygons.
    /// The default render style is `FILL`.
    enum GlRenderStyle : GLint {
        /// Render polygons as points, with only the vertices drawn (no edges or faces).
        POINTS = GL_POINT,
        /// Render polygons as wireframes, with only the edges drawn (no faces).
        LINES = GL_LINE,
        /// Render filled polygons, with faces drawn solidly.
        FILL = GL_FILL
    };

    /// OpenGL shading models for rendering polygons.
    /// The default shading model is `SMOOTH`.
    enum GlShadeModel : GLint {
        /// Render polygons with flat shading, where each polygon has a single color.
        FLAT = GL_FLAT,
        /// Render polygons with smooth shading, where colors are interpolated across vertices.
        SMOOTH = GL_SMOOTH
    };

    /// Create a new 3D scene instance.
    Scene() = default;

    /// Perform a raytrace and return the first entity hit by the ray.
    /// The ray starts at the 'from' position and extends in the 'direction' for the given 'length'.
    /// The 'precision' parameter defines the step size for the raytrace.
    /// If no entity is hit, nullptr is returned.
    Entity* findEntityUsingRaytrace(const Util::Math::Vector3<float> &from,
        const Util::Math::Vector3<float> &direction, float length, float precision = 0.1) const;

    /// Set the ambient light color for the scene.
    /// This color is applied globally to all objects in the scene.
    void setAmbientLight(const Util::Graphic::Color &ambientLight);

    /// Add a new light to the scene and return a reference to it for later modification or removal.
    /// A maximum of 16 lights can be added to the scene.
    /// If the maximum number of lights is reached, a panic is fired.
    Light& addLight(Light::Type type, const Util::Math::Vector3<float> &position,
        const Util::Graphic::Color &diffuseColor, const Util::Graphic::Color &specularColor);

    /// Remove the given light from the scene.
    void removeLight(const Light &light);

    /// Access the array of lights in the scene.
    /// This method is used by the `Graphics` class to manage OpenGL lights.
    /// It probably has no practical use for game developers.
    const Util::Array<Light>& getLights() const {
        return lights;
    }

    /// Get the ambient light color of the scene.
    const Util::Graphic::Color& getAmbientLight() const {
        return ambientLight;
    }

    /// Get the current OpenGL render style of the scene.
    GlRenderStyle getGlRenderStyle() const {
        return renderStyle;
    }

    /// Set the OpenGL render style of the scene.
    void setGlRenderStyle(GlRenderStyle renderStyle);

    /// Get the current OpenGL shading model of the scene.
    GlShadeModel getGlShadeModel() const {
        return shadeModel;
    }

    /// Set the OpenGL shading model of the scene.
    void setGlShadeModel(GlShadeModel shadeModel);

    /// Check if lighting is enabled in the scene.
    bool isLightingEnabled() const {
        return lightEnabled;
    }

    /// Enable or disable lighting in the scene.
    void setLightingEnabled(bool enabled);

    /// Get the background color of the scene (OpenGL clear color).
    const Util::Graphic::Color& getBackgroundColor() const {
        return backgroundColor;
    }

    /// Set the background color of the scene (OpenGL clear color).
    void setBackgroundColor(const Util::Graphic::Color &backgroundColor);

    /// Initialize the scene. This method is called by the engine once when the scene is first loaded.
    /// It initializes the graphics context, calls the user-defined `initialize()` method
    /// and initializes all entities in the scene.
    /// It is not intended to be called directly by game developers.
    void initializeScene(Graphics &graphics) final;

    /// Update all entities in the scene. This method is called by the engine once per frame.
    /// It is not intended to be called directly by game developers.
    void updateEntities(float delta) final;

    /// Check for collisions between entities in the scene. This method is called by the engine once per frame.
    /// It checks for collisions between entities that have sphere colliders.
    /// When a collision is detected, a `CollisionEvent` is created and dispatched to both entities involved in the
    /// collision. It is not intended to be called directly by game developers.
    void checkCollisions() final;

private:

    Util::Graphic::Color backgroundColor = Util::Graphic::Colors::BLACK;

    bool lightEnabled = true;
    Util::Graphic::Color ambientLight = Util::Graphic::Colors::WHITE.dim();
    Util::Array<Light> lights = Util::Array<Light>(16);

    GlRenderStyle renderStyle = FILL;
    GlShadeModel shadeModel = SMOOTH;
};

}
}

#endif
