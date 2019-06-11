#include "PhysicalMemoryNode.h"

PhysicalMemoryNode::PhysicalMemoryNode() : MemoryNode("physical", FsNode::REGULAR_FILE) {

}

void PhysicalMemoryNode::getValues() {
    uint32_t free = Kernel::Management::getInstance().getPageFrameAllocator()->getFreeMemory();
    uint32_t start = Kernel::Management::getInstance().getPageFrameAllocator()->getStartAddress();
    uint32_t end = Kernel::Management::getInstance().getPageFrameAllocator()->getEndAddress();

    cache = String::format("Start: 0x%08x\nEnd: 0x%08x\nFree: %u Bytes\n", start, end, free);
}