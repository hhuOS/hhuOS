/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_CPUSERVICE_H
#define HHUOS_CPUSERVICE_H

#include "kernel/memory/GlobalDescriptorTable.h"
#include "Service.h"

namespace Kernel {

class CpuService : public Service {

public:
    /**
     * Constructor.
     */
    CpuService();

    /**
     * Copy Constructor.
     */
    CpuService(const CpuService &other) = delete;

    /**
     * Assignment operator.
     */
    CpuService &operator=(const CpuService &other) = delete;

    /**
     * Destructor.
     */
    ~CpuService() override = default;

    void loadGdt();

    void setTssStackEntry(const uint32_t *stackPointer);

    void startupApplicationProcessors();

    uint8_t getVirtualCpuId();

    static const constexpr uint8_t SERVICE_ID = 10;

private:

    uint8_t registerCpu(uint8_t localApicId);

    uint8_t getCoreCount();

    void prepareGdts();

    void prepareStacks();

    void prepareApplicationProcessorStartupCode();

    void prepareApplicationProcessorWarmReset();

    uint8_t getLocalApicId();

    uint8_t getVirtualCpuId(uint8_t localApicId);

    uint8_t* getStack(uint8_t cpuId);

    uint8_t virtualCpuCounter = 0;
    uint8_t virtualCpuIds[256] = {};
    GlobalDescriptorTable *gdt = nullptr;
    GlobalDescriptorTable::TaskStateSegment *tss = nullptr;
    GlobalDescriptorTable::Descriptor *gdtDescriptor = nullptr;
    uint8_t *stacks = nullptr;
};

}

#endif
