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
 *
 * The 3D-rendering has been rewritten using OpenGL (TinyGL) during a bachelor's thesis by Kevin Weber
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-keweb100
 *
 * The 2D particle system is based on a bachelor's thesis, written by Abdulbasir Gümüs.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-abgue101
 */

#ifndef HHUOS_LIB_PULSAR_STATISTICS_H
#define HHUOS_LIB_PULSAR_STATISTICS_H

#include <stddef.h>

#include "util/time/Timestamp.h"

namespace Pulsar {

/// Utility class to gather statistics about the game engine performance.
/// It can measure four different time spans:
/// - Frame time: Time taken for one complete frame (draw + update + idle)
/// - Draw time: Time taken to render the current frame
/// - Update time: Time taken to update the game logic
/// - Idle time: Time spent idling to maintain the target frame rate
///
/// It also calculates the frames per second (FPS) based on the frame times.
/// The engine manually starts and stops the timers for each time span at appropriate points in the game loop.
/// The statistics class maintains a history of the last 100 measurements for each time span
/// and provides methods to retrieve the last recorded times as well as average values.
class Statistics {

public:
    /// Structure to hold gathered statistics.
    struct Gather {
        /// Average frames per second based on recorded frame times.
        size_t framesPerSecond = 0;
        /// Average frame time.
        Util::Time::Timestamp frameTime;
        /// Average draw time.
        Util::Time::Timestamp drawTime;
        /// Average update time.
        Util::Time::Timestamp updateTime;
        /// Average idle time.
        Util::Time::Timestamp idleTime;
    };

    /// Create a new statistics instance.
    Statistics() = default;

    /// Start measuring frame time.
    void startFrameTime() {
        frameTimeStart = Util::Time::Timestamp::getSystemTime();
    }

    /// Stop measuring frame time.
    void stopFrameTime() {
        const auto frameTime = Util::Time::Timestamp::getSystemTime() - frameTimeStart;
        frameTimes[frameTimesIndex++ % ARRAY_SIZE] = frameTime;
        frames++;

        timeCounter += frameTime;
        if (timeCounter >= Util::Time::Timestamp::ofSeconds(1)) {
            framesPerSecond = frames;
            frames = 0;
            timeCounter = Util::Time::Timestamp();
        }
    }

    /// Start measuring draw time.
    void startDrawTime() {
        drawTimeStart = Util::Time::Timestamp::getSystemTime();
    }

    /// Stop measuring draw time.
    void stopDrawTime() {
        drawTimes[drawTimesIndex++ % ARRAY_SIZE] = Util::Time::Timestamp::getSystemTime() - drawTimeStart;
    }

    /// Start measuring update time.
    void startUpdateTime() {
        updateTimeStart = Util::Time::Timestamp::getSystemTime();
    }

    /// Stop measuring update time.
    void stopUpdateTimeTime() {
        updateTimes[updateTimesIndex++ % ARRAY_SIZE] = Util::Time::Timestamp::getSystemTime() - updateTimeStart;
    }

    /// Start measuring idle time.
    void startIdleTime() {
        idleTimeStart = Util::Time::Timestamp::getSystemTime();
    }

    /// Stop measuring idle time.
    void stopIdleTime() {
        idleTimes[idleTimesIndex++ % ARRAY_SIZE] = Util::Time::Timestamp::getSystemTime() - idleTimeStart;
    }

    /// Get the last recorded frame time.
    /// If no frame time has been recorded yet, returns a zero timestamp.
    const Util::Time::Timestamp& getLastFrameTime() const {
        if (frameTimesIndex == 0) {
            return frameTimes[0];
        }

        return frameTimes[(frameTimesIndex - 1) % ARRAY_SIZE];
    }

    /// Get the last recorded draw time.
    /// If no draw time has been recorded yet, returns a zero timestamp.
    const Util::Time::Timestamp& getLastDrawTime() const {
        if (drawTimesIndex == 0) {
            return drawTimes[0];
        }

        return drawTimes[(drawTimesIndex - 1) % ARRAY_SIZE];
    }

    /// Get the last recorded update time.
    /// If no update time has been recorded yet, returns a zero timestamp.
    const Util::Time::Timestamp& getLastUpdateTime() const {
        if (updateTimesIndex == 0) {
            return updateTimes[0];
        }

        return updateTimes[(updateTimesIndex - 1) % ARRAY_SIZE];
    }

    /// Gather the measured times and calculate average values.
    /// The maximum number of samples considered is 100 (the size of the internal arrays).
    Gather gather() const {
        Gather gather{};
        const auto count = frameTimesIndex < ARRAY_SIZE ? frameTimesIndex : ARRAY_SIZE;

        if (count > 0) {
            for (size_t i = 0; i < count; i++) {
                gather.frameTime += frameTimes[i];
                gather.drawTime += drawTimes[i];
                gather.updateTime += updateTimes[i];
                gather.idleTime += idleTimes[i];
            }

            gather.framesPerSecond = framesPerSecond;
            gather.frameTime = Util::Time::Timestamp::ofMicroseconds(gather.frameTime.toMicroseconds() / count);
            gather.drawTime = Util::Time::Timestamp::ofMicroseconds(gather.drawTime.toMicroseconds() / count);
            gather.updateTime = Util::Time::Timestamp::ofMicroseconds(gather.updateTime.toMicroseconds() / count);
            gather.idleTime = Util::Time::Timestamp::ofMicroseconds(gather.idleTime.toMicroseconds() / count);
        }

        return gather;
    }

private:

    static constexpr size_t ARRAY_SIZE = 100;

    size_t frames = 0;
    size_t framesPerSecond = 0;
    Util::Time::Timestamp timeCounter;

    Util::Time::Timestamp frameTimes[ARRAY_SIZE]{};
    Util::Time::Timestamp drawTimes[ARRAY_SIZE]{};
    Util::Time::Timestamp updateTimes[ARRAY_SIZE]{};
    Util::Time::Timestamp idleTimes[ARRAY_SIZE]{};

    size_t frameTimesIndex = 0;
    size_t drawTimesIndex = 0;
    size_t updateTimesIndex = 0;
    size_t idleTimesIndex = 0;

    Util::Time::Timestamp frameTimeStart;
    Util::Time::Timestamp drawTimeStart;
    Util::Time::Timestamp updateTimeStart;
    Util::Time::Timestamp idleTimeStart;
};

}

#endif