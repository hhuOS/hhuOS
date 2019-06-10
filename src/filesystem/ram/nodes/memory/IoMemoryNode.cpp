#include "IoMemoryNode.h"

IoMemoryNode::IoMemoryNode() : MemoryNode("iomemory", FsNode::REGULAR_FILE) {

}

void IoMemoryNode::getValues() {
    uint32_t free = SystemManagement::getInstance().getIOMemoryManager()->getFreeMemory();
    uint32_t start = SystemManagement::getInstance().getIOMemoryManager()->getStartAddress();
    uint32_t end = SystemManagement::getInstance().getIOMemoryManager()->getEndAddress();

    cache = String::format("Start: 0x%08x\nEnd: 0x%08x\nFree: %u Bytes\n", start, end, free);
}