/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_SIMPLESERIALPORT_H
#define HHUOS_SIMPLESERIALPORT_H

#include <cstdint>

#include "device/cpu/IoPort.h"
#include "lib/util/io/stream/OutputStream.h"
#include "Serial.h"
#include "lib/util/io/stream/InputStream.h"

namespace Device {

/**
 * Driver for the serial COM-ports without support for interrupts and buffered streams.
 */
class SimpleSerialPort : public Util::Io::OutputStream, Util::Io::InputStream {

friend class SerialPort;

public:

    explicit SimpleSerialPort(Serial::ComPort port, Serial::BaudRate dataRate = Serial::BaudRate::BAUD_115200);

    /**
     * Copy Constructor.
     */
    SimpleSerialPort(const SimpleSerialPort &other) = delete;

    /**
     * Assignment operator.
     */
    SimpleSerialPort &operator=(const SimpleSerialPort &other) = delete;

    /**
     * Destructor.
     */
    ~SimpleSerialPort() override = default;

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

    [[nodiscard]] Serial::ComPort getPort() const;

    void write(uint8_t c) override;

    void write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) override;

    int16_t read() override;

    int32_t read(uint8_t *targetBuffer, uint32_t offset, uint32_t length) override;

    Serial::ComPort port;
    Serial::BaudRate dataRate;

    IoPort dataRegister;
    IoPort interruptRegister;
    IoPort fifoControlRegister;
    IoPort lineControlRegister;
    IoPort modemControlRegister;
    IoPort lineStatusRegister;
    IoPort modemStatusRegister;
    IoPort scratchRegister;
};

}

#endif