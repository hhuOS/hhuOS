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
 * The network stack is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-maseh100
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
#include "lib/util/graphic/Color.h"
#include "lib/util/graphic/Font.h"

namespace Util::Game {

Engine::Engine(const Util::Graphic::LinearFrameBuffer &lfb, const uint8_t targetFrameRate) : graphics(lfb, game), targetFrameRate(targetFrameRate) {
    GameManager::transformation = (lfb.getResolutionX() > lfb.getResolutionY() ? lfb.getResolutionY() : lfb.getResolutionX()) / 2;
    GameManager::game = &game;
}

void Engine::run() {
    const auto delta = 1.0 / targetFrameRate;
    const auto deltaMilliseconds = static_cast<uint32_t>(delta * 1000);

    Graphic::Ansi::prepareGraphicalApplication(true);
    graphics.setColor(Graphic::Colors::WHITE);
    graphics.drawString(Math::Vector2D(-0.075, 0), "Loading...");
    graphics.show();

    Async::Thread::createThread("Key-Listener", new KeyListenerRunnable(*this));
    Async::Thread::createThread("Mouse-Listener", new MouseListenerRunnable(*this));

    game.initializeNextScene(graphics);
    game.getCurrentScene().applyChanges();

    while (game.isRunning()) {
        statistics.startFrameTime();
        statistics.startUpdateTime();
        double frameTime = statistics.getLastFrameTime() / 1000.0;
        if (frameTime < 0.001) {
            frameTime = 0.001;
        }

        updateLock.acquire();
        if (game.sceneSwitched) {
            game.initializeNextScene(graphics);
        }

        auto &scene = game.getCurrentScene();
        scene.update(frameTime);
        scene.updateEntities(frameTime);
        scene.checkCollisions();
        scene.applyChanges();
        updateStatus();
        statistics.stopUpdateTimeTime();
        updateLock.release();

        statistics.startDrawTime();
        scene.draw(graphics);
        if (showStatus) drawStatus();
        graphics.show();
        statistics.stopDrawTime();

        const auto drawTime = statistics.getLastDrawTime();
        const auto updateTime = statistics.getLastUpdateTime();
        if (drawTime + updateTime < deltaMilliseconds) {
            statistics.startIdleTime();
            Async::Thread::sleep(Time::Timestamp::ofMilliseconds(deltaMilliseconds - (drawTime + updateTime)));
            statistics.stopIdleTime();
        }

        statistics.incFrames();
        statistics.stopFrameTime();
    }

    Graphic::Ansi::cleanupGraphicalApplication();
}

void Engine::updateStatus() {
    statusUpdateTimer += statistics.getLastFrameTime();
    if (statusUpdateTimer > 1000) {
        status = statistics.gather();
        statusUpdateTimer = 0;
    }
}

void Engine::drawStatus() {
    auto cameraPosition = game.getCurrentScene().getCamera().getPosition();
    auto charHeight = ((Graphic::Fonts::TERMINAL_FONT_SMALL.getCharHeight() + 2) / graphics.getAbsoluteResolution().getY()) * 2;
    auto charWidth = ((Graphic::Fonts::TERMINAL_FONT_SMALL.getCharWidth()) / graphics.getAbsoluteResolution().getX()) * 2;
    auto color = graphics.getColor();

    const auto &memoryManager = *reinterpret_cast<HeapMemoryManager*>(USER_SPACE_MEMORY_MANAGER_ADDRESS);
    auto heapUsed = (memoryManager.getTotalMemory() - memoryManager.getFreeMemory());
    auto heapUsedM = heapUsed / 1000 / 1000;
    auto heapUsedK = (heapUsed - heapUsedM * 1000 * 1000) / 1000;

    graphics.setColor(Graphic::Color(50, 50, 50, 100));
    graphics.fillRectangle(Math::Vector2D(cameraPosition.getX() - 1, cameraPosition.getY() + 1 - charHeight / 2), charWidth * 41.5, charHeight * 4.5);

    auto x = cameraPosition.getX() - 1 + charWidth;
    auto y = 1 - charHeight;

    graphics.setColor(Graphic::Colors::WHITE);
    graphics.drawStringSmall(Math::Vector2D(x, y), String::format("FPS: %u", status.fps));
    graphics.drawStringSmall(Math::Vector2D(x, y - charHeight), String::format("D: %ums | U: %ums | I: %ums", status.drawTime, status.updateTime, status.idleTime));
    graphics.drawStringSmall(Math::Vector2D(x, y - charHeight * 2), String::format("Objects: %u", game.getCurrentScene().getObjectCount()));
    graphics.drawStringSmall(Math::Vector2D(x, y - charHeight * 3), String::format("Heap used: %u.%03u MB", heapUsedM, heapUsedK));
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

            if (scene.keyListener != nullptr) {
                engine.updateLock.acquire();

                switch (key.getScancode()) {
                    case Io::Key::F1 :
                        if (key.isPressed()) engine.showStatus = !engine.showStatus;
                        break;
                    default:
                        key.isPressed() ? scene.keyListener->keyPressed(key) : scene.keyListener->keyReleased(key);
                }

                engine.updateLock.release();
            }
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

    uint8_t lastButtons = 0;

    auto stream = Io::FileInputStream(file);
    while (engine.game.isRunning()) {
        auto buttons = stream.read();
        auto xMovement = static_cast<int8_t>(stream.read());
        auto yMovement = static_cast<int8_t>(stream.read());
        auto &scene = engine.game.getCurrentScene();

        if (scene.mouseListener == nullptr) {
            continue;
        }

        engine.updateLock.acquire();
        checkKey(MouseListener::LEFT, lastButtons, buttons);
        checkKey(MouseListener::RIGHT, lastButtons, buttons);
        checkKey(MouseListener::MIDDLE, lastButtons, buttons);
        lastButtons = buttons;

        if (xMovement != 0 || yMovement != 0) {
            scene.mouseListener->mouseMoved(Util::Math::Vector2D(xMovement / static_cast<double>(INT8_MAX), -yMovement / static_cast<double>(INT8_MAX)));
        }
        engine.updateLock.release();
    }
}

void Engine::MouseListenerRunnable::checkKey(MouseListener::Key key, uint8_t lastButtonState, uint8_t currentButtonState) {
    auto &scene = engine.game.getCurrentScene();
    if (!(lastButtonState & key) && (currentButtonState & key)) {
        scene.mouseListener->keyPressed(key);
    } else if ((lastButtonState & key) && !(currentButtonState & key)) {
        scene.mouseListener->keyReleased(key);
    }
}

}