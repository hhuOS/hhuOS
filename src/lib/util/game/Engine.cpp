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
#include "lib/util/math/Vector2D.h"
#include "lib/util/base/HeapMemoryManager.h"
#include "lib/util/base/Constants.h"
#include "lib/util/base/String.h"
#include "lib/util/game/Camera.h"
#include "lib/util/game/Scene.h"
#include "lib/util/base/Address.h"
#include "lib/util/io/key/MouseDecoder.h"
#include "lib/util/math/Vector3D.h"
#include "lib/util/graphic/BufferedLinearFrameBuffer.h"

namespace Util::Game {

Engine::Engine(const Util::Graphic::LinearFrameBuffer &lfb, const uint8_t targetFrameRate) : graphics(lfb, game), targetFrameRate(targetFrameRate) {
    GameManager::absoluteResolution = Math::Vector2D(lfb.getResolutionX(), lfb.getResolutionY());
    GameManager::relativeResolution = Math::Vector2D(lfb.getResolutionX() > lfb.getResolutionY() ? (double) lfb.getResolutionX() / lfb.getResolutionY() : 1,
                                                     lfb.getResolutionY() > lfb.getResolutionX() ? (double) lfb.getResolutionY() / lfb.getResolutionX() : 1);
    GameManager::transformation = (lfb.getResolutionX() > lfb.getResolutionY() ? lfb.getResolutionY() : lfb.getResolutionX()) / 2;
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
    const auto delta = 1.0 / targetFrameRate;
    const auto deltaMilliseconds = static_cast<uint32_t>(delta * 1000);

    Graphic::Ansi::prepareGraphicalApplication(true);
    initializeNextScene();

    Util::Io::File::setAccessMode(Util::Io::STANDARD_INPUT, Util::Io::File::NON_BLOCKING);
    mouseInputStream->setAccessMode(Io::File::NON_BLOCKING);

    while (game.isRunning()) {
        if (game.sceneSwitched) {
            initializeNextScene();
        }

        statistics.startFrameTime();
        statistics.startUpdateTime();
        double frameTime = statistics.getLastFrameTime() / 1000.0;

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
}

void Engine::initializeNextScene() {
    if (!game.firstScene) {
        game.getCurrentScene().getCamera().setPosition(Math::Vector3D(0, 0, 0));
    }

    auto resolution = GameManager::getAbsoluteResolution();
    auto stringPosition = Math::Vector2D((resolution.getX() - Util::Address<uint32_t>(LOADING).stringLength() * loadingFont.getCharWidth()) / 2.0, resolution.getY() / 2.0);

    graphics.clear();
    graphics.setColor(Graphic::Colors::WHITE);
    graphics.drawString(loadingFont, stringPosition, LOADING);
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
    auto charHeight = statisticsFont.getCharHeight() + 2;
    auto color = graphics.getColor();

    const auto &memoryManager = *reinterpret_cast<HeapMemoryManager*>(USER_SPACE_MEMORY_MANAGER_ADDRESS);
    auto heapUsed = (memoryManager.getTotalMemory() - memoryManager.getFreeMemory());
    auto heapUsedM = heapUsed / 1000 / 1000;
    auto heapUsedK = (heapUsed - heapUsedM * 1000 * 1000) / 1000;

    graphics.setColor(Graphic::Colors::WHITE);
    graphics.drawString(statisticsFont, Math::Vector2D(10, 10), String::format("FPS: %u", status.fps));
    graphics.drawString(statisticsFont, Math::Vector2D(10, 10 + charHeight), String::format("D: %ums | U: %ums | I: %ums", status.drawTime, status.updateTime, status.idleTime));
    graphics.drawString(statisticsFont, Math::Vector2D(10, 10 + charHeight * 2), String::format("Objects: %u | Edges: %u/%u", game.getCurrentScene().getObjectCount(), graphics.drawnEdgeCounter, graphics.edgeCounter));
    graphics.drawString(statisticsFont, Math::Vector2D(10, 10 + charHeight * 3), String::format("Heap used: %u.%03u MB", heapUsedM, heapUsedK));
    graphics.drawString(statisticsFont, Math::Vector2D(10, 10 + charHeight * 4), String::format("Resolution: %ux%u@%u", graphics.lfb.getResolutionX(), graphics.lfb.getResolutionY(), graphics.lfb.getColorDepth()));
    graphics.setColor(color);
}

void Engine::checkKeyboard() {
    int16_t scancode = Util::System::in.read();
    while (scancode >= 0) {
        if (keyDecoder.parseScancode(scancode)) {
            auto key = keyDecoder.getCurrentKey();
            auto &scene = game.getCurrentScene();

            switch (key.getScancode()) {
                case Io::Key::F1 :
                    if (key.isPressed()) showStatus = !showStatus;
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
                scene.mouseListener->mouseMoved(Util::Math::Vector2D(mouseUpdate.xMovement / static_cast<double>(UINT8_MAX), mouseUpdate.yMovement / static_cast<double>(UINT8_MAX)));
            }

            if (mouseUpdate.scroll != 0) {
                scene.mouseListener->mouseScrolled(mouseUpdate.scroll);
            }

            Util::Address<uint32_t>(mouseValues).setRange(0, 4);
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