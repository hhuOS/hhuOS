#include "PagingAreaNode.h"

PagingAreaNode::PagingAreaNode() : MemoryNode("pagingarea", FsNode::REGULAR_FILE) {

}

void PagingAreaNode::getValues() {
    uint32_t free = SystemManagement::getInstance().getPagingAreaManager()->getFreeMemory();
    uint32_t start = SystemManagement::getInstance().getPagingAreaManager()->getStartAddress();
    uint32_t end = SystemManagement::getInstance().getPagingAreaManager()->getEndAddress();

    cache = String::format("Start: 0x%08x\nEnd: 0x%08x\nFree: %u Bytes\n", start, end, free);
}