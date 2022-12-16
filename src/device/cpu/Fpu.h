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

#ifndef HHUOS_FPU_H
#define HHUOS_FPU_H

#include <cstdint>

#include "kernel/interrupt/InterruptHandler.h"

namespace Kernel {
class Logger;
class Thread;
struct InterruptFrame;
}  // namespace Kernel

namespace Device {

class Fpu : public Kernel::InterruptHandler {

public:
    /**
     * Constructor.
     */
    explicit Fpu(const uint8_t *defaultFpuContext);

    /**
     * Copy Constructor.
     */
    Fpu(const Fpu &other) = delete;

    /**
     * Assignment operator.
     */
    Fpu &operator=(const Fpu &other) = delete;

    /**
     * Destructor.
     */
    ~Fpu() override = default;

    void plugin() override;

    void trigger(const Kernel::InterruptFrame &frame) override;

    void checkTerminatedThread(Kernel::Thread &thread);

    static bool isAvailable();

    static bool isFxsrAvailable();

    static void armFpuMonitor();

    static void disarmFpuMonitor();

private:

    void switchContext(Kernel::Thread &currentThread);

    void switchContextFpuOnly(Kernel::Thread &currentThread);

    static bool probeFpu();

    bool fxsrAvailable = isFxsrAvailable();
    Kernel::Thread *lastFpuThread = nullptr;

    static Kernel::Logger log;
};

}

#endif
