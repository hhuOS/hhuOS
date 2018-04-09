//
// Created by burak on 09.04.18.
//

#ifndef HHUOS_PAGINGAREANODE_H
#define HHUOS_PAGINGAREANODE_H


#include <kernel/memory/SystemManagement.h>
#include "MemoryNode.h"

class PagingAreaNode : public MemoryNode {

public:
    PagingAreaNode() : MemoryNode("pagingarea", FsNode::REGULAR_FILE) {}

    /**
     * Copy-constructor.
     */
    PagingAreaNode(const PagingAreaNode &copy) = delete;

    /**
     * Destructor.
     */
    ~PagingAreaNode() override = default;

    void getValues() override {
        uint32_t free = SystemManagement::getInstance()->getPagingAreaManager()->getFreeMemory();
        uint32_t start = SystemManagement::getInstance()->getPagingAreaManager()->getStartAddress();
        uint32_t end = SystemManagement::getInstance()->getPagingAreaManager()->getEndAddress();

        cache = String("Paging Area\n") +
                String("Start: 0x") + String::valueOf(start, 16) +
                String(" End: 0x") + String::valueOf(end, 16) +
                String(" Free: ") + String::valueOf(free, 10) + " Bytes\n";

    }
};


#endif //HHUOS_PAGINGAREANODE_H
