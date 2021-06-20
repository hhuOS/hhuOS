//
// Created by hannes on 26.05.21.
//

#include <kernel/network/NetworkByteBlock.h>
#include "EthernetDevice.h"

namespace Kernel {
    EthernetDevice::EthernetDevice(EthernetDeviceIdentifier *identifier, NetworkDevice *networkDevice) {
        this->identifier = identifier;
        this->networkDevice = networkDevice;
        this->ethernetAddress = new EthernetAddress(networkDevice);
    }

    EthernetDeviceIdentifier * EthernetDevice::getIdentifier() const {
        return identifier;
    }

    uint8_t EthernetDevice::sendEthernetFrame(EthernetFrame *ethernetFrame) {
        if (ethernetFrame == nullptr) {
            log.error("%s: Outgoing frame was null, ignoring", identifier);
            return 1;
        }
        if (this->networkDevice == nullptr) {
            log.error("%s: Outgoing device was null, discarding frame", identifier);
            return 1;
        }
        //The frame's attributes will be deleted after sending
        //-> copy it here!
        ethernetFrame->setSourceAddress(new EthernetAddress(this->ethernetAddress));
        auto *byteBlock = new NetworkByteBlock(ethernetFrame->getLengthInBytes());

        //ethernetFrame will be deleted in EthernetModule later
        //-> no 'delete ethernetFrame' here!
        if (ethernetFrame->copyTo(byteBlock)) {
            delete byteBlock;
            log.error("%s: Copy to byteBlock failed, discarding frame", identifier);
            return 1;
        }
        if (!byteBlock->isCompletelyFilled()) {
            delete byteBlock;
            log.error("%s: Copy to byteBlock incomplete, discarding frame", identifier);
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
        if (networkDevice == nullptr || otherDevice == nullptr) {
            return false;
        }
        return this->networkDevice == otherDevice;
    }

    String EthernetDevice::asString() {
        if (identifier == nullptr || ethernetAddress == nullptr) {
            return "NULL";
        }
        return "\nID: " + identifier->asString() + ",\nMAC: " + ethernetAddress->asString();
    }

    bool EthernetDevice::equals(EthernetDevice *compare) {
        if (networkDevice == nullptr || compare == nullptr) {
            return false;
        }
        return this->networkDevice == compare->networkDevice;
    }

    EthernetAddress *EthernetDevice::getAddress() {
        return ethernetAddress;
    }

    bool EthernetDevice::sameIdentifierAs(EthernetDeviceIdentifier *other) {
        return this->identifier->equals(other);
    }
}