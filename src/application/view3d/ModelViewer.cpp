/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "lib/util/io/file/File.h"
#include "lib/util/game/Game.h"
#include "lib/util/game/GameManager.h"
#include "application/view3d/ModelEntity.h"
#include "lib/util/game/Camera.h"
#include "lib/util/io/key/Key.h"
#include "lib/util/game/3d/Light.h"
#include "lib/util/graphic/Colors.h"

ModelViewer::ModelViewer(const Util::String &path) : modelPath(path) {}

void ModelViewer::initialize() {
    const auto texturePath = modelPath.split(".")[0] + ".bmp";
    auto textureFile = Util::Io::File(texturePath);
    if (textureFile.exists()) {
        model = new ModelEntity(modelPath, texturePath);
    } else {
        model = new ModelEntity(modelPath);
    }

    model->setPosition(camera.getFrontVector() * 5);
    addObject(model);

    setAmbientLight(Util::Graphic::Colors::WHITE);
    light = &addLight(Util::Game::D3::Light::POINT, camera.getPosition(), Util::Graphic::Color(255, 255, 255), Util::Graphic::Color(255, 255, 255));

    setKeyListener(*this);
}

void ModelViewer::update(double delta) {
    auto translation = Util::Math::Vector3<double>(0, 0, 0);
    if (cameraTranslation.getZ() != 0) {
        translation = translation + camera.getFrontVector() * cameraTranslation.getZ();
    }
    if (cameraTranslation.getX() != 0) {
        translation = translation + camera.getRightVector() * cameraTranslation.getX();
    }

    camera.rotate(cameraRotation * delta * 50);
    camera.translate(translation * delta * 10);

    light->setPosition(camera.getPosition());

    model->rotate(modelRotation * delta * 50);
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
            cameraRotation = Util::Math::Vector3<double>(cameraRotation.getX(), 1, cameraRotation.getZ());
        break;
        case Util::Io::Key::DOWN:
            cameraRotation = Util::Math::Vector3<double>(cameraRotation.getX(), -1, cameraRotation.getZ());
        break;
        case Util::Io::Key::W:
            cameraTranslation = Util::Math::Vector3<double>(cameraTranslation.getX(), cameraTranslation.getY(), 1);
        break;
        case Util::Io::Key::S:
            cameraTranslation = Util::Math::Vector3<double>(cameraTranslation.getX(), cameraTranslation.getY(), -1);
        break;
        case Util::Io::Key::A:
            cameraTranslation = Util::Math::Vector3<double>(-1, cameraTranslation.getY(), cameraTranslation.getZ());
        break;
        case Util::Io::Key::D:
            cameraTranslation = Util::Math::Vector3<double>(1, cameraTranslation.getY(), cameraTranslation.getZ());
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
            model->setPosition(camera.getFrontVector() * 5);
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
        case Util::Io::Key::S:
            cameraTranslation = Util::Math::Vector3<double>(cameraTranslation.getX(), cameraTranslation.getY(), 0);
        break;
        case Util::Io::Key::A:
        case Util::Io::Key::D:
            cameraTranslation = Util::Math::Vector3<double>(0, cameraTranslation.getY(), cameraTranslation.getZ());
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
