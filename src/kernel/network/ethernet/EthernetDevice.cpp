//
// Created by hannes on 26.05.21.
//

#include <kernel/network/NetworkByteBlock.h>
#include "EthernetDevice.h"
#include "EthernetModule.h"

EthernetDevice::EthernetDevice(String *identifier, NetworkDevice *networkDevice) {
    this->identifier = identifier;
    this->networkDevice = networkDevice;
    this->ethernetAddress = new EthernetAddress(networkDevice);
}

String *EthernetDevice::getIdentifier() const {
    return identifier;
}

uint8_t EthernetDevice::sendEthernetFrame(EthernetFrame *ethernetFrame) {
    if (ethernetFrame == nullptr) {
        return ETH_FRAME_NULL;
    }
    if (this->networkDevice == nullptr) {
        return ETH_DEVICE_NULL;
    }
    //interface selection happens in routing module
    // -> we don't know source address before this point here!
    ethernetFrame->setSourceAddress(this->ethernetAddress);
    auto *byteBlock = new NetworkByteBlock(ethernetFrame->getLengthInBytes());

    //ethernetFrame will be deleted in EthernetModule later
    //-> no 'delete ethernetFrame' here!
    if (ethernetFrame->copyTo(byteBlock)) {
        delete byteBlock;
        return ETH_COPY_BYTEBLOCK_FAILED;
    }
    if (!byteBlock->isCompletelyFilled()) {
        delete byteBlock;
        return ETH_COPY_BYTEBLOCK_INCOMPLETE;
    }

    uint8_t byteBlockError = byteBlock->sendOutVia(this->networkDevice);
    if (byteBlockError) {
        delete byteBlock;
        return byteBlockError;
    }

    delete byteBlock;
    return ETH_DELIVER_SUCCESS;
}

bool EthernetDevice::connectedTo(NetworkDevice *otherDevice) {
    return this->networkDevice == otherDevice;
}

String EthernetDevice::asString() {
    return "\nID: " + *identifier + ",\nMAC: " + ethernetAddress->asString();
}

bool EthernetDevice::equals(EthernetDevice *compare) {
    return this->networkDevice == compare->networkDevice;
}

EthernetAddress *EthernetDevice::getAddress() {
    return ethernetAddress;
}
