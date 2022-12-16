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

#include "Ps2Controller.h"

#include "lib/util/async/Thread.h"
#include "kernel/log/Logger.h"
#include "lib/util/Exception.h"
#include "lib/util/time/Timestamp.h"

namespace Device {

Kernel::Logger Ps2Controller::log = Kernel::Logger::get("PS/2");

Ps2Controller* Ps2Controller::initialize() {
    auto *controller = new Ps2Controller();

    // Disable ports during initialization
    controller->writeCommand(DISABLE_FIRST_PORT);
    controller->writeCommand(DISABLE_SECOND_PORT);

    // Flush output buffer
    controller->flushOutputBuffer();

    // Disable interrupts and translation
    auto configuration = controller->writeCommand(READ_CONFIGURATION_BYTE);
    configuration &= ~(FIRST_PORT_INTERRUPTS | SECOND_PORT_INTERRUPTS | FIRST_PORT_TRANSLATION);
    controller->writeCommand(WRITE_CONFIGURATION_BYTE, configuration);

    // Perform self test
    auto result = controller->writeCommand(TEST_CONTROLLER);
    if (result != SELF_TEST_OK) {
        log.error("Self test failed!");
        delete controller;
        return nullptr;
    }
    log.info("Self test result is OK");

    // Check if the controller has reset itself during the self test and if so, write the configuration byte again
    if (controller->writeCommand(READ_CONFIGURATION_BYTE) != configuration) {
        controller->writeCommand(WRITE_CONFIGURATION_BYTE, configuration);
    }

    // Check if the controller has two ports
    controller->writeCommand(ENABLE_SECOND_PORT);
    configuration = controller->writeCommand(READ_CONFIGURATION_BYTE);
    if (configuration & SECOND_PORT_CLOCK) {
        log.info("Single channel controller detected");
    } else {
        log.info("Dual channel controller detected");
        controller->writeCommand(DISABLE_SECOND_PORT);
    }

    result = controller->writeCommand(TEST_FIRST_PORT);
    if (result == PORT_TEST_OK) {
        log.info("First port test result is OK");
        controller->firstPortAvailable = true;
    } else {
        log.error("First port test returned [0x%02x] -> Deactivating port", result);
    }

    result = controller->writeCommand(TEST_SECOND_PORT);
    if (result == PORT_TEST_OK) {
        log.info("Second port test result is OK");
        controller->secondPortAvailable = true;
    } else {
        log.error("Second port test returned [0x%02x] -> Deactivating port", result);
    }

    // Enable working ports
    if (controller->isPortAvailable(FIRST)) {
        controller->writeCommand(ENABLE_FIRST_PORT);
    }
    if (controller->isPortAvailable(SECOND)) {
        controller->writeCommand(ENABLE_SECOND_PORT);
    }

    configuration = controller->writeCommand(READ_CONFIGURATION_BYTE);
    if (controller->isPortAvailable(FIRST)) {
        configuration |= FIRST_PORT_INTERRUPTS;
    }
    if (controller->isPortAvailable(SECOND)) {
        configuration |= SECOND_PORT_INTERRUPTS;
    }
    controller->writeCommand(WRITE_CONFIGURATION_BYTE, configuration);

    return controller;
}

void Ps2Controller::flushOutputBuffer() {
    while ((controlPort.readByte() & 0x01)) {
        dataPort.readByte();
    }
}

bool Ps2Controller::waitOutputBuffer() {
    uint32_t timeout = 0;
    while (!(controlPort.readByte() & 0x01) && timeout < TIMEOUT) {
        Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(10));
        timeout += 10;
    }

    return timeout < TIMEOUT;
}

bool Ps2Controller::waitInputBuffer() {
    uint32_t timeout = 0;
    while ((controlPort.readByte() & 0x02) && timeout < TIMEOUT) {
        Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(10));
        timeout += 10;
    }

    return timeout < TIMEOUT;
}

uint8_t Ps2Controller::readDataByte() {
    if (!waitOutputBuffer()) {
        return UINT8_MAX;
    }

    return dataPort.readByte();
}

bool Ps2Controller::writeDataByte(uint8_t data) {
    if (!waitInputBuffer()) {
        return false;
    }

    dataPort.writeByte(data);
    return true;
}

uint8_t Ps2Controller::readControlByte() {
    return controlPort.readByte();
}

bool Ps2Controller::writeControlByte(uint8_t data) {
    if (!waitInputBuffer()) {
        return false;
    }

    controlPort.writeByte(data);
    return true;
}

uint8_t Ps2Controller::writeCommand(uint8_t firstByte, uint8_t secondByte) {
    writeControlByte(firstByte);
    if (secondByte != 0x00) {
        writeDataByte(secondByte);
    }

    switch (firstByte) {
        case 0x20 ... 0x3f:
        case 0xa9 ... 0xab:
        case 0xd0:
            return readDataByte();
        default:
            return 0;
    }
}

void Ps2Controller::writeDataToPort(Ps2Controller::Port port, uint8_t data) {
    if (port == FIRST) {
        writeDataByte(data);
    } else if (port == SECOND) {
        writeCommand(WRITE_TO_SECOND_PORT_INPUT_BUFFER, data);
    } else {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Ps2Controller: Invalid port!");
    }
}

void Ps2Controller::enableKeyboardTranslation() {
    auto configuration = writeCommand(READ_CONFIGURATION_BYTE);
    configuration |= FIRST_PORT_TRANSLATION;
    writeCommand(WRITE_CONFIGURATION_BYTE, configuration);
}

bool Ps2Controller::isPortAvailable(Port port) const {
    if (port == FIRST) {
        return firstPortAvailable;
    } else if (port == SECOND) {
        return secondPortAvailable;
    } else {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Ps2Controller: Invalid port!");
    }
}

}