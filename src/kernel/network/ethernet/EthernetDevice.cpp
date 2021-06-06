//
// Created by hannes on 26.05.21.
//

#include <kernel/network/NetworkByteBlock.h>
#include "EthernetDevice.h"

EthernetDevice::EthernetDevice(String *identifier, NetworkDevice *networkDevice) {
    this->identifier = identifier;
    this->networkDevice = networkDevice;
    this->ethernetAddress = new EthernetAddress(networkDevice);
}

String *EthernetDevice::getIdentifier() const {
    return identifier;
}

uint8_t EthernetDevice::sendEthernetFrame(EthernetFrame *ethernetFrame) {
    if (ethernetFrame == nullptr || this->networkDevice == nullptr) {
        return 1;
    }
    //interface selection happens in routing module
    // -> we don't know source address before this point here!
    ethernetFrame->setSourceAddress(this->ethernetAddress);
    uint16_t frameLength = ethernetFrame->getLengthInBytes();
    if (frameLength == 0) {
        return 1;
    }
    auto *byteBlock = new NetworkByteBlock(frameLength);
    if (ethernetFrame->copyTo(byteBlock) ||
        !byteBlock->isCompletelyFilled()
            ) {
        //ethernetFrame will be deleted in EthernetModule later
        //-> no delete here!
        delete byteBlock;
        return 1;
    }
    if (byteBlock->sendOutVia(this->networkDevice)) {
        delete byteBlock;
        return 1;
    }

    delete byteBlock;
    return 0;
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
