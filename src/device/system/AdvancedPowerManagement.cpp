/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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

#include "AdvancedPowerManagement.h"

#include "Bios.h"
#include "lib/util/base/Panic.h"
#include "kernel/log/Log.h"
#include "kernel/process/Thread.h"
#include "device/system/Machine.h"
#include "kernel/memory/GlobalDescriptorTable.h"

namespace Device {

Kernel::GlobalDescriptorTable AdvancedPowerManagement::apmGdt;
uint32_t AdvancedPowerManagement::entryPoint = 0;

AdvancedPowerManagement::AdvancedPowerManagement(uint8_t apmMajorVersion, uint8_t apmMinorVersion, AdvancedPowerManagement::ConnectionType connectionType) :
        apmMajorVersion(apmMajorVersion), apmMinorVersion(apmMinorVersion), connectionType(connectionType) {
    if (apmMajorVersion == 1 && apmMinorVersion > 0) {
        LOG_INFO("Setting APM driver version");
        auto biosReturn = callApmFunction(connectionType, SET_APM_DRIVER_VERSION, SYSTEM, DEVICE_ID_APM_BIOS, apmMinorVersion, apmMajorVersion);
        if (biosReturn.flags & CARRY_FLAG) {
            LOG_WARN("Failed to set APM driver version (Using version 1.0)");
            AdvancedPowerManagement::apmMajorVersion = 1;
            AdvancedPowerManagement::apmMinorVersion = 0;
        }
    }

    LOG_INFO("Enabling power management");
    if (!enablePowerManagement()) {
        LOG_WARN("APM: Failed to enable power management");
    }
}

AdvancedPowerManagement* AdvancedPowerManagement::initialize() {
    // Check APM availability
    auto biosReturn = callApmFunction(REAL_MODE, INSTALLATION_CHECK, SYSTEM, DEVICE_ID_APM_BIOS);
    if (biosReturn.flags & CARRY_FLAG) {
        LOG_ERROR("No APM compatible BIOS available");
        return nullptr;
    }

    if (Bios::get8BitRegister(biosReturn.ebx, Bios::HIGHER) != 'P' || Bios::get8BitRegister(biosReturn.ebx, Bios::LOWER) != 'M') {
        LOG_ERROR("APM BIOS returned invalid values in BX");
        return nullptr;
    }

    ConnectionType connectionType = PROTECTED_MODE;
    auto apmMajorVersion = Bios::get8BitRegister(biosReturn.eax, Bios::HIGHER);
    auto apmMinorVersion = Bios::get8BitRegister(biosReturn.eax, Bios::LOWER);
    LOG_INFO("APM %u.%u compatible BIOS detected", apmMajorVersion, apmMinorVersion);

    // Connect to APM interface
    LOG_INFO("Connecting to 32-bit protected mode APM interface");
    biosReturn = callApmFunction(REAL_MODE, PROTECTED_MODE_CONNECT_32_BIT, SYSTEM, DEVICE_ID_APM_BIOS);
    if (biosReturn.flags & CARRY_FLAG) {
        LOG_WARN("Failed to connect to 32-bit protected mode APM interface", (biosReturn.eax & 0x0000ff00) >> 8);
        LOG_INFO("Connecting to real mode APM interface");
        biosReturn = callApmFunction(REAL_MODE, REAL_MODE_CONNECT, SYSTEM, DEVICE_ID_APM_BIOS);
        if (biosReturn.flags & CARRY_FLAG) {
            LOG_ERROR("Failed to connect to real mode APM interface");
            return nullptr;
        }

        connectionType = REAL_MODE;
    } else {
        apmGdt.addSegment(Kernel::GlobalDescriptorTable::SegmentDescriptor(0x00000000, 0xffffffff, 0x9a, 0x0c)); // Kernel code segment
        apmGdt.addSegment(Kernel::GlobalDescriptorTable::SegmentDescriptor(0x00000000, 0xffffffff, 0x92, 0x0c)); // Kernel data segment
        apmGdt.addSegment(Kernel::GlobalDescriptorTable::SegmentDescriptor((biosReturn.eax & 0x000ffff) << 4, (biosReturn.esi & 0x0000ffff) - 1, 0x9a, 0x0c)); // 32-bit code segment
        apmGdt.addSegment(Kernel::GlobalDescriptorTable::SegmentDescriptor((biosReturn.ecx & 0x000ffff) << 4, ((biosReturn.ecx & 0xffff0000) >> 16) - 1, 0x9a, 0x08)); // 16-bit code segment
        apmGdt.addSegment(Kernel::GlobalDescriptorTable::SegmentDescriptor((biosReturn.edx & 0x000ffff) << 4, (biosReturn.edi & 0x000ffff) - 1, 0x92, 0x0c)); // 32-bit data segment

        entryPoint = biosReturn.ebx;
    }

    return new AdvancedPowerManagement(apmMajorVersion, apmMinorVersion, connectionType);
}

bool AdvancedPowerManagement::isAvailable() {
    if (!Bios::isAvailable()) {
        return false;
    }

    auto biosReturn = callApmFunction(REAL_MODE, INSTALLATION_CHECK, SYSTEM, DEVICE_ID_APM_BIOS);
    if (biosReturn.flags & CARRY_FLAG) {
        return false;
    }

    return true;
}

void AdvancedPowerManagement::shutdown() {
    if (apmMajorVersion == 1 && apmMinorVersion == 0) {
        Machine::shutdown();
    }

    callApmFunction(connectionType, SET_POWER_STATE, SYSTEM, DEVICE_ID_APM_BIOS_ALL, OFF);
}

bool AdvancedPowerManagement::enablePowerManagement() const {
    Kernel::Thread::Context biosReturn{};
    if (apmMajorVersion == 1 && apmMinorVersion == 0) {
        biosReturn = callApmFunction(connectionType, CONFIGURE_POWER_MANAGEMENT, static_cast<DeviceClass>(0xff), DEVICE_ID_ALL);
    } else if (apmMajorVersion == 1 && apmMinorVersion > 0) {
        biosReturn = callApmFunction(connectionType, CONFIGURE_POWER_MANAGEMENT, SYSTEM, DEVICE_ID_APM_BIOS_ALL);
    }

    return (biosReturn.flags & CARRY_FLAG) == 0;
}

AdvancedPowerManagement::PowerManagementEvent AdvancedPowerManagement::getLastPowerManagementEvent() const {
    auto biosReturn = callApmFunction(connectionType, GET_PM_EVENT, SYSTEM, DEVICE_ID_APM_BIOS);
    if (biosReturn.flags & CARRY_FLAG) {
        return NONE;
    }

    return static_cast<PowerManagementEvent>(biosReturn.ebx & 0x0000ffff);
}

Kernel::Thread::Context AdvancedPowerManagement::callApmFunction(ConnectionType connectionType, ApmFunction function, DeviceClass deviceClass, uint8_t deviceId, uint8_t cl, uint8_t ch) {
    Kernel::Thread::Context biosContext{};
    biosContext.eax = Device::Bios::construct16BitRegister(function, APM_COMMAND);
    biosContext.ebx = Device::Bios::construct16BitRegister(deviceId, deviceClass);
    biosContext.ecx = Device::Bios::construct16BitRegister(cl, ch);

    if (connectionType == REAL_MODE) {
        return Device::Bios::interrupt(BIOS_INTERRUPT_NUMBER, biosContext);
    } else if (connectionType == PROTECTED_MODE) {
        return Bios::protectedModeCall(apmGdt, entryPoint, biosContext);
    } else {
        Util::Panic::fire(Util::Panic::ILLEGAL_STATE, "APM: Invalid connection type!");
    }
}

}