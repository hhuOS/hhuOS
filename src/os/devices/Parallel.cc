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

#include <kernel/Kernel.h>
#include "Parallel.h"

uint16_t Parallel::getBasePort(LptPort port) {
    auto *address = reinterpret_cast<uint16_t *>(0xc0000408);

    address += port;

    return *address;
}

bool Parallel::checkPort(LptPort port) {
    return getBasePort(port) != 0;
}

Parallel::Parallel(LptPort port) : port(port), dataPort(getBasePort(port)),
                                             statusPort(static_cast<uint16_t>(getBasePort(port) + 1)),
                                             controlPort(static_cast<uint16_t>(getBasePort(port) + 2)) {
    initializePrinter();

    timeService = Kernel::getService<TimeService>();
}

void Parallel::initializePrinter() {
    uint8_t control = controlPort.inb();

    controlPort.outb(control | static_cast<uint8_t>(0x06)); // Initialize Printer and enable automatic linefeed
}

bool Parallel::isBusy() {
    return (statusPort.inb() & static_cast<uint8_t>(0x80)) == 0x00; // NOTE: The busy-flag is active low!
}

bool Parallel::isPaperEmpty() {
    return (statusPort.inb() & static_cast<uint8_t>(0x20)) == 0x20;
}

bool Parallel::checkError() {
    return (statusPort.inb() & static_cast<uint8_t>(0x08)) == 0x08;
}

void Parallel::sendChar(char c) {
    while(isBusy()); // Wait for the printer to be ready

    dataPort.outb(static_cast<uint8_t>(c)); // Send the byte

    // Pulse the strobe bit, so that the printer knows, that there is data to be fetched on the data port.
    uint8_t control = controlPort.inb();
    controlPort.outb(control | static_cast<uint8_t>(0x01));
    timeService->msleep(10);
    controlPort.outb(control);

    while(isBusy()); // Wait for the printer to finish reading the data
}

void Parallel::sendData(char *data, uint32_t len) {
    for(uint32_t i = 0; i < len; i++) {
        sendChar(data[i]);
    }
}

Parallel::LptPort Parallel::getPortNumber() {
    return port;
}
