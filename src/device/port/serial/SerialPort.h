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

#ifndef HHUOS_SERIALPORT_H
#define HHUOS_SERIALPORT_H

#include <stdint.h>

#include "kernel/interrupt/InterruptHandler.h"
#include "lib/util/io/stream/FilterInputStream.h"
#include "lib/util/io/stream/OutputStream.h"
#include "lib/util/collection/ArrayBlockingQueue.h"
#include "lib/util/io/stream/QueueInputStream.h"
#include "SimpleSerialPort.h"
#include "device/port/serial/Serial.h"

namespace Kernel {
enum InterruptVector : uint8_t;
struct InterruptFrame;
}  // namespace Kernel

namespace Device {

/**
 * Driver for the serial COM-ports.
 */
class SerialPort : public Util::Io::FilterInputStream, public Util::Io::OutputStream, public Kernel::InterruptHandler {

public:

    explicit SerialPort(Serial::ComPort port, Serial::BaudRate dataRate = Serial::BaudRate::BAUD_115200);

    /**
     * Check if a COM-port exists.
     * Always check if the COM-port exists before creating an instance of this class!
     *
     */
    static bool checkPort(Serial::ComPort port);

    static void initializeAvailablePorts();

    /**
     * Copy Constructor.
     */
    SerialPort(const SerialPort &other) = delete;

    /**
     * Assignment operator.
     */
    SerialPort &operator=(const SerialPort &other) = delete;

    /**
     * Destructor.
     */
    ~SerialPort() override = default;

    /**
     * Overriding function from InterruptHandler.
     */
    void plugin() override;

    /**
     * Overriding function from InterruptHandler.
     */
    void trigger(const Kernel::InterruptFrame &frame, Kernel::InterruptVector slot) override;

    /**
     * Set the baud-rate.
     *
     * @param rate The baud-rate
     */
    void setDataRate(Serial::BaudRate rate);

    /**
     * Get the baud-rate.
     */
    [[nodiscard]] Serial::BaudRate getDataRate() const;

    void write(uint8_t c) override;

    void write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) override;

    uint8_t readDirect();

private:

    static void initializePort(Serial::ComPort port);

    Util::ArrayBlockingQueue<uint8_t> inputBuffer;
    Util::Io::QueueInputStream inputStream;

    SimpleSerialPort port;

    static const constexpr uint32_t BUFFER_SIZE = 1024;
};

}

#endif