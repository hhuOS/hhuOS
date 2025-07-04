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
#include "lib/util/math/Vector2.h"
#include "lib/util/base/String.h"
#include "lib/util/game/Camera.h"
#include "lib/util/game/Scene.h"
#include "lib/util/base/Address.h"
#include "lib/util/io/key/MouseDecoder.h"
#include "lib/util/math/Vector3.h"
#include "lib/util/graphic/BufferedLinearFrameBuffer.h"
#include "lib/util/base/FreeListMemoryManager.h"
#include "lib/util/game/3d/Scene.h"

namespace Util::Game {

Engine::Engine(const Graphic::LinearFrameBuffer &lfb, uint8_t targetFrameRate, double scaleFactor) : graphics(lfb, game, scaleFactor), targetFrameRate(targetFrameRate) {
    GameManager::game = &game;
    GameManager::graphics = &graphics;

    auto mouseFile = Io::File("/device/mouse");
    if (mouseFile.exists()) {
        mouseInputStream = new Io::FileInputStream(mouseFile);
    }
}

Engine::~Engine() {
    delete mouseInputStream;
}

void Engine::run() {
    const auto targetFrameTime = Time::Timestamp::ofMicroseconds(static_cast<uint64_t>(1000000.0 / targetFrameRate));

    // Initialize screen and standard input
    Graphic::Ansi::prepareGraphicalApplication(true);
    Io::File::setAccessMode(Io::STANDARD_INPUT, Util::Io::File::NON_BLOCKING);
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

        for (auto &audioChannel : game.audioChannels) {
            if (audioChannel.isPlaying()) {
                audioChannel.update(frameTime);
            }
        }

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

    graphics.disableGl();
}

void Engine::initializeNextScene() {
    game.stopAllAudioChannels();

    if (!game.firstScene) {
        graphics.disableGl();
        game.getCurrentScene().getCamera().reset();
        graphics.update();
    }

    const double stringLength = Address(LOADING).stringLength();
    const auto stringPos = Math::Vector2<double>(-stringLength * graphics.getRelativeFontSize() / 2, -graphics.getRelativeFontSize() / 2);

    graphics.clear();
    graphics.setColor(Graphic::Colors::WHITE);
    graphics.drawStringDirect(stringPos, LOADING);
    graphics.show();

    statistics.reset();

    game.initializeNextScene(graphics);
    game.getCurrentScene().applyChanges();
}

void Engine::updateStatus() {
    statusUpdateTimer += statistics.getLastFrameTime();
    if (statusUpdateTimer > Time::Timestamp::ofSeconds(1)) {
        status = statistics.gather();
        statusUpdateTimer = Time::Timestamp();
    }
}

void Engine::drawStatus() {
    const auto color = graphics.getColor();
    const auto &camera = game.getCurrentScene().getCamera();
    const auto &memoryManager = Util::System::getAddressSpaceHeader().heapMemoryManager;
    const auto heapUsed = (memoryManager.getTotalMemory() - memoryManager.getFreeMemory());
    const auto heapUsedM = heapUsed / 1000 / 1000;
    const auto heapUsedK = (heapUsed - heapUsedM * 1000 * 1000) / 1000;

    graphics.setColor(Graphic::Colors::WHITE);
    graphics.drawStringDirectAbsolute(10, 10, String::format("FPS: %u | Frame time: %u.%ums", status.framesPerSecond,
            static_cast<uint32_t>(status.frameTime.toMilliseconds()), static_cast<uint32_t>((status.frameTime.toMicroseconds() - status.frameTime.toMilliseconds() * 1000) / 100)));
    graphics.drawStringDirectAbsolute(10, 10 + Graphics::FONT_SIZE, String::format("Draw: %u.%ums | Update: %u.%ums | Idle: %u.%ums",
            static_cast<uint32_t>(status.drawTime.toMilliseconds()), static_cast<uint32_t>((status.drawTime.toMicroseconds() - status.drawTime.toMilliseconds() * 1000) / 100),
            static_cast<uint32_t>(status.updateTime.toMilliseconds()), static_cast<uint32_t>((status.updateTime.toMicroseconds() - status.updateTime.toMilliseconds() * 1000) / 100),
            static_cast<uint32_t>(status.idleTime.toMilliseconds()), static_cast<uint32_t>((status.idleTime.toMicroseconds() - status.idleTime.toMilliseconds() * 1000) / 100)));
    graphics.drawStringDirectAbsolute(10, 10 + Graphics::FONT_SIZE * 2, String::format("Objects: %u", game.getCurrentScene().getObjectCount()));
    graphics.drawStringDirectAbsolute(10, 10 + Graphics::FONT_SIZE * 3, String::format("Heap used: %u.%03u MB", heapUsedM, heapUsedK));
    graphics.drawStringDirectAbsolute(10, 10 + Graphics::FONT_SIZE * 4, String::format("Resolution: %ux%u@%u", graphics.bufferedLfb.getResolutionX(), graphics.bufferedLfb.getResolutionY(), graphics.bufferedLfb.getColorDepth()));

    if (graphics.isGlEnabled()) {
        graphics.drawStringDirectAbsolute(10, 10 + Graphics::FONT_SIZE * 5, String::format("Camera: Position(%d, %d, %d), Rotation(%d, %d, %d)",
                static_cast<int32_t>(camera.getPosition().getX()), static_cast<int32_t>(camera.getPosition().getY()), static_cast<int32_t>(camera.getPosition().getZ()),
                static_cast<int32_t>(camera.getRotation().getX()), static_cast<int32_t>(camera.getRotation().getY()), static_cast<int32_t>(camera.getRotation().getZ())));
        graphics.drawStringDirectAbsolute(10, 10 + Graphics::FONT_SIZE * 6, String::format("Camera: Front(%d, %d, %d), Up(%d, %d, %d), Right(%d, %d, %d)",
                static_cast<int32_t>(camera.getFrontVector().getX() * 100), static_cast<int32_t>(camera.getFrontVector().getY() * 100), static_cast<int32_t>(camera.getFrontVector().getZ() * 100),
                static_cast<int32_t>(camera.getUpVector().getX() * 100), static_cast<int32_t>(camera.getUpVector().getY() * 100), static_cast<int32_t>(camera.getUpVector().getZ() * 100),
                static_cast<int32_t>(camera.getRightVector().getX() * 100), static_cast<int32_t>(camera.getRightVector().getY() * 100), static_cast<int32_t>(camera.getRightVector().getZ() * 100)));
    } else {
        graphics.drawStringDirectAbsolute(10, 10 + Graphics::FONT_SIZE * 5, String::format("Camera: Position(%d, %d)",
                static_cast<int32_t>(camera.getPosition().getX()), static_cast<int32_t>(camera.getPosition().getY())));
    }

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
                    if (key.isPressed() && graphics.isGlEnabled()) {
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
                    if (key.isPressed() && graphics.isGlEnabled()) {
                        auto &scene3d = reinterpret_cast<D3::Scene&>(scene);
                        scene3d.setLightEnabled(!scene3d.isLightEnabled());
                    }
                break;
                case Io::Key::F4:
                    if (key.isPressed() && graphics.isGlEnabled()) {
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