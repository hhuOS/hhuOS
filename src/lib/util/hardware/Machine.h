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

#ifndef HHUOS_LIB_UTIL_HARDWARE_MACHINE_H
#define HHUOS_LIB_UTIL_HARDWARE_MACHINE_H

#include <stdint.h>

namespace Util {
namespace Hardware {

/// Provides functions to configure the machine, such as shutting it down or performing power management operations.
/// For now, this only provides a `shutdown()` function.
/// Currently, hhuOS only supports shutdown via QEMU isa-debug-exit or the Advanced Power Management (APM).
/// When running on real hardware and APM is not available, the shutdown function will not work.
/// Since most modern systems use ACPI for power management, a shutdown function using ACPI is planned for the future.
namespace Machine {

/// Type of shutdown operation to perform (either shutdown or reboot).
enum ShutdownType : uint8_t {
    SHUTDOWN = 0x01,
    REBOOT = 0x02
};

/// Shut down or reboot the machine.
bool shutdown(ShutdownType type = SHUTDOWN);

}

}
}

#endif
