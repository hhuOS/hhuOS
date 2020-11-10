#ifndef HHUOS_PROCESS_H
#define HHUOS_PROCESS_H

#include <lib/system/IdGenerator.h>
#include "kernel/memory/VirtualAddressSpace.h"

namespace Kernel {

class Process {

private:

    static IdGenerator idGenerator;

    uint32_t id;
    uint8_t priority = 0xff;

    const Kernel::VirtualAddressSpace &addressSpace;

private:
    // uint32_t id;
    explicit Process(const Kernel::VirtualAddressSpace &addressSpace);

public:
    // uint32_t id;
    static Process *loadExecutable(const String &path);

    Process(const Process &other) = delete;

    Process &operator=(const Process &other) = delete;

    ~Process() = default;

    uint32_t getPid();

    uint8_t getPriority();

    void setPriority(uint8_t priority);

    bool operator==(const Process &other);
};

}

#endif
