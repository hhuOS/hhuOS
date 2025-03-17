/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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

#include <stdint.h>

#include "kernel/interrupt/InterruptHandler.h"
#include "lib/util/io/stream/FilterInputStream.h"
#include "Ps2Device.h"
#include "lib/util/collection/ArrayBlockingQueue.h"
#include "lib/util/io/stream/QueueInputStream.h"

namespace Kernel {
enum InterruptVector : uint8_t;
struct InterruptFrame;
}  // namespace Kernel

namespace Device {
class Ps2Controller;

class Mouse : public Ps2Device, public Util::Io::FilterInputStream, public Kernel::InterruptHandler {

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

    void trigger(const Kernel::InterruptFrame &frame, Kernel::InterruptVector slot) override;

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

    DeviceType identify();

    void activateScrollWheel();

    void activateAdditionalButtons();

    uint32_t cycle = 1;
    uint32_t flags = 0;
    int32_t dx = 0;
    int32_t dy = 0;

    DeviceType type = STANDARD_MOUSE;

    Util::ArrayBlockingQueue<uint8_t> inputBuffer;
    Util::Io::QueueInputStream inputStream;

    static const constexpr uint32_t BUFFER_SIZE = 1024;
};

}

#endif
