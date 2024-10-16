/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "ModelViewer.h"

#include "lib/util/game/Game.h"
#include "lib/util/game/GameManager.h"
#include "application/view3d/ModelEntity.h"
#include "lib/util/game/Camera.h"
#include "lib/util/io/key/Key.h"
#include "lib/util/io/key/MouseDecoder.h"
#include "lib/util/math/Vector2D.h"

ModelViewer::ModelViewer(const Util::String &path) : modelPath(path) {}

void ModelViewer::initialize() {
    model = new ModelEntity(modelPath);
    addObject(model);

    setKeyListener(*this);
    setMouseListener(*this);
}

void ModelViewer::update(double delta) {
    camera.translateLocal(Util::Math::Vector3D(0, 0, zoom * delta));
    model->rotate(rotation * delta * 50);
}

void ModelViewer::keyPressed(const Util::Io::Key &key) {
    switch (key.getScancode()) {
        case Util::Io::Key::W:
            if (rotation.getX() == 0) rotation = rotation - Util::Math::Vector3D(1, 0, 0);
            break;
        case Util::Io::Key::S:
            if (rotation.getX() == 0) rotation = rotation + Util::Math::Vector3D(1, 0, 0);
            break;
        case Util::Io::Key::A:
            if (rotation.getY() == 0) rotation = rotation - Util::Math::Vector3D(0, 1, 0);
            break;
        case Util::Io::Key::D:
            if (rotation.getY() == 0) rotation = rotation + Util::Math::Vector3D(0, 1, 0);
            break;
        case Util::Io::Key::E:
            if (rotation.getZ() == 0) rotation = rotation - Util::Math::Vector3D(0, 0, 1);
            break;
        case Util::Io::Key::Q:
            if (rotation.getZ() == 0) rotation = rotation + Util::Math::Vector3D(0, 0, 1);
            break;
        case Util::Io::Key::PLUS:
            if (zoom == 0) zoom = 1;
            break;
        case Util::Io::Key::MINUS:
            if (zoom == 0) zoom = -1;
            break;
        case Util::Io::Key::SPACE:
            model->setPosition(Util::Math::Vector3D(0, 0, 3));
            model->setRotation(Util::Math::Vector3D(0, 0, 0));
            camera.setRotation(Util::Math::Vector3D(0, 0, 0));
            camera.setPosition(Util::Math::Vector3D(0, 0, 0));
            break;
        case Util::Io::Key::ESC:
            Util::Game::GameManager::getGame().stop();
            break;
        default:
            break;
    }

}

void ModelViewer::keyReleased(const Util::Io::Key &key) {
    switch (key.getScancode()) {
        case Util::Io::Key::W:
            if (rotation.getX() == -1) rotation = rotation + Util::Math::Vector3D(1, 0, 0);
            break;
        case Util::Io::Key::S:
            if (rotation.getX() == 1) rotation = rotation - Util::Math::Vector3D(1, 0, 0);
            break;
        case Util::Io::Key::A:
            if (rotation.getY() == -1) rotation = rotation + Util::Math::Vector3D(0, 1, 0);
            break;
        case Util::Io::Key::D:
            if (rotation.getY() == 1) rotation = rotation - Util::Math::Vector3D(0, 1, 0);
            break;
        case Util::Io::Key::E:
            if (rotation.getZ() == -1) rotation = rotation + Util::Math::Vector3D(0, 0, 1);
            break;
        case Util::Io::Key::Q:
            if (rotation.getZ() == 1) rotation = rotation - Util::Math::Vector3D(0, 0, 1);
            break;
        case Util::Io::Key::PLUS:
            if (zoom == 1) zoom = 0;
            break;
        case Util::Io::Key::MINUS:
            if (zoom == -1) zoom = 0;
            break;
        default:
            break;
    }
}

void ModelViewer::mouseMoved(const Util::Math::Vector2D &relativeMovement) {
    camera.rotate(Util::Math::Vector3D(-relativeMovement.getY() * 10, relativeMovement.getX() * 10, 0));
}

void ModelViewer::mouseScrolled(Util::Io::Mouse::ScrollDirection direction) {
    switch (direction) {
        case Util::Io::Mouse::UP:
            camera.translateLocal(Util::Math::Vector3D(0, 0, 0.1));
            break;
        case Util::Io::Mouse::DOWN:
            camera.translateLocal(Util::Math::Vector3D(0, 0, -0.1));
            break;
        default:
            break;
    }
}

void ModelViewer::buttonPressed([[maybe_unused]] Util::Io::Mouse::Button button) {}

void ModelViewer::buttonReleased([[maybe_unused]] Util::Io::Mouse::Button button) {}
