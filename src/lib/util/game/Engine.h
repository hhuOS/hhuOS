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

#ifndef HHUOS_ENGINE_H
#define HHUOS_ENGINE_H

#include <cstdint>

#include "lib/util/async/Runnable.h"
#include "lib/util/time/Timestamp.h"
#include "lib/util/async/Spinlock.h"
#include "lib/util/game/Graphics2D.h"
#include "lib/util/game/MouseListener.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/io/key/Key.h"
#include "lib/util/collection/Array.h"
#include "lib/util/collection/Collection.h"
#include "lib/util/collection/Iterator.h"
#include "Game.h"

namespace Util {
namespace Graphic {
class LinearFrameBuffer;
}  // namespace Graphic
}  // namespace Util

namespace Util::Game {

class Engine : public Util::Async::Runnable {

public:
    /**
     * Default Constructor.
     */
    Engine(const Util::Graphic::LinearFrameBuffer &lfb, const uint8_t targetFrameRate);

    /**
     * Copy Constructor.
     */
    Engine(const Engine &other) = delete;

    /**
     * Assignment operator.
     */
    Engine &operator=(const Engine &other) = delete;

    /**
     * Destructor.
     */
    ~Engine() override = default;

    void run() override;

private:

    struct Statistics {

        struct Gather {
            uint32_t fps;
            uint32_t frameTime;
            uint32_t drawTime;
            uint32_t updateTime;
            uint32_t idleTime;
        };

        void incFrames() {
            frames++;
        }

        void startFrameTime() {
            frameTimeStart = Time::getSystemTime().toMilliseconds();
        }

        void stopFrameTime() {
            const uint32_t frameTime = Time::getSystemTime().toMilliseconds() - frameTimeStart;
            frameTimes[frameTimesIndex++ % ARRAY_SIZE] = frameTime;

            timeCounter += frameTime;
            if (timeCounter >= 1000) {
                fps = frames;
                frames = 0;
                timeCounter = 0;
            }
        }

        void startDrawTime() {
            drawTimeStart = Time::getSystemTime().toMilliseconds();
        }

        void stopDrawTime() {
            drawTimes[drawTimesIndex++ % ARRAY_SIZE] = Time::getSystemTime().toMilliseconds() - drawTimeStart;
        }

        void startUpdateTime() {
            updateTimeStart = Time::getSystemTime().toMilliseconds();
        }

        void stopUpdateTimeTime() {
            updateTimes[updateTimesIndex++ % ARRAY_SIZE] = Time::getSystemTime().toMilliseconds() - updateTimeStart;
        }

        void startIdleTime() {
            idleTimeStart = Time::getSystemTime().toMilliseconds();
        }

        void stopIdleTime() {
            idleTimes[idleTimesIndex++ % ARRAY_SIZE] = Time::getSystemTime().toMilliseconds() - idleTimeStart;
        }

        uint32_t getLastFrameTime() {
            const auto index = frameTimesIndex % ARRAY_SIZE;
            return frameTimes[index == 0 ? ARRAY_SIZE - 1 : index - 1];
        }

        uint32_t getLastDrawTime() {
            const auto index = drawTimesIndex % ARRAY_SIZE;
            return drawTimes[index == 0 ? ARRAY_SIZE - 1 : index - 1];
        }

        uint32_t getLastUpdateTime() {
            const auto index = updateTimesIndex % ARRAY_SIZE;
            return updateTimes[index == 0 ? ARRAY_SIZE - 1 : index - 1];
        }

        [[nodiscard]] Gather gather() {
            Gather gather{};
            uint32_t count = frameTimesIndex < ARRAY_SIZE ? frameTimesIndex : ARRAY_SIZE;

            if (count > 0) {
                for (uint32_t i = 0; i < count; i++) {
                    gather.frameTime += frameTimes[i];
                    gather.drawTime += drawTimes[i];
                    gather.updateTime += updateTimes[i];
                    gather.idleTime += idleTimes[i];
                }

                gather.fps = fps;
                gather.frameTime /= count;
                gather.drawTime /= count;
                gather.updateTime /= count;
                gather.idleTime /= count;
            }

            return gather;
        }

    private:
        static const constexpr uint32_t ARRAY_SIZE = 100;

        uint32_t frames = 0;
        uint32_t fps = 0;
        uint32_t timeCounter = 0;

        uint32_t frameTimes[ARRAY_SIZE]{};
        uint32_t drawTimes[ARRAY_SIZE]{};
        uint32_t updateTimes[ARRAY_SIZE]{};
        uint32_t idleTimes[ARRAY_SIZE]{};

        uint32_t frameTimesIndex = 0;
        uint32_t drawTimesIndex = 0;
        uint32_t updateTimesIndex = 0;
        uint32_t idleTimesIndex = 0;

        uint32_t frameTimeStart = 0;
        uint32_t drawTimeStart = 0;
        uint32_t updateTimeStart = 0;
        uint32_t idleTimeStart = 0;
    };

    class KeyListenerRunnable : public Async::Runnable {

    public:
        /**
         * Constructor.
         */
        explicit KeyListenerRunnable(Engine &engine);

        /**
         * Copy Constructor.
         */
        KeyListenerRunnable(const KeyListenerRunnable &other) = delete;

        /**
         * Assignment operator.
         */
        KeyListenerRunnable &operator=(const KeyListenerRunnable &other) = delete;

        /**
         * Destructor.
         */
        ~KeyListenerRunnable() override = default;

        void run() override;

    private:

        Engine &engine;
    };

    class MouseListenerRunnable : public Async::Runnable {

    public:
        /**
         * Constructor.
         */
        explicit MouseListenerRunnable(Engine &engine);

        /**
         * Copy Constructor.
         */
        MouseListenerRunnable(const MouseListenerRunnable &other) = delete;

        /**
         * Assignment operator.
         */
        MouseListenerRunnable &operator=(const MouseListenerRunnable &other) = delete;

        /**
         * Destructor.
         */
        ~MouseListenerRunnable() override = default;

        void run() override;

    private:

        void checkKey(MouseListener::Key key, uint8_t lastButtonState, uint8_t currentButtonState);

        Engine &engine;
    };

    void updateStatus();

    void drawStatus();

    Game game;
    Graphics2D graphics;
    Statistics statistics;
    Async::Spinlock updateLock;

    bool showStatus = false;
    uint32_t statusUpdateTimer = 0;
    Statistics::Gather status = statistics.gather();

    Util::ArrayList<Io::Key> pressedKeys;

    const uint8_t targetFrameRate;
};

}

#endif
