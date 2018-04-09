//
// Created by burak on 09.04.18.
//

#ifndef HHUOS_KERNELHEAPNODE_H
#define HHUOS_KERNELHEAPNODE_H


#include <kernel/filesystem/RamFs/VirtualNode.h>
#include <kernel/memory/SystemManagement.h>
#include "MemoryNode.h"

class KernelHeapNode : public MemoryNode {

public:
    KernelHeapNode() : MemoryNode("kernelheap", FsNode::REGULAR_FILE) {}

    /**
     * Copy-constructor.
     */
    KernelHeapNode(const KernelHeapNode &copy) = delete;

    /**
     * Destructor.
     */
    ~KernelHeapNode() override = default;

    void getValues() override {
        uint32_t free = SystemManagement::getKernelHeapManager()->getFreeMemory();
        uint32_t start = SystemManagement::getKernelHeapManager()->getStartAddress();
        uint32_t end = SystemManagement::getKernelHeapManager()->getEndAddress();

        cache = String("KernelHeap\n") +
                String("Start: 0x") + String::valueOf(start, 16) +
                String(" End: 0x") + String::valueOf(end, 16) +
                String(" Free: ") + String::valueOf(free, 10) + " Bytes\n";

    }
};


#endif //HHUOS_KERNELHEAPNODE_H
