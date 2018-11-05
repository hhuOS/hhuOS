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

#ifndef HHUOS_FLOPPYMOTORCONTROLTHREAD_H
#define HHUOS_FLOPPYMOTORCONTROLTHREAD_H

#include <kernel/threads/Thread.h>
#include <devices/storage/devices/FloppyDevice.h>

/**
 * Runs in background and controls the state of a floppy drive's motor.
 */
class FloppyMotorControlThread : public Thread {

    friend class FloppyController;

private:

    FloppyDevice &device;

    uint32_t timeout;

    bool isRunning;

    TimeService *timeService = nullptr;

public:

    /**
     * Constructor.
     *
     * @param device The device
     */
    explicit FloppyMotorControlThread(FloppyDevice &device);

    /**
     * Copy-constructor.
     */
    FloppyMotorControlThread(const FloppyMotorControlThread &copy) = delete;

    /**
     * Destructor.
     */
    ~FloppyMotorControlThread() override = default;

    /**
     * Overriding function from Thread.
     */
    void run() override;

    /**
     * Stop the thread.
     */
    void stop();

};

#endif
