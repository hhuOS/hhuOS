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
#include "lib/util/game/3d/Light.h"
#include "lib/util/graphic/Colors.h"

ModelViewer::ModelViewer(const Util::String &path) : modelPath(path) {}

void ModelViewer::initialize() {
    light = &addLight(Util::Game::D3::Light::POINT, camera.getPosition(), Util::Graphic::Colors::WHITE, Util::Graphic::Colors::WHITE);

    model = new ModelEntity(modelPath);
    model->setPosition(camera.getTargetVector() * 5);
    addObject(model);

    setKeyListener(*this);
}

void ModelViewer::update(double delta) {
    camera.rotate(cameraRotation * delta * 50);
    camera.translate(cameraTranslation * delta * 10);

    model->rotate(modelRotation * delta * 50);
    light->setPosition(camera.getPosition());
}

void ModelViewer::keyPressed(const Util::Io::Key &key) {
    switch (key.getScancode()) {
        case Util::Io::Key::LEFT:
            cameraRotation = Util::Math::Vector3<double>(cameraRotation.getX(), cameraRotation.getY(), -1);
            break;
        case Util::Io::Key::RIGHT:
            cameraRotation = Util::Math::Vector3<double>(cameraRotation.getX(), cameraRotation.getY(), 1);
            break;
        case Util::Io::Key::UP:
            cameraRotation = Util::Math::Vector3<double>(cameraRotation.getX(), -1, cameraRotation.getZ());
            break;
        case Util::Io::Key::DOWN:
            cameraRotation = Util::Math::Vector3<double>(cameraRotation.getX(), 1, cameraRotation.getZ());
            break;
        case Util::Io::Key::W:
            cameraTranslation = Util::Math::Vector3<double>(0, 1, 0);
            break;
        case Util::Io::Key::S:
            cameraTranslation = Util::Math::Vector3<double>(0, -1, 0);
            break;
        case Util::Io::Key::A:
            cameraTranslation = camera.getRightVector() * -1;
            break;
        case Util::Io::Key::D:
            cameraTranslation = camera.getRightVector();
            break;
        case Util::Io::Key::E:
            cameraTranslation = camera.getTargetVector();
            break;
        case Util::Io::Key::Q:
            cameraTranslation = camera.getTargetVector() * -1;
            break;
        case Util::Io::Key::O:
            modelRotation = Util::Math::Vector3<double>(-1, modelRotation.getY(), modelRotation.getZ());
            break;
        case Util::Io::Key::U:
            modelRotation = Util::Math::Vector3<double>(1, modelRotation.getY(), modelRotation.getZ());
            break;
        case Util::Io::Key::I:
            modelRotation = Util::Math::Vector3<double>(modelRotation.getX(), -1, modelRotation.getZ());
            break;
        case Util::Io::Key::K:
            modelRotation = Util::Math::Vector3<double>(modelRotation.getX(), 1, modelRotation.getZ());
            break;
        case Util::Io::Key::J:
            modelRotation = Util::Math::Vector3<double>(modelRotation.getX(), modelRotation.getY(), -1);
            break;
        case Util::Io::Key::L:
            modelRotation = Util::Math::Vector3<double>(modelRotation.getX(), modelRotation.getY(), 1);
            break;
        case Util::Io::Key::SPACE:
            camera.reset();
            camera.setPosition(Util::Math::Vector3<double>(0, 0, 0.0));
            model->setPosition(camera.getTargetVector() * 5);
            model->setRotation(Util::Math::Vector3<double>(0, 0, 0));
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
        case Util::Io::Key::LEFT:
        case Util::Io::Key::RIGHT:
            cameraRotation = Util::Math::Vector3<double>(cameraRotation.getX(), cameraRotation.getY(), 0);
            break;
        case Util::Io::Key::UP:
        case Util::Io::Key::DOWN:
            cameraRotation = Util::Math::Vector3<double>(cameraRotation.getX(), 0, cameraRotation.getZ());
            break;
        case Util::Io::Key::W:
            cameraTranslation = cameraTranslation - Util::Math::Vector3<double>(0, 1, 0);
            break;
        case Util::Io::Key::S:
            cameraTranslation = cameraTranslation + Util::Math::Vector3<double>(0, 1, 0);
            break;
        case Util::Io::Key::A:
            cameraTranslation = cameraTranslation + camera.getRightVector();
            break;
        case Util::Io::Key::D:
            cameraTranslation = cameraTranslation - camera.getRightVector();
            break;
        case Util::Io::Key::E:
            cameraTranslation = cameraTranslation - camera.getTargetVector();
            break;
        case Util::Io::Key::Q:
            cameraTranslation = cameraTranslation + camera.getTargetVector();
            break;
        case Util::Io::Key::O:
        case Util::Io::Key::U:
            modelRotation = Util::Math::Vector3<double>(0, modelRotation.getY(), modelRotation.getZ());
            break;
        case Util::Io::Key::I:
        case Util::Io::Key::K:
            modelRotation = Util::Math::Vector3<double>(modelRotation.getX(), 0, modelRotation.getZ());
            break;
        case Util::Io::Key::J:
        case Util::Io::Key::L:
            modelRotation = Util::Math::Vector3<double>(modelRotation.getX(), modelRotation.getY(), 0);
            break;
        default:
            break;
    }
}
