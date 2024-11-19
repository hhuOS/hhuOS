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

#ifndef HHUOS_ENGINE_H
#define HHUOS_ENGINE_H

#include <stdint.h>

#include "lib/util/async/Runnable.h"
#include "lib/util/time/Timestamp.h"
#include "lib/util/game/Graphics.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/io/key/Key.h"
#include "Game.h"
#include "lib/util/io/key/KeyDecoder.h"
#include "lib/util/io/key/layout/DeLayout.h"

namespace Util {
namespace Io {
class FileInputStream;

namespace Mouse {
enum Button : uint8_t;
}  // namespace Mouse
}  // namespace Io

namespace Graphic {
class LinearFrameBuffer;
class Font;
}  // namespace Graphic
}  // namespace Util

namespace Util::Game {

class Engine : public Async::Runnable {

public:
    /**
     * Constructor.
     */
    Engine(const Graphic::LinearFrameBuffer &lfb, uint8_t targetFrameRate, double scaleFactor);

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
    ~Engine() override;

    void run() override;

private:

    struct Statistics {

        struct Gather {
            uint32_t framesPerSecond = 0;
            Time::Timestamp frameTime;
            Time::Timestamp drawTime;
            Time::Timestamp updateTime;
            Time::Timestamp idleTime;
        };

        void startFrameTime() {
            frameTimeStart = Time::getSystemTime();
        }

        void stopFrameTime() {
            const auto frameTime = Time::getSystemTime() - frameTimeStart;
            frameTimes[frameTimesIndex++ % ARRAY_SIZE] = frameTime;
            frames++;

            timeCounter += frameTime;
            if (timeCounter >= Time::Timestamp::ofSeconds(1)) {
                framesPerSecond = frames;
                frames = 0;
                timeCounter.reset();
            }
        }

        void startDrawTime() {
            drawTimeStart = Time::getSystemTime();
        }

        void stopDrawTime() {
            drawTimes[drawTimesIndex++ % ARRAY_SIZE] = Time::getSystemTime() - drawTimeStart;
        }

        void startUpdateTime() {
            updateTimeStart = Time::getSystemTime();
        }

        void stopUpdateTimeTime() {
            updateTimes[updateTimesIndex++ % ARRAY_SIZE] = Time::getSystemTime() - updateTimeStart;
        }

        void startIdleTime() {
            idleTimeStart = Time::getSystemTime();
        }

        void stopIdleTime() {
            idleTimes[idleTimesIndex++ % ARRAY_SIZE] = Time::getSystemTime() - idleTimeStart;
        }

        [[nodiscard]] const Time::Timestamp& getLastFrameTime() const {
            const auto index = frameTimesIndex % ARRAY_SIZE;
            return frameTimes[index == 0 ? ARRAY_SIZE - 1 : index - 1];
        }

        [[nodiscard]] const Time::Timestamp& getLastDrawTime() const {
            const auto index = drawTimesIndex % ARRAY_SIZE;
            return drawTimes[index == 0 ? ARRAY_SIZE - 1 : index - 1];
        }

        [[nodiscard]] const Time::Timestamp& getLastUpdateTime() const {
            const auto index = updateTimesIndex % ARRAY_SIZE;
            return updateTimes[index == 0 ? ARRAY_SIZE - 1 : index - 1];
        }

        [[nodiscard]] Gather gather() const {
            Gather gather{};
            uint32_t count = frameTimesIndex < ARRAY_SIZE ? frameTimesIndex : ARRAY_SIZE;

            if (count > 0) {
                for (uint32_t i = 0; i < count; i++) {
                    gather.frameTime += frameTimes[i];
                    gather.drawTime += drawTimes[i];
                    gather.updateTime += updateTimes[i];
                    gather.idleTime += idleTimes[i];
                }

                gather.framesPerSecond = framesPerSecond;
                gather.frameTime = Time::Timestamp::ofMicroseconds(gather.frameTime.toMicroseconds() / count);
                gather.drawTime = Time::Timestamp::ofMicroseconds(gather.drawTime.toMicroseconds() / count);
                gather.updateTime = Time::Timestamp::ofMicroseconds(gather.updateTime.toMicroseconds() / count);
                gather.idleTime = Time::Timestamp::ofMicroseconds(gather.idleTime.toMicroseconds() / count);
            }

            return gather;
        }

        void reset() {
            frames = 0;
            framesPerSecond = 0;
            timeCounter.reset();

            frameTimesIndex = 0;
            drawTimesIndex = 0;
            updateTimesIndex = 0;
            idleTimesIndex = 0;

            frameTimeStart.reset();
            drawTimeStart.reset();
            updateTimeStart.reset();
            idleTimeStart.reset();
        }

    private:
        static const constexpr uint32_t ARRAY_SIZE = 100;

        uint32_t frames = 0;
        uint32_t framesPerSecond = 0;
        Time::Timestamp timeCounter;

        Time::Timestamp frameTimes[ARRAY_SIZE]{};
        Time::Timestamp drawTimes[ARRAY_SIZE]{};
        Time::Timestamp updateTimes[ARRAY_SIZE]{};
        Time::Timestamp idleTimes[ARRAY_SIZE]{};

        uint32_t frameTimesIndex = 0;
        uint32_t drawTimesIndex = 0;
        uint32_t updateTimesIndex = 0;
        uint32_t idleTimesIndex = 0;

        Time::Timestamp frameTimeStart;
        Time::Timestamp drawTimeStart;
        Time::Timestamp updateTimeStart;
        Time::Timestamp idleTimeStart;
    };

    void initializeNextScene();

    void updateStatus();

    void drawStatus();

    void checkKeyboard();

    void checkMouse();

    void checkMouseKey(Io::Mouse::Button button, uint8_t lastButtonState, uint8_t currentButtonState);

    Game game;
    Graphics graphics;
    Statistics statistics;

    bool showStatus = false;
    Time::Timestamp statusUpdateTimer;
    Statistics::Gather status = statistics.gather();

    Io::FileInputStream *mouseInputStream;
    uint8_t mouseValues[4]{};
    uint32_t mouseValueIndex = 0;
    uint8_t lastMouseButtonState = 0;

    Io::KeyDecoder keyDecoder = Io::KeyDecoder(new Io::DeLayout());
    ArrayList<Io::Key> pressedKeys;

    const uint8_t targetFrameRate;

    const Graphic::Font &statisticsFont;

    static const constexpr char *LOADING = "Loading...";
};

}

#endif
