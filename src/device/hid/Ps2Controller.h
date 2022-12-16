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

#ifndef HHUOS_PS2CONTROLLER_H
#define HHUOS_PS2CONTROLLER_H

#include <cstdint>

#include "device/cpu/IoPort.h"

namespace Kernel {
class Logger;
}  // namespace Kernel

namespace Device {

class Ps2Controller {

public:
    /**
     * Default Constructor.
     */
    Ps2Controller() = default;

    /**
     * Copy Constructor.
     */
    Ps2Controller(const Ps2Controller &other) = delete;

    /**
     * Assignment operator.
     */
    Ps2Controller &operator=(const Ps2Controller &other) = delete;

    /**
     * Destructor.
     */
    ~Ps2Controller() = default;

    enum Port : uint8_t {
        FIRST,
        SECOND
    };

    static Ps2Controller* initialize();

    void flushOutputBuffer();

    void writeDataToPort(Port port, uint8_t data);

    uint8_t readDataByte();

    uint8_t readControlByte();

    void enableKeyboardTranslation();

    [[nodiscard]] bool isPortAvailable(Port port) const;

private:

    enum Command : uint8_t {
        READ_CONFIGURATION_BYTE = 0x20,
        WRITE_CONFIGURATION_BYTE = 0x60,
        DISABLE_SECOND_PORT = 0xa7,
        ENABLE_SECOND_PORT = 0xa8,
        TEST_SECOND_PORT = 0xa9,
        TEST_CONTROLLER = 0xaa,
        TEST_FIRST_PORT = 0xab,
        DISABLE_FIRST_PORT = 0xad,
        ENABLE_FIRST_PORT = 0xae,
        READ_CONTROLLER_OUTPUT_PORT = 0xd0,
        WRITE_CONTROLLER_OUTPUT_PORT = 0xd1,
        WRITE_TO_FIRST_PORT_OUTPUT_BUFFER = 0xd2,
        WRITE_TO_SECOND_PORT_OUTPUT_BUFFER = 0xd3,
        WRITE_TO_SECOND_PORT_INPUT_BUFFER = 0xd4
    };

    enum Configuration : uint8_t {
        FIRST_PORT_INTERRUPTS = 0x01,
        SECOND_PORT_INTERRUPTS = 0x02,
        SYSTEM_FLAG = 0x04,
        FIRST_PORT_CLOCK = 0x10,
        SECOND_PORT_CLOCK = 0x20,
        FIRST_PORT_TRANSLATION = 0x40
    };

    bool waitOutputBuffer();

    bool waitInputBuffer();

    bool writeControlByte(uint8_t data);

    bool writeDataByte(uint8_t data);

    uint8_t writeCommand(uint8_t firstByte, uint8_t secondByte = 0x00);

    bool firstPortAvailable = false;
    bool secondPortAvailable = false;

    IoPort controlPort = IoPort(0x64);
    IoPort dataPort = IoPort(0x60);

    static Kernel::Logger log;

    static const constexpr uint32_t TIMEOUT = 100;
    static const constexpr uint8_t SELF_TEST_OK = 0x55;
    static const constexpr uint8_t PORT_TEST_OK = 0x00;
};

}

#endif
