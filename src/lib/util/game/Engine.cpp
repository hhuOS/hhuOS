/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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
#include "lib/util/math/Vector2D.h"
#include "lib/util/base/HeapMemoryManager.h"
#include "lib/util/base/Constants.h"
#include "lib/util/graphic/Fonts.h"
#include "lib/util/base/String.h"
#include "lib/util/game/Camera.h"
#include "lib/util/game/Scene.h"
#include "lib/util/graphic/Font.h"
#include "lib/util/base/Address.h"
#include "lib/util/io/key/MouseDecoder.h"

namespace Util::Game {

Engine::Engine(const Util::Graphic::LinearFrameBuffer &lfb, const uint8_t targetFrameRate) : graphics(lfb, game), targetFrameRate(targetFrameRate) {
    GameManager::absoluteResolution = Math::Vector2D(lfb.getResolutionX(), lfb.getResolutionY());
    GameManager::relativeResolution = Math::Vector2D(lfb.getResolutionX() > lfb.getResolutionY() ? (double) lfb.getResolutionX() / lfb.getResolutionY() : 1,
                                                     lfb.getResolutionY() > lfb.getResolutionX() ? (double) lfb.getResolutionY() / lfb.getResolutionX() : 1);
    GameManager::transformation = (lfb.getResolutionX() > lfb.getResolutionY() ? lfb.getResolutionY() : lfb.getResolutionX()) / 2;
    GameManager::game = &game;
}

void Engine::run() {
    const auto delta = 1.0 / targetFrameRate;
    const auto deltaMilliseconds = static_cast<uint32_t>(delta * 1000);

    Graphic::Ansi::prepareGraphicalApplication(true);
    initializeNextScene();

    Async::Thread::createThread("Key-Listener", new KeyListenerRunnable(*this));
    Async::Thread::createThread("Mouse-Listener", new MouseListenerRunnable(*this));

    while (game.isRunning()) {
        statistics.startFrameTime();
        statistics.startUpdateTime();
        double frameTime = statistics.getLastFrameTime() / 1000.0;
        if (frameTime < 0.001) {
            frameTime = 0.001;
        }

        updateLock.acquire();
        if (game.sceneSwitched) {
            initializeNextScene();
        }

        graphics.update();

        auto &scene = game.getCurrentScene();
        scene.update(frameTime);
        scene.updateEntities(frameTime);
        scene.checkCollisions();
        scene.applyChanges();
        updateStatus();
        statistics.stopUpdateTimeTime();
        updateLock.release();

        statistics.startDrawTime();
        graphics.resetCounters();
        scene.draw(graphics);
        if (showStatus) drawStatus();
        graphics.show();
        statistics.stopDrawTime();

        statistics.startIdleTime();
        const auto drawTime = statistics.getLastDrawTime();
        const auto updateTime = statistics.getLastUpdateTime();
        if (drawTime + updateTime < deltaMilliseconds) {
            Async::Thread::sleep(Time::Timestamp::ofMilliseconds(deltaMilliseconds - (drawTime + updateTime)));
        }
        statistics.stopIdleTime();

        statistics.incFrames();
        statistics.stopFrameTime();
    }

    Graphic::Ansi::cleanupGraphicalApplication();
}

void Engine::initializeNextScene() {
    if (!game.firstScene) {
        game.getCurrentScene().getCamera().setPosition(Math::Vector3D(0, 0, 0));
    }

    auto resolution = GameManager::getAbsoluteResolution();
    auto stringPosition = Math::Vector2D((resolution.getX() - Util::Address<uint32_t>(LOADING).stringLength() * graphics.getCharWidth()) / 2.0, resolution.getY() / 2.0);

    graphics.clear();
    graphics.setColor(Graphic::Colors::WHITE);
    graphics.drawString(stringPosition, LOADING);
    graphics.show();

    game.initializeNextScene(graphics);
    game.getCurrentScene().applyChanges();
}

void Engine::updateStatus() {
    statusUpdateTimer += statistics.getLastFrameTime();
    if (statusUpdateTimer > 1000) {
        status = statistics.gather();
        statusUpdateTimer = 0;
    }
}

void Engine::drawStatus() {
    auto charHeight = graphics.getCharHeightSmall() + 2;
    auto color = graphics.getColor();

    const auto &memoryManager = *reinterpret_cast<HeapMemoryManager*>(USER_SPACE_MEMORY_MANAGER_ADDRESS);
    auto heapUsed = (memoryManager.getTotalMemory() - memoryManager.getFreeMemory());
    auto heapUsedM = heapUsed / 1000 / 1000;
    auto heapUsedK = (heapUsed - heapUsedM * 1000 * 1000) / 1000;

    graphics.setColor(Graphic::Colors::WHITE);
    graphics.drawStringSmall(Math::Vector2D(10, 10), String::format("FPS: %u", status.fps));
    graphics.drawStringSmall(Math::Vector2D(10, 10 + charHeight), String::format("D: %ums | U: %ums | I: %ums", status.drawTime, status.updateTime, status.idleTime));
    graphics.drawStringSmall(Math::Vector2D(10, 10 + charHeight * 2), String::format("Objects: %u | Edges: %u/%u", game.getCurrentScene().getObjectCount(), graphics.drawnEdgeCounter, graphics.edgeCounter));
    graphics.drawStringSmall(Math::Vector2D(10, 10 + charHeight * 3), String::format("Heap used: %u.%03u MB", heapUsedM, heapUsedK));
    graphics.setColor(color);
}

Engine::KeyListenerRunnable::KeyListenerRunnable(Engine &engine) : engine(engine) {}

void Engine::KeyListenerRunnable::run() {
    auto keyDecoder = Io::KeyDecoder();
    int16_t scancode = Util::System::in.read();

    while (engine.game.isRunning() && scancode != -1) {
        if (keyDecoder.parseScancode(scancode)) {
            auto key = keyDecoder.getCurrentKey();
            auto &scene = engine.game.getCurrentScene();

                engine.updateLock.acquire();
                switch (key.getScancode()) {
                    case Io::Key::F1 :
                        if (key.isPressed()) engine.showStatus = !engine.showStatus;
                        break;
                    default:
                        if (scene.keyListener != nullptr) {
                            key.isPressed() ? scene.keyListener->keyPressed(key) : scene.keyListener->keyReleased(key);
                        }
                }
                engine.updateLock.release();
            }

        scancode = System::in.read();
    }
}

Engine::MouseListenerRunnable::MouseListenerRunnable(Engine &engine) : engine(engine) {}

void Engine::MouseListenerRunnable::run() {
    auto file = Io::File("/device/mouse");
    if (!file.exists()) {
        return;
    }

    uint8_t values[4]{};
    uint8_t lastButtons = 0;

    auto stream = Io::FileInputStream(file);
    while (engine.game.isRunning()) {
        values[0] = stream.read();
        values[1] = stream.read();
        values[2] = stream.read();
        values[3] = stream.read();

        auto mouseUpdate = Io::MouseDecoder::decode(values);

        auto &scene = engine.game.getCurrentScene();
        if (scene.mouseListener == nullptr) {
            continue;
        }

        engine.updateLock.acquire();
        checkKey(Io::Mouse::LEFT_BUTTON, lastButtons, mouseUpdate.buttons);
        checkKey(Io::Mouse::RIGHT_BUTTON, lastButtons, mouseUpdate.buttons);
        checkKey(Io::Mouse::MIDDLE_BUTTON, lastButtons, mouseUpdate.buttons);
        checkKey(Io::Mouse::BUTTON_4, lastButtons, mouseUpdate.buttons);
        checkKey(Io::Mouse::BUTTON_5, lastButtons, mouseUpdate.buttons);
        lastButtons = mouseUpdate.buttons;

        if (mouseUpdate.xMovement != 0 || mouseUpdate.yMovement != 0) {
            scene.mouseListener->mouseMoved(Util::Math::Vector2D(mouseUpdate.xMovement / static_cast<double>(UINT8_MAX), mouseUpdate.yMovement / static_cast<double>(UINT8_MAX)));
        }

        if (mouseUpdate.scroll != 0) {
            scene.mouseListener->mouseScrolled(mouseUpdate.scroll);
        }

        engine.updateLock.release();
    }
}

void Engine::MouseListenerRunnable::checkKey(Io::Mouse::Button button, uint8_t lastButtonState, uint8_t currentButtonState) {
    auto &scene = engine.game.getCurrentScene();
    if (!(lastButtonState & button) && (currentButtonState & button)) {
        scene.mouseListener->buttonPressed(button);
    } else if ((lastButtonState & button) && !(currentButtonState & button)) {
        scene.mouseListener->buttonReleased(button);
    }
}

}