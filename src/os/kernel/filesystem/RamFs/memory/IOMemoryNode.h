//
// Created by burak on 09.04.18.
//

#ifndef HHUOS_IOMEMORYNODE_H
#define HHUOS_IOMEMORYNODE_H


#include <kernel/memory/SystemManagement.h>
#include "MemoryNode.h"

class IOMemoryNode : public MemoryNode {

public:
    IOMemoryNode() : MemoryNode("iomemory", FsNode::REGULAR_FILE) {}

    /**
     * Copy-constructor.
     */
    IOMemoryNode(const IOMemoryNode &copy) = delete;

    /**
     * Destructor.
     */
    ~IOMemoryNode() override = default;

    void getValues() override {
        uint32_t free = SystemManagement::getInstance()->getIOMemoryManager()->getFreeMemory();
        uint32_t start = SystemManagement::getInstance()->getIOMemoryManager()->getStartAddress();
        uint32_t end = SystemManagement::getInstance()->getIOMemoryManager()->getEndAddress();

        cache = String("IO-Memory\n") +
                String("Start: 0x") + String::valueOf(start, 16) +
                String(" End: 0x") + String::valueOf(end, 16) +
                String(" Free: ") + String::valueOf(free, 10) + " Bytes\n";

    }
};


#endif //HHUOS_IOMEMORYNODE_H
