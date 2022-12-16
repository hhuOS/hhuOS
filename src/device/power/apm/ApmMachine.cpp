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

#include "ApmMachine.h"

#include "device/bios/Bios.h"
#include "kernel/log/Logger.h"
#include "lib/util/Exception.h"

namespace Device {

Kernel::Logger ApmMachine::log = Kernel::Logger::get("APM");

ApmMachine::ApmMachine() {
    // Check APM availability
    auto biosReturn = callApmFunction(INSTALLATION_CHECK, SYSTEM, DEVICE_ID_APM_BIOS);
    if ((biosReturn.flags & CARRY_FLAG) != 0) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "APM: No APM compatible BIOS available!");
    }

    if (Bios::get8BitRegister(biosReturn.bx, Bios::HIGHER) != 'P' || Bios::get8BitRegister(biosReturn.bx, Bios::LOWER) != 'M') {
        log.warn("APM seems to be available, but BX contains invalid values");
    }

    apmMajorVersion = Bios::get8BitRegister(biosReturn.ax, Bios::HIGHER);
    apmMinorVersion = Bios::get8BitRegister(biosReturn.ax, Bios::LOWER);
    log.info("APM %u.%u compatible BIOS detected", apmMajorVersion, apmMinorVersion);

    // Connect to real mode interface
    log.info("Connecting to APM real mode interface");
    biosReturn = callApmFunction(REAL_MODE_CONNECT, SYSTEM, DEVICE_ID_APM_BIOS);
    if ((biosReturn.flags & CARRY_FLAG) != 0) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "APM: Unable to connect to real mode interface!");
    }

    // Set APM version
    log.info("Setting APM driver version");
    if (apmMajorVersion == 1 && apmMinorVersion > 0) {
        biosReturn = callApmFunction(SET_APM_DRIVER_VERSION, SYSTEM, DEVICE_ID_APM_BIOS, apmMinorVersion, apmMajorVersion);
        if ((biosReturn.flags & CARRY_FLAG) != 0) {
            log.warn("Unable to set APM driver version (Using version 1.0)");
            apmMajorVersion = 1;
            apmMinorVersion = 0;
        }
    }

    // Enable Power Management
    log.info("Enabling power management");
    if (!enablePowerManagement()) {
        callApmFunction(DISCONNECT, SYSTEM, DEVICE_ID_APM_BIOS);
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "APM: Unable to enable power management");
    }
}

bool ApmMachine::isAvailable() {
    if (!Device::Bios::isAvailable()) {
        return false;
    }

    auto biosReturn = callApmFunction(INSTALLATION_CHECK, SYSTEM, DEVICE_ID_APM_BIOS);
    if ((biosReturn.flags & CARRY_FLAG) != 0) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "APM: No APM compatible BIOS available!");
    }

    if ((biosReturn.flags & CARRY_FLAG) != 0) {
        return false;
    }

    return true;
}

void ApmMachine::shutdown() {
    if (apmMajorVersion == 1 && apmMinorVersion == 0) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "APM: Setting power state of all devices is not supported in APM 1.0");
    }

    callApmFunction(SET_POWER_STATE, SYSTEM, DEVICE_ID_APM_BIOS_ALL, OFF);
}

bool ApmMachine::enablePowerManagement() const {
    Bios::RealModeContext biosReturn{};
    if (apmMajorVersion == 1 && apmMinorVersion == 0) {
        biosReturn = callApmFunction(CONFIGURE_POWER_MANAGEMENT, static_cast<DeviceClass>(0xff), DEVICE_ID_ALL);
    } else if (apmMajorVersion == 1 && apmMinorVersion > 0) {
        biosReturn = callApmFunction(CONFIGURE_POWER_MANAGEMENT, SYSTEM, DEVICE_ID_APM_BIOS_ALL);
    }

    return (biosReturn.flags & CARRY_FLAG) == 0;
}

Bios::RealModeContext ApmMachine::callApmFunction(ApmMachine::ApmFunction function, ApmMachine::DeviceClass deviceClass, uint8_t deviceId, uint8_t cl, uint8_t ch) {
    Bios::RealModeContext biosParameters{};
    biosParameters.ax = Device::Bios::construct16BitRegister(function, APM_COMMAND);
    biosParameters.bx = Device::Bios::construct16BitRegister(deviceId, deviceClass);
    biosParameters.cx = Device::Bios::construct16BitRegister(cl, ch);
    return Device::Bios::interrupt(0x15, biosParameters);
}

}