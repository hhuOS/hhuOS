#include "KernelHeapNode.h"

KernelHeapNode::KernelHeapNode() : MemoryNode("kernelheap", FsNode::REGULAR_FILE) {

}

void KernelHeapNode::getValues() {
    uint32_t free = Kernel::Management::getKernelHeapManager()->getFreeMemory();
    uint32_t start = Kernel::Management::getKernelHeapManager()->getStartAddress();
    uint32_t end = Kernel::Management::getKernelHeapManager()->getEndAddress();

    cache = String::format("Start: 0x%08x\nEnd: 0x%08x\nFree: %u Bytes\n", start, end, free);
}