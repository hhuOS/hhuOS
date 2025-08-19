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

#include "SimpleSerialPort.h"

namespace Device {
namespace Serial {
enum ComPort : uint16_t;
enum class BaudRate : uint16_t;
}  // namespace Serial

SimpleSerialPort::SimpleSerialPort(Serial::ComPort port, Serial::BaudRate dataRate) :
        port(port), dataRate(dataRate), dataRegister(port), interruptRegister(port + 1), fifoControlRegister(port + 2),
        lineControlRegister(port + 3), modemControlRegister(port + 4), lineStatusRegister(port + 5),
        modemStatusRegister(port + 6), scratchRegister(port + 7) {
    interruptRegister.writeByte(0x00);      // Disable all interrupts
    lineControlRegister.writeByte(0x80);    // Enable DLAB, so that the divisor can be set

    dataRegister.writeByte(static_cast<uint8_t>(static_cast<uint16_t>(dataRate) & 0x0f));      // Divisor low byte
    interruptRegister.writeByte(static_cast<uint8_t>(static_cast<uint16_t>(dataRate) >> 8));   // Divisor high byte

    lineControlRegister.writeByte(0x03);    // 8 bits per char, no parity, one stop bit
    fifoControlRegister.writeByte(0x07);    // Enable FIFO-buffers, Clear FIFO-buffers, Trigger interrupt after each byte
    modemControlRegister.writeByte(0x0b);   // Enable data lines
}

void SimpleSerialPort::setDataRate(Serial::BaudRate rate) {
    dataRate = rate;

    uint8_t interruptBackup = interruptRegister.readByte();
    uint8_t lineControlBackup = lineStatusRegister.readByte();

    interruptRegister.writeByte(0x00);                   // Disable all interrupts
    lineControlRegister.writeByte(0x80);                 // Enable to DLAB, so that the divisor can be set

    dataRegister.writeByte(static_cast<uint8_t>(static_cast<uint16_t>(rate) & 0x0f));       // Divisor low byte
    interruptRegister.writeByte(static_cast<uint8_t>(static_cast<uint16_t>(rate) >> 8));    // Divisor high byte

    lineControlRegister.writeByte(lineControlBackup);   // Restore line control register
    interruptRegister.writeByte(interruptBackup);       // Restore interrupt register
}

Serial::BaudRate SimpleSerialPort::getDataRate() const {
    return dataRate;
}

Serial::ComPort SimpleSerialPort::getPort() const {
    return port;
}

bool SimpleSerialPort::write(uint8_t c) {
    if (c == '\n') {
        return write(13);
    }

    while (!(lineStatusRegister.readByte() & 0x20)) {}
    dataRegister.writeByte(c);

    return true;
}

uint32_t SimpleSerialPort::write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) {
    for (uint32_t i = 0; i < length; i++) {
        if (!write(sourceBuffer[offset + i])) {
            return i;
        }
    }

    return length;
}

int16_t SimpleSerialPort::read() {
	if (peekedChar != -1) {
		int16_t ret = peekedChar;
		peekedChar = -1;
		return ret;
	}
	
    while (!isReadyToRead()) {}

    return dataRegister.readByte();
}

int16_t SimpleSerialPort::peek() {
	if (peekedChar == -1) peekedChar = read();
	return peekedChar;
}

int32_t SimpleSerialPort::read(uint8_t *targetBuffer, uint32_t offset, uint32_t length) {
    for (uint32_t i = 0; i < length; i++) {
        targetBuffer[offset] = read();
    }

    return length;
}

bool SimpleSerialPort::isReadyToRead() {
    return (lineStatusRegister.readByte() & 0x01) == 0x01;
}

}