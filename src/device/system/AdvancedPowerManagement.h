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

#ifndef HHUOS_APMMACHINE_H
#define HHUOS_APMMACHINE_H

#include <stdint.h>

#include "Machine.h"
#include "kernel/process/Thread.h"

namespace Kernel {
class GlobalDescriptorTable;
}  // namespace Kernel

namespace Device {

class AdvancedPowerManagement : public Machine {

public:

    enum PowerManagementEvent : uint16_t {
        NONE = 0x0000,
        SYSTEM_STANDBY_REQUEST = 0x0001,
        SYSTEM_SUSPEND_REQUEST = 0x0002,
        NORMAL_RESUME_SYSTEM = 0x0003,
        CRITICAL_RESUME_SYSTEM = 0x0004,
        BATTERY_LOW = 0x0005,
        POWER_STATUS_CHANGE = 0x0006,
        UPDATE_TIME = 0x0007,
        CRITICAL_SYSTEM_SUSPEND = 0x0008,
        USER_SYSTEM_STANDBY_REQUEST = 0x0009,
        USER_SYSTEM_SUSPEND_REQUEST = 0x000a,
        SYSTEM_STANDBY_RESUME = 0x000b,
        CAPABILITIES_CHANGE = 0x000c,
    };

    static AdvancedPowerManagement* initialize();

    /**
     * Copy Constructor.
     */
    AdvancedPowerManagement(const AdvancedPowerManagement &other) = delete;

    /**
     * Assignment operator.
     */
    AdvancedPowerManagement &operator=(const AdvancedPowerManagement &other) = delete;

    /**
     * Destructor.
     */
    ~AdvancedPowerManagement() override = default;

    static bool isAvailable();

    void shutdown() override;

    [[nodiscard]] PowerManagementEvent getLastPowerManagementEvent() const;

private:

    enum ApmFunction : uint8_t {
        INSTALLATION_CHECK = 0x00,
        REAL_MODE_CONNECT = 0x01,
        PROTECTED_MODE_CONNECT_16_BIT = 0x02,
        PROTECTED_MODE_CONNECT_32_BIT = 0x03,
        DISCONNECT = 0x04,
        CPU_IDLE = 0x05,
        CPU_BUSY = 0x06,
        SET_POWER_STATE = 0x07,
        CONFIGURE_POWER_MANAGEMENT = 0x08,
        RESTORE_POWER_ON_DEFAULTS = 0x09,
        GET_POWER_STATUS = 0x0a,
        GET_PM_EVENT = 0x0b,
        GET_POWER_STATE = 0x0c,
        CONFIGURE_DEVICE_POWER_MANAGEMENT = 0x0d,
        SET_APM_DRIVER_VERSION = 0x0e,
        CONFIGURE_POWER_MANAGEMENT_ENGAGEMENT = 0x0f,
        GET_CAPABILITIES = 0x10,
        CONFIGURE_RESUME_TIMER = 0x11,
        CONFIGURE_RESUME_ON_RING = 0x12,
        CONFIGURE_TIMER_BASED_REQUESTS = 0x13,
        OEM_FUNCTION = 0x80
    };

    enum DeviceClass : uint8_t {
        SYSTEM = 0x00,
        DISPLAY = 0x01,
        SECONDARY_STORAGE = 0x02,
        PARALLEL_PORTS = 0x03,
        SERIAL_PORTS = 0x04,
        NETWORK_ADAPTERS = 0x05,
        PCMCIA_SOCKETS = 0x06,
        BATTERIES = 0x80
    };

    enum PowerState : uint8_t {
        STANDBY = 0x01,
        SUSPEND = 0x02,
        OFF = 0x03
    };

    enum ConnectionType {
        REAL_MODE,
        PROTECTED_MODE
    };

    /**
     * Constructor.
     */
    AdvancedPowerManagement(uint8_t apmMajorVersion, uint8_t apmMinorVersion, ConnectionType connectionType);

    [[nodiscard]] bool enablePowerManagement() const;

    static Kernel::Thread::Context callApmFunction(ConnectionType connectionType, AdvancedPowerManagement::ApmFunction function, AdvancedPowerManagement::DeviceClass deviceClass, uint8_t deviceId, uint8_t cl = 0, uint8_t ch = 0);

    uint8_t apmMajorVersion;
    uint8_t apmMinorVersion;
    ConnectionType connectionType;

    // 32-bit protected mode interface
    static Kernel::GlobalDescriptorTable apmGdt;
    static uint32_t entryPoint;

    static const constexpr uint8_t BIOS_INTERRUPT_NUMBER = 0x15;
    static const constexpr uint8_t APM_COMMAND = 0x53;
    static const constexpr uint8_t DEVICE_ID_APM_BIOS = 0x00;
    static const constexpr uint8_t DEVICE_ID_APM_BIOS_ALL = 0x01;
    static const constexpr uint8_t DEVICE_ID_ALL = 0xff;
    static const constexpr uint8_t CARRY_FLAG = 0x01;
};

}

#endif