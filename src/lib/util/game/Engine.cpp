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
 *
 * The game engine is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 *
 * It has been enhanced with 3D-capabilities during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 */

#include "lib/util/async/Thread.h"
#include "MouseListener.h"
#include "Engine.h"
#include "lib/util/io/file/File.h"
#include "lib/util/game/Game.h"
#include "lib/util/game/KeyListener.h"
#include "lib/util/graphic/Ansi.h"
#include "lib/util/graphic/Colors.h"
#include "lib/util/io/stream/FileInputStream.h"
#include "lib/util/io/key/KeyDecoder.h"
#include "lib/util/base/System.h"
#include "lib/util/io/stream/InputStream.h"
#include "GameManager.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/math/Vector2.h"
#include "lib/util/base/String.h"
#include "lib/util/game/Camera.h"
#include "lib/util/game/Scene.h"
#include "lib/util/base/Address.h"
#include "lib/util/io/key/MouseDecoder.h"
#include "lib/util/math/Vector3.h"
#include "lib/util/graphic/BufferedLinearFrameBuffer.h"
#include "lib/util/graphic/Font.h"
#include "lib/util/base/FreeListMemoryManager.h"
#include "lib/util/game/3d/Scene.h"

namespace Util::Game {

Engine::Engine(const Graphic::LinearFrameBuffer &lfb, uint8_t targetFrameRate, double scaleFactor) : graphics(lfb, game, scaleFactor), targetFrameRate(targetFrameRate),
        statisticsFont(Graphic::Font::getFontForResolution(lfb.getResolutionY() * scaleFactor)) {
    GameManager::game = &game;

    auto mouseFile = Io::File("/device/mouse");
    if (mouseFile.exists()) {
        mouseInputStream = new Util::Io::FileInputStream(mouseFile);
    }
}

Engine::~Engine() {
    delete mouseInputStream;
}

void Engine::run() {
    const auto targetFrameTime = Util::Time::Timestamp::ofMicroseconds(static_cast<uint64_t>(1000000.0 / targetFrameRate));

    // Initialize screen and standard input
    Graphic::Ansi::prepareGraphicalApplication(true);
    Util::Io::File::setAccessMode(Util::Io::STANDARD_INPUT, Util::Io::File::NON_BLOCKING);
    mouseInputStream->setAccessMode(Io::File::NON_BLOCKING);

    initializeNextScene();

    while (game.isRunning()) {
        if (game.sceneSwitched) {
            initializeNextScene();
        }

        statistics.startFrameTime();
        statistics.startUpdateTime();
        double frameTime = static_cast<double>(statistics.getLastFrameTime().toMicroseconds()) / 1000000;

        checkKeyboard();
        checkMouse();

        graphics.update();

        auto &scene = game.getCurrentScene();
        scene.update(frameTime);
        scene.updateEntities(frameTime);
        scene.checkCollisions();
        scene.applyChanges();
        updateStatus();
        statistics.stopUpdateTimeTime();

        statistics.startDrawTime();
        scene.draw(graphics);
        if (showStatus) drawStatus();
        graphics.show();
        statistics.stopDrawTime();

        statistics.startIdleTime();
        const auto renderTime = statistics.getLastDrawTime() + statistics.getLastUpdateTime();
        if (renderTime < targetFrameTime) {
            Async::Thread::sleep(targetFrameTime - renderTime);
        }
        statistics.stopIdleTime();

        statistics.stopFrameTime();
    }

    graphics.closeGl();
}

void Engine::initializeNextScene() {
    if (!game.firstScene) {
        graphics.closeGl();
        game.getCurrentScene().getCamera().setPosition(Math::Vector3<double>(0, 0, 0));
        graphics.update();
    }

    auto resolution = GameManager::getAbsoluteResolution();
    auto stringPositionX = static_cast<uint16_t>((resolution.getX() - Util::Address(LOADING).stringLength() * statisticsFont.getCharWidth()) / 2.0);
    auto stringPositionY = static_cast<uint16_t>(resolution.getY() / 2.0);

    graphics.clear();
    graphics.setColor(Graphic::Colors::WHITE);
    graphics.drawStringDirectAbsolute(statisticsFont, stringPositionX, stringPositionY, LOADING);
    graphics.show();

    statistics.reset();

    game.initializeNextScene(graphics);
    game.getCurrentScene().applyChanges();
}

void Engine::updateStatus() {
    statusUpdateTimer += statistics.getLastFrameTime();
    if (statusUpdateTimer > Time::Timestamp::ofSeconds(1)) {
        status = statistics.gather();
        statusUpdateTimer.reset();
    }
}

void Engine::drawStatus() {
    auto charHeight = statisticsFont.getCharHeight() + 2;
    auto color = graphics.getColor();

    const auto &camera = game.getCurrentScene().getCamera();
    const auto &memoryManager = Util::System::getAddressSpaceHeader().memoryManager;
    const auto heapUsed = (memoryManager.getTotalMemory() - memoryManager.getFreeMemory());
    const auto heapUsedM = heapUsed / 1000 / 1000;
    const auto heapUsedK = (heapUsed - heapUsedM * 1000 * 1000) / 1000;

    graphics.setColor(Graphic::Colors::WHITE);
    graphics.drawStringDirectAbsolute(statisticsFont, 10, 10, String::format("FPS: %u | Frame time: %u.%ums", status.framesPerSecond,
            static_cast<uint32_t>(status.frameTime.toMilliseconds()), static_cast<uint32_t>((status.frameTime.toMicroseconds() - status.frameTime.toMilliseconds() * 1000) / 100)));
    graphics.drawStringDirectAbsolute(statisticsFont, 10, 10 + charHeight, String::format("Draw: %u.%ums | Update: %u.%ums | Idle: %u.%ums",
            static_cast<uint32_t>(status.drawTime.toMilliseconds()), static_cast<uint32_t>((status.drawTime.toMicroseconds() - status.drawTime.toMilliseconds() * 1000) / 100),
            static_cast<uint32_t>(status.updateTime.toMilliseconds()), static_cast<uint32_t>((status.updateTime.toMicroseconds() - status.updateTime.toMilliseconds() * 1000) / 100),
            static_cast<uint32_t>(status.idleTime.toMilliseconds()), static_cast<uint32_t>((status.idleTime.toMicroseconds() - status.idleTime.toMilliseconds() * 1000) / 100)));
    graphics.drawStringDirectAbsolute(statisticsFont, 10, 10 + charHeight * 2, String::format("Objects: %u", game.getCurrentScene().getObjectCount()));
    graphics.drawStringDirectAbsolute(statisticsFont, 10, 10 + charHeight * 3, String::format("Heap used: %u.%03u MB", heapUsedM, heapUsedK));
    graphics.drawStringDirectAbsolute(statisticsFont, 10, 10 + charHeight * 4, String::format("Resolution: %ux%u@%u", graphics.bufferedLfb.getResolutionX(), graphics.bufferedLfb.getResolutionY(), graphics.bufferedLfb.getColorDepth()));
    graphics.drawStringDirectAbsolute(statisticsFont, 10, 10 + charHeight * 5, String::format("Camera: Position(%d, %d, %d), Rotation(%d, %d, %d)",
            static_cast<int32_t>(camera.getPosition().getX()), static_cast<int32_t>(camera.getPosition().getY()), static_cast<int32_t>(camera.getPosition().getZ()),
            static_cast<int32_t>(camera.getRotation().getX()), static_cast<int32_t>(camera.getRotation().getY()), static_cast<int32_t>(camera.getRotation().getZ())));
    graphics.setColor(color);
}

void Engine::checkKeyboard() {
    int16_t scancode = Util::System::in.read();
    while (scancode >= 0) {
        if (keyDecoder.parseScancode(scancode)) {
            auto key = keyDecoder.getCurrentKey();
            auto &scene = game.getCurrentScene();

            switch (key.getScancode()) {
                case Io::Key::F1:
                    if (key.isPressed()) {
                        showStatus = !showStatus;
                    }
                    break;
                case Io::Key::F2:
                    if (key.isPressed() && graphics.glEnabled()) {
                        auto &scene3d = reinterpret_cast<D3::Scene&>(scene);
                        switch (scene3d.getGlRenderStyle()) {
                            case D3::Scene::GlRenderStyle::POINTS:
                                scene3d.setGlRenderStyle(D3::Scene::GlRenderStyle::LINES);
                                break;
                            case D3::Scene::GlRenderStyle::LINES:
                                scene3d.setGlRenderStyle(D3::Scene::GlRenderStyle::FILL);
                                break;
                            case D3::Scene::GlRenderStyle::FILL:
                                scene3d.setGlRenderStyle(D3::Scene::GlRenderStyle::POINTS);
                                break;
                        }
                    }
                    break;
                case Io::Key::F3:
                    if (key.isPressed() && graphics.glEnabled()) {
                        auto &scene3d = reinterpret_cast<D3::Scene&>(scene);
                        scene3d.setLightEnabled(!scene3d.isLightEnabled());
                    }
                break;
                case Io::Key::F4:
                    if (key.isPressed() && graphics.glEnabled()) {
                        auto &scene3d = reinterpret_cast<D3::Scene&>(scene);
                        switch (scene3d.getGlShadeModel()) {
                            case D3::Scene::GlShadeModel::FLAT:
                                scene3d.setGlShadeModel(D3::Scene::GlShadeModel::SMOOTH);
                                break;
                            case D3::Scene::GlShadeModel::SMOOTH:
                                scene3d.setGlShadeModel(D3::Scene::GlShadeModel::FLAT);
                                break;
                        }
                    }
                break;
                default:
                    if (scene.keyListener != nullptr) {
                        key.isPressed() ? scene.keyListener->keyPressed(key) : scene.keyListener->keyReleased(key);
                    }
            }
        }

        scancode = Util::System::in.read();
    }
}

void Engine::checkMouse() {
    int16_t value = mouseInputStream->read();
    while (value >= 0) {
        mouseValues[mouseValueIndex++] = value;
        if (mouseValueIndex == 4) {
            auto mouseUpdate = Io::MouseDecoder::decode(mouseValues);

            auto &scene = game.getCurrentScene();
            if (scene.mouseListener == nullptr) {
                continue;
            }

            checkMouseKey(Io::Mouse::LEFT_BUTTON, lastMouseButtonState, mouseUpdate.buttons);
            checkMouseKey(Io::Mouse::RIGHT_BUTTON, lastMouseButtonState, mouseUpdate.buttons);
            checkMouseKey(Io::Mouse::MIDDLE_BUTTON, lastMouseButtonState, mouseUpdate.buttons);
            checkMouseKey(Io::Mouse::BUTTON_4, lastMouseButtonState, mouseUpdate.buttons);
            checkMouseKey(Io::Mouse::BUTTON_5, lastMouseButtonState, mouseUpdate.buttons);
            lastMouseButtonState = mouseUpdate.buttons;

            if (mouseUpdate.xMovement != 0 || mouseUpdate.yMovement != 0) {
                scene.mouseListener->mouseMoved(Util::Math::Vector2<double>(mouseUpdate.xMovement / static_cast<double>(UINT8_MAX), mouseUpdate.yMovement / static_cast<double>(UINT8_MAX)));
            }

            if (mouseUpdate.scroll != 0) {
                scene.mouseListener->mouseScrolled(mouseUpdate.scroll);
            }

            Util::Address(mouseValues).setRange(0, 4);
            mouseValueIndex = 0;
        }

        value = mouseInputStream->read();
    }
}

void Engine::checkMouseKey(Io::Mouse::Button button, uint8_t lastButtonState, uint8_t currentButtonState) {
    auto &scene = game.getCurrentScene();
    if (!(lastButtonState & button) && (currentButtonState & button)) {
        scene.mouseListener->buttonPressed(button);
    } else if ((lastButtonState & button) && !(currentButtonState & button)) {
        scene.mouseListener->buttonReleased(button);
    }
}

}