#include "IoMemoryNode.h"

IoMemoryNode::IoMemoryNode() : MemoryNode("iomemory", FsNode::REGULAR_FILE) {

}

void IoMemoryNode::getValues() {
    uint32_t free = Kernel::Management::getInstance().getIOMemoryManager()->getFreeMemory();
    uint32_t start = Kernel::Management::getInstance().getIOMemoryManager()->getStartAddress();
    uint32_t end = Kernel::Management::getInstance().getIOMemoryManager()->getEndAddress();

    cache = String::format("Start: 0x%08x\nEnd: 0x%08x\nFree: %u Bytes\n", start, end, free);
}