/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
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
 */

#include "FloppyMotorControlRunnable.h"

#include "lib/util/async/Thread.h"
#include "device/storage/floppy/FloppyController.h"
#include "device/storage/floppy/FloppyDevice.h"
#include "lib/util/time/Timestamp.h"

namespace Device::Storage {

FloppyMotorControlRunnable::FloppyMotorControlRunnable(FloppyDevice &device) : device(device) {}

void FloppyMotorControlRunnable::run() {
    while (true) {
        if (device.getMotorState() == FloppyController::WAIT) {
            if (remainingTime <= 0) {
                device.killMotor();
                resetTime();
            }

            remainingTime -= INTERVAL;
        }

        Util::Async::Thread::sleep(Util::Time::Timestamp(0, INTERVAL * 1000000));
    }
}

void FloppyMotorControlRunnable::resetTime() {
    remainingTime = TIME;
}

}