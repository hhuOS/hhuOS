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

#include "ParallelPort.h"

#include "filesystem/memory/StreamNode.h"
#include "kernel/service/FilesystemService.h"
#include "kernel/system/System.h"
#include "lib/util/async/Thread.h"
#include "filesystem/core/Filesystem.h"
#include "filesystem/memory/MemoryDriver.h"
#include "kernel/log/Logger.h"
#include "lib/util/Exception.h"
#include "lib/util/time/Timestamp.h"

namespace Device {

Kernel::Logger ParallelPort::log = Kernel::Logger::get("LPT");

ParallelPort::ParallelPort(ParallelPort::LptPort port) : sppDataPort(port), sppStatusPort(port + 1), sppControlPort(port + 2) {
    uint8_t control = sppControlPort.readByte();

    // Initialize printer and enable automatic linefeed
    control = control | CONTROL_REGISTER_INITIALISE;
    // Clear strobe, automatic linefeed, irq, and bidirectional bits
    control = control & ~(CONTROL_REGISTER_STROBE | CONTROL_REGISTER_AUTOMATIC_LINEFEED | CONTROL_REGISTER_IRQ_ACK | CONTROL_REGISTER_BIDIRECTIONAL);

    sppControlPort.writeByte(control);
}

bool ParallelPort::checkPort(ParallelPort::LptPort port) {
    auto controlPort = IoPort(port + 2);

    // Toggle the automatic linefeed bit in the control register and check if it stays toggled.
    // If yes, there is a port.
    uint8_t tmp = controlPort.readByte();
    tmp ^= CONTROL_REGISTER_AUTOMATIC_LINEFEED;
    controlPort.writeByte(tmp);

    if (controlPort.readByte() == tmp) {
        controlPort.writeByte(tmp ^ CONTROL_REGISTER_AUTOMATIC_LINEFEED);
        return true;
    }

    return false;
}

void ParallelPort::initializePort(ParallelPort::LptPort port) {
    if (!checkPort(port)) {
        return;
    }

    log.info("Parallel port [%s] detected", portToString(port));

    auto *parallelPort = new ParallelPort(port);
    auto *streamNode = new Filesystem::Memory::StreamNode(Util::Memory::String(portToString(port)).toLowerCase(), reinterpret_cast<Util::Stream::OutputStream*>(parallelPort));

    auto &filesystem = Kernel::System::getService<Kernel::FilesystemService>().getFilesystem();
    auto &driver = filesystem.getVirtualDriver("/device");
    bool success = driver.addNode("/", streamNode);

    if (!success) {
        log.error("Failed to initialize virtual node for [%s]", portToString(port));
        delete streamNode;
    }
}

void ParallelPort::initializeAvailablePorts() {
    initializePort(LPT1);
    initializePort(LPT2);
    initializePort(LPT3);
}

ParallelPort::LptPort ParallelPort::portFromString(const Util::Memory::String &portName) {
    const auto port = portName.toLowerCase();

    if (port == "lpt1") {
        return LPT1;
    } else if (port == "lpt2") {
        return LPT2;
    } else if (port == "lpt3") {
        return LPT3;
    } else {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "LPT: Invalid port!");
    }
}

const char *ParallelPort::portToString(ParallelPort::LptPort port) {
    switch (port) {
        case LPT1:
            return "LPT1";
        case LPT2:
            return "LPT2";
        case LPT3:
            return "LPT3";
        default:
            Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "LPT: Invalid port!");
    }
}

bool ParallelPort::isBusy() {
    return (sppStatusPort.readByte() & STATUS_REGISTER_BUSY) == 0x00;
}

void ParallelPort::write(uint8_t c) {
    if (c == '\n') {
        write(13);
    }

    // Wait for the printer to be ready
    while (isBusy()) {}

    // Send the byte
    sppDataPort.writeByte(c);

    // Pulse the strobe bit, so that the printer knows, that there is data to be fetched on the data port
    uint8_t control = sppControlPort.readByte();
    sppControlPort.writeByte(control | CONTROL_REGISTER_STROBE);
    Util::Async::Thread::sleep(Util::Time::Timestamp(0, 10000000));
    sppControlPort.writeByte(control);

    // Wait for the printer to finish reading the data
    while (isBusy()) {}
}

void ParallelPort::write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) {
    for (uint32_t i = 0; i < length; i++) {
        write(sourceBuffer[offset + i]);
    }
}

}