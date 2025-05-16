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
 */

#ifndef HHUOS_PARALLELPORT_H
#define HHUOS_PARALLELPORT_H

#include <stdint.h>

#include "device/cpu/IoPort.h"
#include "lib/util/base/String.h"
#include "lib/util/io/stream/OutputStream.h"

namespace Device {

class ParallelPort : public Util::Io::OutputStream {

public:
    /**
     * Starting addresses of the LPT port registers.
     */
    enum LptPort {
        LPT1 = 0x378,
        LPT2 = 0x278,
        LPT3 = 0x3bc
    };

    /**
     * Constructor.
     */
    explicit ParallelPort(LptPort port);

    /**
     * Copy Constructor.
     */
    ParallelPort(const ParallelPort &other) = delete;

    /**
     * Assignment operator.
     */
    ParallelPort &operator=(const ParallelPort &other) = delete;

    /**
     * Destructor.
     */
    ~ParallelPort() override = default;

    /**
     * Check if an LPT-port exists.
     * Always check if the LPT-port exists before creating an instance of this class!
     *
     */
    static bool checkPort(LptPort port);

    static void initializeAvailablePorts();

    [[nodiscard]] static LptPort portFromString(const Util::String &portName);

    [[nodiscard]] static const char* portToString(LptPort port);

    void write(uint8_t c) override;

    void write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) override;

private:

    static void initializePort(LptPort port);

    bool isBusy();

    IoPort sppDataPort;
    IoPort sppStatusPort;
    IoPort sppControlPort;

    static const constexpr uint8_t STATUS_REGISTER_IRQ = 0x04;
    static const constexpr uint8_t STATUS_REGISTER_ERROR = 0x08;
    static const constexpr uint8_t STATUS_REGISTER_SELECT_IN = 0x10;
    static const constexpr uint8_t STATUS_REGISTER_PAPER_OUT = 0x20;
    static const constexpr uint8_t STATUS_REGISTER_ACK = 0x40;
    static const constexpr uint8_t STATUS_REGISTER_BUSY = 0x80;

    static const constexpr uint8_t CONTROL_REGISTER_STROBE = 0x01;
    static const constexpr uint8_t CONTROL_REGISTER_AUTOMATIC_LINEFEED = 0x02;
    static const constexpr uint8_t CONTROL_REGISTER_INITIALISE = 0x04;
    static const constexpr uint8_t CONTROL_REGISTER_SELECT = 0x08;
    static const constexpr uint8_t CONTROL_REGISTER_IRQ_ACK = 0x10;
    static const constexpr uint8_t CONTROL_REGISTER_BIDIRECTIONAL = 0x20;
};

}

#endif
