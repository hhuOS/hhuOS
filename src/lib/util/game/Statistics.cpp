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

#include "Statistics.h"

namespace Util::Game {

void Statistics::startFrameTime() {
    frameTimeStart = Time::Timestamp::getSystemTime();
}

void Statistics::stopFrameTime() {
    const auto frameTime = Time::Timestamp::getSystemTime() - frameTimeStart;
    frameTimes[frameTimesIndex++ % ARRAY_SIZE] = frameTime;
    frames++;

    timeCounter += frameTime;
    if (timeCounter >= Time::Timestamp::ofSeconds(1)) {
        framesPerSecond = frames;
        frames = 0;
        timeCounter = Time::Timestamp();
    }
}

void Statistics::startDrawTime() {
    drawTimeStart = Time::Timestamp::getSystemTime();
}

void Statistics::stopDrawTime() {
    drawTimes[drawTimesIndex++ % ARRAY_SIZE] = Time::Timestamp::getSystemTime() - drawTimeStart;
}

void Statistics::startUpdateTime() {
    updateTimeStart = Time::Timestamp::getSystemTime();
}

void Statistics::stopUpdateTimeTime() {
    updateTimes[updateTimesIndex++ % ARRAY_SIZE] = Time::Timestamp::getSystemTime() - updateTimeStart;
}

void Statistics::startIdleTime() {
    idleTimeStart = Time::Timestamp::getSystemTime();
}

void Statistics::stopIdleTime() {
    idleTimes[idleTimesIndex++ % ARRAY_SIZE] = Time::Timestamp::getSystemTime() - idleTimeStart;
}

const Time::Timestamp& Statistics::getLastFrameTime() const {
    if (frameTimesIndex == 0) {
        return frameTimes[0];
    }

    return frameTimes[(frameTimesIndex - 1) % ARRAY_SIZE];
}

const Time::Timestamp& Statistics::getLastDrawTime() const {
    if (drawTimesIndex == 0) {
        return drawTimes[0];
    }

    return drawTimes[(drawTimesIndex - 1) % ARRAY_SIZE];
}

const Time::Timestamp& Statistics::getLastUpdateTime() const {
    if (updateTimesIndex == 0) {
        return updateTimes[0];
    }

    return updateTimes[(updateTimesIndex - 1) % ARRAY_SIZE];
}

Statistics::Gather Statistics::gather() const {
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
        gather.frameTime = Time::Timestamp::ofMicroseconds(gather.frameTime.toMicroseconds() / count);
        gather.drawTime = Time::Timestamp::ofMicroseconds(gather.drawTime.toMicroseconds() / count);
        gather.updateTime = Time::Timestamp::ofMicroseconds(gather.updateTime.toMicroseconds() / count);
        gather.idleTime = Time::Timestamp::ofMicroseconds(gather.idleTime.toMicroseconds() / count);
    }

    return gather;
}

}