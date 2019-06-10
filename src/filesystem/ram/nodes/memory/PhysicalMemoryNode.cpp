#include "PhysicalMemoryNode.h"

PhysicalMemoryNode::PhysicalMemoryNode() : MemoryNode("physical", FsNode::REGULAR_FILE) {

}

void PhysicalMemoryNode::getValues() {
    uint32_t free = SystemManagement::getInstance().getPageFrameAllocator()->getFreeMemory();
    uint32_t start = SystemManagement::getInstance().getPageFrameAllocator()->getStartAddress();
    uint32_t end = SystemManagement::getInstance().getPageFrameAllocator()->getEndAddress();

    cache = String::format("Start: 0x%08x\nEnd: 0x%08x\nFree: %u Bytes\n", start, end, free);
}