#include <devices/Pci.h>
#include "PciNode.h"


PciNode::PciNode() : VirtualNode("pci", FsNode::REGULAR_FILE) {

}

uint64_t PciNode::getLength() {

    if (cache.isEmpty()) {
        cacheDeviceList();
    }

    return cache.length();
}

uint64_t PciNode::readData(char *buf, uint64_t pos, uint64_t numBytes) {

    if (cache.isEmpty()) {
        cacheDeviceList();
    }

    uint32_t length = cache.length();

    if (pos + numBytes > length) {
        numBytes = (uint32_t) (length - pos);
    }

    memcpy(buf, (char*) cache + pos, numBytes);

    return numBytes;
}

uint64_t PciNode::writeData(char *buf, uint64_t pos, uint64_t numBytes) {
    return 0;
}

void PciNode::cacheDeviceList() {

    for (auto device : Pci::getDevices()) {
        cache += String::valueOf(device.vendorId, 16) + String(":") + String::valueOf(device.deviceId, 16) + String("\n");
    }
}
