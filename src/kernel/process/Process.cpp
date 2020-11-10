#include <lib/file/elf/Elf.h>
#include <kernel/core/Management.h>
#include "Process.h"
#include "ProcessScheduler.h"

namespace Kernel {

IdGenerator Process::idGenerator;

Process::Process(const Kernel::VirtualAddressSpace &addressSpace) : id(idGenerator.getId()), addressSpace(addressSpace) {

}

Process *Process::loadExecutable(const String &path) {
    auto elf = Elf::load(path);

    if (elf == nullptr) {
        return nullptr;
    }

    auto addressSpace = Kernel::Management::getInstance().createAddressSpace(elf->getSizeInMemory(),
                                                                             "FreeListMemoryManager");

    return new Process(*addressSpace);
}

uint8_t Process::getPriority() {
    return priority;
}

uint32_t Process::getPid(){
    return id;
}

void Process::setPriority(uint8_t priority) {
    this->priority = ProcessScheduler::getInstance().changePriority(*this, priority);
}

bool Process::operator==(const Process &other) {
    return id == other.id;
}

}