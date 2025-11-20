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

#include "Engine.h"

#include "pulsar/audio/AudioRunnable.h"
#include "pulsar/3d/Scene.h"
#include "util/graphic/Ansi.h"

namespace Pulsar {

Engine::Engine(const Util::Graphic::LinearFrameBuffer &lfb, const uint8_t targetFrameRate, const double scaleFactor) :
    graphics(lfb, scaleFactor),
    game(graphics.getTransformation(), graphics.getDimensions()),
    targetFrameRate(targetFrameRate)
{
    Game::instance = &game;
}

void Engine::run() {
    const auto targetFrameTime = Util::Time::Timestamp::ofMicroseconds(
        static_cast<uint64_t>(1000000.0 / targetFrameRate));

    // Initialize screen and keyboard/mouse input streams
    Util::Graphic::Ansi::prepareGraphicalApplication(true);
    Util::Io::File::setAccessMode(Util::Io::STANDARD_INPUT, Util::Io::File::NON_BLOCKING);

    const auto mouseFile = Util::Io::File("/device/mouse");
    auto mouseInputStream = Util::Io::FileInputStream(mouseFile.exists() ?
        mouseFile : Util::Io::File("/device/null"));
    mouseInputStream.setAccessMode(Util::Io::File::NON_BLOCKING);

    // Start audio thread
    auto *audioRunnable = new AudioRunnable(game.audioChannels);
    const auto audioThread = Util::Async::Thread::createThread("Game-Audio", audioRunnable);

    // Initialize first scene
    initializeNextScene();

    // Enter game loop
    game.running = true;
    while (game.isRunning()) {
        // Check if we need to switch to the next scene
        if (game.sceneSwitched) {
            initializeNextScene();
        }

        statistics.startFrameTime();

        // Update game logic (input handling, entity updates, collision checks, etc.)
        statistics.startUpdateTime();

        checkKeyboard();
        checkMouse(mouseInputStream);

        graphics.update();

        auto &scene = game.getCurrentScene();
        const auto lastFrameTime = statistics.getLastFrameTime().toSecondsFloat<double>();
        scene.update(lastFrameTime);
        scene.updateEntities(lastFrameTime);
        scene.checkCollisions();
        scene.applyChanges();

        updateStatus();
        statistics.stopUpdateTimeTime();

        // Render the current scene
        statistics.startDrawTime();
        scene.draw(graphics);

        if (showStatus) {
            drawStatus();
        }

        graphics.show();
        statistics.stopDrawTime();

        // If the frame was processed faster than the target frame time, idle for the remaining time
        statistics.startIdleTime();
        const auto renderTime = statistics.getLastDrawTime() + statistics.getLastUpdateTime();
        if (renderTime < targetFrameTime) {
            Util::Async::Thread::sleep(targetFrameTime - renderTime);
        }
        statistics.stopIdleTime();

        statistics.stopFrameTime();
    }

    // Cleanup after exiting the game loop
    audioRunnable->stop();
    audioThread.join();

    graphics.disableGl();
}

void Engine::initializeNextScene() {
    // Stop audio playback
    game.stopAllAudioChannels();

    // Reset graphics
    if (!game.firstScene) {
        graphics.disableGl();
        game.getCurrentScene().getCamera().reset();
        graphics.update();
    }

    // Display loading message
    const double stringLength = Util::Address(LOADING).stringLength();
    const auto stringPos = Util::Math::Vector2<double>(
        -stringLength * graphics.getRelativeFontSize() / 2, -graphics.getRelativeFontSize() / 2);

    graphics.clear();
    graphics.setColor(Util::Graphic::Colors::WHITE);
    graphics.drawStringDirect(stringPos, LOADING);
    graphics.show();

    // Reset statistics
    statistics = Statistics();

    // Initialize next scene
    game.initializeNextScene(graphics);
    game.getCurrentScene().applyChanges();
}

void Engine::updateStatus() {
    statusUpdateTimer += statistics.getLastFrameTime();
    if (statusUpdateTimer > Util::Time::Timestamp::ofSeconds(1)) {
        status = statistics.gather();
        statusUpdateTimer = Util::Time::Timestamp();
    }
}

void Engine::drawStatus() {
    const auto color = graphics.getColor();
    const auto &camera = game.getCurrentScene().getCamera();
    const auto &memoryManager = Util::System::getAddressSpaceHeader().heapMemoryManager;
    const auto heapUsed = memoryManager.getTotalMemory() - memoryManager.getFreeMemory();
    const auto heapUsedM = heapUsed / 1000 / 1000;
    const auto heapUsedK = (heapUsed - heapUsedM * 1000 * 1000) / 1000;

    graphics.setColor(Util::Graphic::Colors::WHITE);

    graphics.drawStringDirectAbsolute(10, 10,
        Util::String::format("FPS: %u | Frame time: %.1fms",
            status.framesPerSecond,
            status.frameTime.toSecondsFloat<float>() * 1000));

    graphics.drawStringDirectAbsolute(10, 10 + Graphics::FONT_SIZE,
        Util::String::format("Draw: %.1fms | Update: %.1fms | Idle: %.1fms",
            status.drawTime.toSecondsFloat<float>() * 1000,
            status.updateTime.toSecondsFloat<float>() * 1000,
            status.idleTime.toSecondsFloat<float>() * 1000));

    graphics.drawStringDirectAbsolute(10, 10 + Graphics::FONT_SIZE * 2,
        Util::String::format("Objects: %u", game.getCurrentScene().getObjectCount()));

    graphics.drawStringDirectAbsolute(10, 10 + Graphics::FONT_SIZE * 3,
        Util::String::format("Heap used: %u.%03u MB", heapUsedM, heapUsedK));

    graphics.drawStringDirectAbsolute(10, 10 + Graphics::FONT_SIZE * 4,
        Util::String::format("Resolution: %ux%u@%u",
            graphics.bufferedLfb.getResolutionX(),
            graphics.bufferedLfb.getResolutionY(),
            graphics.bufferedLfb.getColorDepth()));

    if (graphics.isGlEnabled()) {
        const auto &cameraPosition = camera.getPosition();
        const auto &cameraRotation = camera.getRotation();
        const auto &cameraFront = camera.getFrontVector();
        const auto &cameraUp = camera.getUpVector();
        const auto &cameraRight = camera.getRightVector();

        graphics.drawStringDirectAbsolute(10, 10 + Graphics::FONT_SIZE * 5,
            Util::String::format("Camera: Position(%.1f, %.1f, %.1f), Rotation(%.1f, %.1f, %.1f)",
                cameraPosition.getX(), cameraPosition.getY(), cameraPosition.getZ(),
                cameraRotation.getX(), cameraRotation.getY(), cameraRotation.getZ()));


        graphics.drawStringDirectAbsolute(10, 10 + Graphics::FONT_SIZE * 6,
           Util::String::format("        Front(%.1f, %.1f, %.1f), Up(%.1f, %.1f, %.1f), Right(%.1f, %.1f, %.1f)",
                cameraFront.getX(), cameraFront.getY(), cameraFront.getZ(),
                cameraUp.getX(), cameraUp.getY(), cameraUp.getZ(),
                cameraRight.getX(), cameraRight.getY(), cameraRight.getZ()));
    } else {
        const auto &cameraPosition = camera.getPosition();

        graphics.drawStringDirectAbsolute(10, 10 + Graphics::FONT_SIZE * 5,
            Util::String::format("Camera: Position(%.1f, %.1f)", cameraPosition.getX(), cameraPosition.getY()));
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
                // Toggle status display
                case Util::Io::Key::F1:
                    if (key.isPressed()) {
                        showStatus = !showStatus;
                    }
                    break;
                // Toggle OpenGL render style (3D scenes only)
                case Util::Io::Key::F2:
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
                // Toggle lighting (3D scenes only)
                case Util::Io::Key::F3:
                    if (key.isPressed() && graphics.isGlEnabled()) {
                        auto &scene3d = reinterpret_cast<D3::Scene&>(scene);
                        scene3d.setLightingEnabled(!scene3d.isLightingEnabled());
                    }
                break;
                // Toggle shading model (3D scenes only)
                case Util::Io::Key::F4:
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
                // Forward key event to the current scene
                default:
                    key.isPressed() ? scene.keyPressed(key) : scene.keyReleased(key);
            }
        }

        scancode = Util::System::in.read();
    }
}

void Engine::checkMouse(Util::Io::FileInputStream &mouseInputStream) {
    int16_t value = mouseInputStream.read();
    while (value >= 0) {
        mouseValues[mouseValueIndex++] = value;
        if (mouseValueIndex == 4) {
            const auto mouseUpdate = Util::Io::MouseDecoder::decode(mouseValues);

            checkMouseKey(Util::Io::MouseDecoder::LEFT_BUTTON,
                lastMouseButtons, mouseUpdate.buttons);
            checkMouseKey(Util::Io::MouseDecoder::RIGHT_BUTTON,
                lastMouseButtons, mouseUpdate.buttons);
            checkMouseKey(Util::Io::MouseDecoder::MIDDLE_BUTTON,
                lastMouseButtons, mouseUpdate.buttons);
            checkMouseKey(Util::Io::MouseDecoder::BUTTON_4,
                lastMouseButtons, mouseUpdate.buttons);
            checkMouseKey(Util::Io::MouseDecoder::BUTTON_5,
                lastMouseButtons, mouseUpdate.buttons);

            lastMouseButtons = mouseUpdate.buttons;

            if (mouseUpdate.xMovement != 0 || mouseUpdate.yMovement != 0) {
                const auto movement = Util::Math::Vector2<double>(
                    mouseUpdate.xMovement / static_cast<double>(UINT8_MAX),
                    mouseUpdate.yMovement / static_cast<double>(UINT8_MAX));
                game.getCurrentScene().mouseMoved(movement);
            }

            if (mouseUpdate.scroll != 0) {
                game.getCurrentScene().mouseScrolled(mouseUpdate.scroll);
            }

            Util::Address(mouseValues).setRange(0, 4);
            mouseValueIndex = 0;
        }

        value = mouseInputStream.read();
    }
}

void Engine::checkMouseKey(const Util::Io::MouseDecoder::Button button,
    const uint8_t lastButtonState, const uint8_t currentButtonState) const
{
    auto &scene = game.getCurrentScene();
    if (!(lastButtonState & button) && currentButtonState & button) {
        scene.mouseButtonPressed(button);
    } else if (lastButtonState & button && !(currentButtonState & button)) {
        scene.mouseButtonReleased(button);
    }
}

}
