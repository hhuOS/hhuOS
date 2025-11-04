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
 * The OpenGL demo has been created during a bachelor's thesis by Kevin Weber
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-keweb100
 */

#include "OpenGlDemo.h"

#include "Cuboid.h"
#include "DemoModel.h"
#include "Rectangle.h"
#include "lib/util/pulsar/Game.h"
#include "lib/util/math/Vector2.h"
#include "lib/util/base/String.h"
#include "lib/util/pulsar/3d/Light.h"
#include "lib/util/pulsar/Camera.h"
#include "lib/util/graphic/Color.h"
#include "lib/util/graphic/Colors.h"
#include "lib/util/io/key/Key.h"

void OpenGlDemo::initialize() {
    setBackgroundColor(Util::Graphic::Color(176, 252, 255));

    setAmbientLight(Util::Graphic::Color(77, 77, 77));
    addLight(Util::Pulsar::D3::Light::POINT, Util::Math::Vector3<double>(0, 1, 1), Util::Graphic::Color(255, 255, 255), Util::Graphic::Color(0, 0, 0));
    addLight(Util::Pulsar::D3::Light::POINT, Util::Math::Vector3<double>(39, 20, 36), Util::Graphic::Color(102, 102, 255), Util::Graphic::Color(0, 0, 0));


    getCamera().setPosition(Util::Math::Vector3<double>(0, 0, 5));

    auto *floor1 = new Cuboid(Util::Math::Vector3<double>(0, -5.5, 0), Util::Math::Vector3<double>(0, 0, 0), Util::Math::Vector3<double>(0, 0, 0), Util::Math::Vector3<double>(20, 1, 20), Util::Graphic::Color(0, 255, 0));
    auto *floor2 = new Cuboid(Util::Math::Vector3<double>(30, -5.5, 0), Util::Math::Vector3<double>(0, 0, 0), Util::Math::Vector3<double>(0, 0, 0), Util::Math::Vector3<double>(20, 1, 20), Util::Graphic::Color(0, 255, 0));

    auto *cuboid1 = new Cuboid(Util::Math::Vector3<double>(0, 0, 0), Util::Math::Vector3<double>(0, 0, 0), Util::Math::Vector3<double>(0, 0, 0), Util::Math::Vector3<double>(1, 1, 1), Util::Graphic::Color(230, 230, 230));
    auto *cuboid2 = new Cuboid(Util::Math::Vector3<double>(40.0, -4.0, 5.0), Util::Math::Vector3<double>(0, 0, 0), Util::Math::Vector3<double>(0, 0, 0), Util::Math::Vector3<double>(3, 1, 1), Util::Graphic::Color(255, 230, 230));
    auto *cuboid3 = new Cuboid(Util::Math::Vector3<double>(-5.0, -2.0, 10.0), Util::Math::Vector3<double>(0, 0, 0), Util::Math::Vector3<double>(0, 0, 0), Util::Math::Vector3<double>(1, 3, 1), Util::Graphic::Color(204, 51, 51));
    auto *cuboid4 = new Cuboid(Util::Math::Vector3<double>(0, 7, 0), Util::Math::Vector3<double>(0, 0, 0), Util::Math::Vector3<double>(1, 1, 1), Util::Math::Vector3<double>(1, 1, 1), Util::Graphic::Color(0, 255, 0));
    auto *cuboid5 = new Cuboid(Util::Math::Vector3<double>(10, 0, 0), Util::Math::Vector3<double>(2,50,30), Util::Math::Vector3<double>(0, 0, 0), Util::Math::Vector3<double>(3, 2,2), Util::Graphic::Color(0, 255, 0));

    auto *texturedCube = new Cuboid(Util::Math::Vector3<double>(-10, 0, 5.5), Util::Math::Vector3<double>(45,45,0), Util::Math::Vector3<double>(0, 0, 0), Util::Math::Vector3<double>(1, 1,1), "/user/dino/block/box.bmp");

    auto *tree = new DemoModel(DemoModel::TREE, Util::Math::Vector3<double>(25, -5, 0), Util::Math::Vector3<double>(0, 0, -90), Util::Math::Vector3<double>(10, 10, 10), Util::Graphic::Colors::WHITE);
    auto *lantern = new DemoModel(DemoModel::LANTERN, Util::Math::Vector3<double>(3, -5, -2), Util::Math::Vector3<double>(0, 0, 0), Util::Math::Vector3<double>(7, 7, 7), Util::Graphic::Colors::BROWN);
    auto *icosphere = new DemoModel(DemoModel::ICOSPHERE, Util::Math::Vector3<double>(35, 20, -2), Util::Math::Vector3<double>(0, 0, 0), Util::Math::Vector3<double>(5, 5, 5));

    auto *logo = new Rectangle(Util::Math::Vector3<double>(15, 5, -15), Util::Math::Vector3<double>(0, 0, 0), Util::Math::Vector3<double>(0, 0, 0), Util::Math::Vector2<double>(10, 2.62), "/user/demo/hhu.bmp");

    addEntity(floor1);
    addEntity(floor2);
    addEntity(cuboid1);
    addEntity(cuboid2);
    addEntity(cuboid3);
    addEntity(cuboid4);
    addEntity(cuboid5);
    addEntity(texturedCube);
    addEntity(tree);
    addEntity(lantern);
    addEntity(icosphere);
    addEntity(logo);
}

void OpenGlDemo::update([[maybe_unused]] double delta) {
    auto &camera = getCamera();
    auto translation = Util::Math::Vector3<double>(0, 0, 0);
    if (cameraTranslation.getZ() != 0) {
        translation = translation + camera.getFrontVector() * cameraTranslation.getZ();
    }
    if (cameraTranslation.getX() != 0) {
        translation = translation + camera.getRightVector() * cameraTranslation.getX();
    }

    camera.rotate(cameraRotation * delta * 50);
    camera.translate(translation * delta * 10);
}

void OpenGlDemo::keyPressed(const Util::Io::Key &key) {
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
        case Util::Io::Key::SPACE:
            getCamera().reset();
            getCamera().setPosition(Util::Math::Vector3<double>(0, 0, 5));
            break;
        case Util::Io::Key::ESC:
            Util::Pulsar::Game::getInstance().stop();
            break;
        default:
            break;
    }
}

void OpenGlDemo::keyReleased(const Util::Io::Key &key) {
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
        default:
            break;
    }
}
