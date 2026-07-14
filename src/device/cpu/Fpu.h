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

#ifndef HHUOS_FPU_H
#define HHUOS_FPU_H

#include <stdint.h>

#include "kernel/process/Thread.h"

namespace Device {

class Fpu {

public:

    explicit Fpu(uint8_t *defaultFpuContext) = delete;

    Fpu(const Fpu &other) = delete;

    Fpu& operator=(const Fpu &other) = delete;

    ~Fpu() = default;

    static void initialize();

    static bool isAvailable() {
        return available;
    }

    static uint8_t* createContext();

    static void saveContext(const Kernel::Thread &thread);

    static void restoreContext(const Kernel::Thread &thread);

private:

    static bool checkExistence();

    static bool probeFpu();

    static bool available;
    static bool fxsrAvailable;
    static uint8_t *defaultFpuContext;

    static constexpr size_t FNSAVE_SIZE = 108;
    static constexpr size_t FXSAVE_SIZE = 512;
};

}

#endif
