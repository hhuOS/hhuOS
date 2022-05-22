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

#ifndef HHUOS_FLOPPYMOTORCONTROLJOB_H
#define HHUOS_FLOPPYMOTORCONTROLJOB_H

#include "lib/util/async/Runnable.h"
#include "FloppyDevice.h"

namespace Device::Storage {

/**
 * Runs in background and controls the state of a floppy drive's motor.
 */
class FloppyMotorControlJob : public Util::Async::Runnable {

public:

    /**
     * Constructor.
     *
     * @param device The device
     */
    explicit FloppyMotorControlJob(FloppyDevice &device);

    /**
     * Copy Constructor.
     */
    FloppyMotorControlJob(const FloppyMotorControlJob &copy) = delete;

    /**
     * Assignment operator.
     */
    FloppyMotorControlJob& operator=(const FloppyMotorControlJob &other) = delete;

    /**
     * Destructor.
     */
    ~FloppyMotorControlJob() override = default;

    /**
     * Overriding function from Thread.
     */
    void run() override;

    void resetTime();

    static const constexpr uint32_t TIME = 2000;
    static const constexpr uint32_t INTERVAL = 500;

private:

    FloppyDevice &device;
    uint32_t remainingTime = TIME;
};

}

#endif
