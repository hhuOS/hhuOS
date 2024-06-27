#include "UsbMemoryNode.h"

Kernel::Usb::UsbMemoryNode::UsbMemoryNode(const Util::String& name, uint8_t minor) :
    UsbNode(minor), Filesystem::Memory::MemoryNode(name) {}