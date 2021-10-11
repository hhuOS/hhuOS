/*
 * Copyright (C) 2018-2021 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_PROCESS_H
#define HHUOS_PROCESS_H

#include <kernel/paging/VirtualAddressSpace.h>
#include "Thread.h"
#include "ThreadScheduler.h"

namespace Kernel {

class Process {

public:
    /**
     * Constructor.
     */
    explicit Process(ProcessScheduler &scheduler, VirtualAddressSpace &addressSpace);

    /**
     * Copy constructor.
     */
    Process(const Process &other) = delete;

    /**
     * Assignment operator.
     */
    Process &operator=(const Process &other) = delete;

    /**
     * Destructor.
     */
    ~Process();

    void ready(Thread &thread);

    void start();

    [[nodiscard]] uint32_t getId() const;

    VirtualAddressSpace& getAddressSpace();

    ThreadScheduler& getThreadScheduler();

private:

    uint32_t id;
    VirtualAddressSpace &addressSpace;
    ProcessScheduler &scheduler;
    ThreadScheduler threadScheduler;

    static Util::Async::IdGenerator<uint32_t> idGenerator;
};

}

#endif
