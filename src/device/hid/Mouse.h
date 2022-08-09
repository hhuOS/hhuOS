/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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
 */

#ifndef HHUOS_MOUSE_H
#define HHUOS_MOUSE_H

#include "device/cpu/IoPort.h"
#include "kernel/log/Logger.h"
#include "kernel/interrupt/InterruptHandler.h"

namespace Device {

class Mouse : public Kernel::InterruptHandler, Util::Stream::InputStream {

public:
    /**
     * Default Constructor.
     */
    Mouse();

    /**
     * Copy Constructor.
     */
    Mouse(const Mouse &other) = delete;

    /**
     * Assignment operator.
     */
    Mouse &operator=(const Mouse &other) = delete;

    /**
     * Destructor.
     */
    ~Mouse() override = default;

    static void initialize();

    void plugin() override;

    void trigger(const Kernel::InterruptFrame &frame) override;

    [[nodiscard]] int16_t read() override;

    [[nodiscard]] int32_t read(uint8_t *targetBuffer, uint32_t offset, uint32_t length) override;

private:

    /**
     * Wait for the controller to have data available.
     */
    void waitData();

    /**
     * Wait for the controller to be ready to receive commands
     */
    void waitControl();

    /**
     * Read a byte from the controller.
     */
    uint8_t readByte();

    /**
     * Write a command to the controller.
     */
    void writeByte(uint8_t data);

    bool writeCommand(uint8_t command);

    bool writeCommandAndByte(uint8_t command, uint8_t data);

    void cleanup();

    uint32_t cycle = 1;
    uint32_t flags = 0;
    int32_t dx = 0;
    int32_t dy = 0;

    IoPort controlPort = IoPort(0x64);
    IoPort dataPort = IoPort(0x60);

    Util::Stream::PipedOutputStream outputStream;
    Util::Stream::PipedInputStream inputStream;

    static Kernel::Logger log;

    static const constexpr uint32_t TIMEOUT = 100;
};

}

#endif
