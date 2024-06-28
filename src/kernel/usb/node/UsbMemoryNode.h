#ifndef USB_MEM_NODE__INCLUDE
#define USB_MEM_NODE__INCLUDE

#include "UsbNode.h"
#include "filesystem/memory/MemoryNode.h"

namespace Kernel::Usb {

class UsbMemoryNode : public UsbNode, Filesystem::Memory::MemoryNode {

public:
    UsbMemoryNode(const Util::String& name, uint8_t minor);
};

};

#endif