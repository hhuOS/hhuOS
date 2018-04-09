//
// Created by burak on 09.04.18.
//

#ifndef HHUOS_PFANODE_H
#define HHUOS_PFANODE_H


#include <kernel/memory/SystemManagement.h>
#include "MemoryNode.h"

class PFANode : public MemoryNode {

public:
    PFANode() : MemoryNode("physical", FsNode::REGULAR_FILE) {}

    /**
     * Copy-constructor.
     */
    PFANode(const PFANode &copy) = delete;

    /**
     * Destructor.
     */
    ~PFANode() override = default;

    void getValues() override {
        uint32_t free = SystemManagement::getInstance()->getPageFrameAllocator()->getFreeMemory();
        uint32_t start = SystemManagement::getInstance()->getPageFrameAllocator()->getStartAddress();
        uint32_t end = SystemManagement::getInstance()->getPageFrameAllocator()->getEndAddress();

        cache = String("Physical Memory\n") +
                String("Start: 0x") + String::valueOf(start, 16) +
                String(" End: 0x") + String::valueOf(end, 16) +
                String(" Free: ") + String::valueOf(free, 10) + " Bytes\n";

    }
};


#endif //HHUOS_PFANODE_H
