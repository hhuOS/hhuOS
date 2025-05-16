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

#include "Scene.h"

#include "Entity.h"
#include "lib/util/collection/Pair.h"
#include "lib/util/game/3d/event/CollisionEvent.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/collection/Iterator.h"
#include "lib/util/game/3d/collider/SphereCollider.h"
#include "lib/util/game/Entity.h"
#include "lib/util/game/Graphics.h"
#include "lib/util/base/Exception.h"
#include "lib/util/game/3d/Light.h"

namespace Util {
namespace Math {
template <typename T> class Vector3;
}  // namespace Math
}  // namespace Util

namespace Util::Game::D3 {

void Scene::initializeScene(Graphics &graphics) {
    graphics.initializeGl();
    graphics.clear();

    initialize();

    for (auto *entity : entities) {
        entity->initialize();
    }
}

void Scene::updateEntities(double delta) {
    for (auto *entity : entities) {
        reinterpret_cast<Util::Game::D3::Entity*>(entity)->update(delta);
    }
}

void Scene::checkCollisions() {
    auto detectedCollisions = Util::ArrayList<Pair<Entity*, Entity*>>();

    for (auto *entity : entities) {
        auto *entity3D = reinterpret_cast<D3::Entity*>(entity);

        if (entity3D->hasCollider()) {
            for (auto *otherEntity : entities) {
                auto *otherEntity3D = reinterpret_cast<D3::Entity*>(otherEntity);
                if (entity == otherEntity || !otherEntity3D->hasCollider() || detectedCollisions.contains(Util::Pair(entity3D, otherEntity3D)) || detectedCollisions.contains(Util::Pair(otherEntity3D, entity3D))) {
                    continue;
                }

                if (entity3D->getCollider().isColliding(otherEntity3D->getCollider())) {
                    auto event = CollisionEvent(*otherEntity3D);
                    auto otherEvent = CollisionEvent(*entity3D);

                    entity3D->onCollisionEvent(event);
                    otherEntity3D->onCollisionEvent(otherEvent);

                    detectedCollisions.add(Util::Pair(entity3D, otherEntity3D));
                }
            }
        }
    }
}

void Scene::setAmbientLight(const Graphic::Color &ambientLight) {
    Scene::ambientLight = ambientLight;
}

Light &Scene::addLight(Light::Type type, const Math::Vector3<double> &position, const Graphic::Color &diffuseColor, const Graphic::Color &specularColor) {
    for (uint32_t i = 0; i < 16; i++) {
        if (lights[i] == nullptr) {
            glEnable(GL_LIGHT0 + i);
            lights[i] = new Light(i, type, position, diffuseColor, specularColor);
            return *lights[i];
        }
    }

    Exception::throwException(Util::Exception::OUT_OF_BOUNDS, "Game: Maximum number of lights reached!");
}

void Scene::removeLight(const Light &light) {
    glDisable(GL_LIGHT0 + light.getIndex());
    delete lights[light.getIndex()];
}

bool Scene::hasLight(uint32_t index) const {
    return lights[index] != nullptr;
}

const Graphic::Color &Scene::getAmbientLight() const {
    return ambientLight;
}

D3::Light &Scene::getLight(uint32_t index) {
    if (lights[index] == nullptr) {
        Exception::throwException(Util::Exception::NULL_POINTER, "Scene: Light does not exist!");
    }

    return *lights[index];
}

Scene::GlRenderStyle Scene::getGlRenderStyle() const {
    return renderStyle;
}

void Scene::setGlRenderStyle(GlRenderStyle renderStyle) {
    Scene::renderStyle = renderStyle;
    glPolygonMode(GL_FRONT_AND_BACK, renderStyle);
}

Scene::GlShadeModel Scene::getGlShadeModel() const {
    return shadeModel;
}

void Scene::setGlShadeModel(GlShadeModel shadeModel) {
    Scene::shadeModel = shadeModel;
    glShadeModel(shadeModel);
}

bool Scene::isLightEnabled() const {
    return lightEnabled;
}

void Scene::setLightEnabled(bool enabled) {
    lightEnabled = enabled;
    if (lightEnabled) {
        glEnable(GL_LIGHTING);
    } else {
        glDisable(GL_LIGHTING);
    }
}

const Graphic::Color& Scene::getBackgroundColor() const {
    return backgroundColor;
}

void Scene::setBackgroundColor(const Graphic::Color &backgroundColor) {
    Scene::backgroundColor = backgroundColor;
    glClearColor(backgroundColor.getRed() / 255.0f, backgroundColor.getGreen() / 255.0f, backgroundColor.getBlue() / 255.0f, 1.0f);
}

}
