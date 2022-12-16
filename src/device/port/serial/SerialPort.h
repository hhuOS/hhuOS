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

#ifndef HHUOS_SERIALPORT_H
#define HHUOS_SERIALPORT_H

#include <cstdint>

#include "kernel/interrupt/InterruptHandler.h"
#include "lib/util/stream/PipedOutputStream.h"
#include "lib/util/stream/FilterInputStream.h"
#include "lib/util/memory/String.h"
#include "device/cpu/IoPort.h"
#include "lib/util/stream/OutputStream.h"
#include "lib/util/stream/PipedInputStream.h"

namespace Kernel {
    class Logger;
struct InterruptFrame;
}

namespace Device {

/**
 * Driver for the serial COM-ports.
 */
class SerialPort : public Util::Stream::FilterInputStream, public Util::Stream::OutputStream, public Kernel::InterruptHandler {

public:
    /**
     * Starting addresses of the COM port registers.
     */
    enum ComPort : uint16_t {
        COM1 = 0x3f8,
        COM2 = 0x2f8,
        COM3 = 0x3e8,
        COM4 = 0x2e8
    };

    /**
     * Available baud rates.
     */
    enum class BaudRate : uint16_t {
        BAUD_115200 = 1,
        BAUD_57600 = 2,
        BAUD_38400 = 3,
        BAUD_28800 = 4,
        BAUD_23040 = 5,
        BAUD_19200 = 6,
        BAUD_14400 = 8,
        BAUD_12800 = 9,
        BAUD_11520 = 10,
        BAUD_9600 = 12,
        BAUD_7680 = 15,
        BAUD_7200 = 16,
        BAUD_6400 = 18,
        BAUD_5760 = 20,
        BAUD_4800 = 24,
        BAUD_4608 = 25,
        BAUD_3840 = 30,
        BAUD_3600 = 32,
        BAUD_3200 = 36,
        BAUD_2880 = 40,
        BAUD_2560 = 45,
        BAUD_2400 = 48,
        BAUD_2304 = 50,
        BAUD_1920 = 60,
        BAUD_1800 = 64,
        BAUD_1600 = 72,
        BAUD_1536 = 75,
        BAUD_1440 = 80,
        BAUD_1280 = 90,
        BAUD_1200 = 96,
        BAUD_1152 = 100,
        BAUD_960 = 120,
        BAUD_900 = 128,
        BAUD_800 = 144,
        BAUD_768 = 150,
        BAUD_720 = 160,
        BAUD_640 = 180,
        BAUD_600 = 192,
        BAUD_576 = 200,
        BAUD_512 = 225,
        BAUD_480 = 240,
        BAUD_450 = 256,
        BAUD_400 = 288,
        BAUD_384 = 300,
        BAUD_360 = 320,
        BAUD_320 = 360,
        BAUD_300 = 384,
        BAUD_288 = 400,
        BAUD_256 = 450,
        BAUD_240 = 480,
        BAUD_225 = 512,
        BAUD_200 = 576,
        BAUD_192 = 600,
        BAUD_180 = 640,
        BAUD_160 = 720,
        BAUD_150 = 768,
        BAUD_144 = 800,
        BAUD_128 = 900,
        BAUD_120 = 960,
        BAUD_100 = 1152,
        BAUD_96 = 1200,
        BAUD_90 = 1280,
        BAUD_80 = 1440,
        BAUD_75 = 1536,
        BAUD_72 = 1600,
        BAUD_64 = 1800,
        BAUD_60 = 1920,
        BAUD_50 = 2304,
        BAUD_48 = 2400,
        BAUD_45 = 2560,
        BAUD_40 = 2880,
        BAUD_36 = 3200,
        BAUD_32 = 3600,
        BAUD_30 = 3840,
        BAUD_25 = 4608,
        BAUD_24 = 4800,
        BAUD_20 = 5760,
        BAUD_18 = 6400,
        BAUD_16 = 7200,
        BAUD_15 = 7680,
        BAUD_12 = 9600,
        BAUD_10 = 11520,
        BAUD_9 = 12800,
        BAUD_8 = 14400,
        BAUD_6 = 19200,
        BAUD_5 = 23040,
        BAUD_4 = 28800,
        BAUD_3 = 38400,
        BAUD_2 = 57600
    };

    explicit SerialPort(ComPort port, BaudRate dataRate = BaudRate::BAUD_115200);

    /**
     * Check if a COM-port exists.
     * Always check if the COM-port exists before creating an instance of this class!
     *
     */
    static bool checkPort(ComPort port);

    static void initializeAvailablePorts();

    [[nodiscard]] static ComPort portFromString(const Util::Memory::String &portName);

    [[nodiscard]] static const char* portToString(ComPort port);

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
    void trigger(const Kernel::InterruptFrame &frame) override;

    /**
     * Set the baud-rate.
     *
     * @param rate The baud-rate
     */
    void setDataRate(BaudRate rate);

    /**
     * Get the baud-rate.
     */
    [[nodiscard]] BaudRate getDataRate() const;

    void write(uint8_t c) override;

    void write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) override;

private:

    static void initializePort(ComPort port);

    Util::Stream::PipedOutputStream outputStream;
    Util::Stream::PipedInputStream inputStream;
    ComPort port;
    BaudRate dataRate;

    IoPort dataRegister;
    IoPort interruptRegister;
    IoPort fifoControlRegister;
    IoPort lineControlRegister;
    IoPort modemControlRegister;
    IoPort lineStatusRegister;
    IoPort modemStatusRegister;
    IoPort scratchRegister;

    static Kernel::Logger log;

};

}

#endif