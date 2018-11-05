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

#ifndef HHUOS_SERIAL_H
#define HHUOS_SERIAL_H

#include <cstdint>
#include <kernel/IOport.h>
#include <kernel/services/EventBus.h>
#include "devices/IODevice.h"

class SerialEvent;

/**
 * Driver for the serial COM-ports.
 *
 * @author Fabian Ruhland
 * @date 2018
 */
class Serial : public IODevice {

public:

    /**
     * Starting addresses of the registers of each port.
     */
    enum ComPort {
        COM1 = 1,
        COM2 = 2,
        COM3 = 3,
        COM4 = 4,
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

    /**
     * Check if a COM-port exists.
     * Always check if the COM-port exists before creating an instance of this class!
     *
     * @param port The port
     */
    static bool checkPort(ComPort port);

    /**
     * Constructor
     *
     * @param port The COM-port
     * @param speed The baud-rate
     */
    explicit Serial(ComPort port, BaudRate speed = BaudRate::BAUD_115200) noexcept;

    Serial(const Serial &other) = delete;

    Serial &operator=(const Serial &other) = delete;

    /**
     * Destructor.
     */
    ~Serial() override = default;

    /**
     * Enable interrupts for this COM-port.
     */
    void plugin();

    /**
     * Overriding function from IODevice.
     */
    void trigger(InterruptFrame &frame) override;

    /**
     * Overriding function from IODevice.
     */
    bool checkForData() override ;

    /**
     * Send data via the COM-port.
     *
     * @param data Pointer to the data
     * @param len The amount of bytes to be sent
     */
    void sendData(char *data, uint32_t len);

    /**
     * Read data from the COM-port.
     * This function is blocking and will return only if len bytes have been received!
     *
     * @param data Buffer to which the received data will be written.
     * @param len The amount of bytes to read
     */
    void readData(char *data, uint32_t len);

    /**
     * Read a single character from the COM-port.
     * This function is blocking and will return only if one byte has been received!
     *
     * @return The read character
     */
    char readChar();

    /**
     * Send a single character via the COM-port.
     *
     * @param c The character to send
     */
    void sendChar(char c);

    /**
     * Set the baud-rate.
     *
     * @param speed The baud-rate
     */
    void setSpeed(BaudRate speed);

    /**
     * Get the baud-rate.
     */
    BaudRate getSpeed();

    /**
     * Get the port number.
     */
    ComPort getPortNumber();

private:

    /**
     * Calculate the base address of the IO-ports of a given parallel port.
     *
     * @param port The port
     * @return The base address
     */
    static uint16_t getBasePort(ComPort port);

private:

    EventBus *eventBus;
    Util::RingBuffer<SerialEvent> eventBuffer;

    ComPort port;
    BaudRate speed;

    IOport dataRegister;
    IOport interruptRegister;
    IOport fifoControlRegister;
    IOport lineControlRegister;
    IOport modemControlRegister;
    IOport lineStatusRegister;
    IOport modemStatusRegister;
    IOport scratchRegister;
};

#endif
