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

#ifndef __IODevice_include__
#define __IODevice_include__

#include "kernel/interrupts/InterruptHandler.h"

/**
 * Base-class for interrupt-bases IODevices.
 *
 * While the Scheduler performs a Thread-switch, interrupts are disabled.
 * If an IODevice (e.g. Keyboard) gets new data at exactly that time, the system won't know, that the device has data,
 * because it can't trigger an interrupt. To ensure that no data gets lost while the scheduler is working,
 * it calls checkForData on every IODevice, fetches the available data and sends appropriate Events via the EventBus.
 *
 * @author Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * @date HHU, 2018
 */
class IODevice : public InterruptHandler {

public:
    /**
     * Constructor.
     */
    IODevice() = default;

    /**
     * Copy-constructor.
     */
    IODevice(const IODevice &copy) = delete;

    /**
     * Destructor.
     */
    ~IODevice() override = default;

    /**
     * Check, if the device has data, that must be fetched.
     */
    virtual bool checkForData() = 0;

    /**
     * Overriding function from InterruptHandler.
     */
    void trigger(InterruptFrame &frame) override = 0;
};

#endif
