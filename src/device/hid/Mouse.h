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

#include <cstdint>

#include "kernel/interrupt/InterruptHandler.h"
#include "lib/util/stream/FilterInputStream.h"
#include "Ps2Device.h"
#include "lib/util/stream/PipedInputStream.h"
#include "lib/util/stream/PipedOutputStream.h"

namespace Kernel {
class Logger;
struct InterruptFrame;
}  // namespace Kernel

namespace Device {
class Ps2Controller;

class Mouse : public Ps2Device, public Util::Stream::FilterInputStream, public Kernel::InterruptHandler {

public:

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

    static Mouse *initialize(Ps2Controller &controller);

    void plugin() override;

    void trigger(const Kernel::InterruptFrame &frame) override;

private:
    /**
     * Default Constructor.
     */
    explicit Mouse(Ps2Controller &controller);

    enum Command : uint8_t {
        SET_SCALING = 0xe6,
        SET_RESOLUTION = 0xe8,
        STATUS_REQEUST = 0xe9,
        SET_STREAM_MODE = 0xea,
        READ_DATA = 0xeb,
        RESET_WRAP_MODE = 0xec,
        SET_WRAP_MODE = 0xee,
        SET_REMOTE_MODE = 0xf0,
        IDENTIFY = 0xf2,
        SET_SAMPLING_RATE = 0xf3,
        ENABLE_DATA_REPORTING = 0xf4,
        DISABLE_DATA_REPORTING = 0xf5,
        SET_DEFAULT_PARAMETERS = 0xf6,
        RESEND_LAST_BYTE = 0xfe,
        RESET = 0xff
    };

    enum Reply : uint8_t {
        SELF_TEST_PASSED = 0xaa,
        ACK = 0xfa,
        SELF_TEST_FAILED_1 = 0xfc,
        SELF_TEST_FAILED_2 = 0xfd,
    };

    bool writeMouseCommand(Command command);

    bool writeMouseCommand(Command command, uint8_t data);

    bool qemuMode; // QEMU inverts axes for some reason...
    uint32_t cycle = 1;
    uint32_t flags = 0;
    int32_t dx = 0;
    int32_t dy = 0;

    Util::Stream::PipedOutputStream outputStream;
    Util::Stream::PipedInputStream inputStream;

    static Kernel::Logger log;

    static const constexpr uint32_t TIMEOUT = 100;
};

}

#endif
