//
// Created by hannes on 26.05.21.
//

#include <kernel/network/NetworkByteBlock.h>
#include "EthernetDevice.h"
#include "EthernetModule.h"

namespace Kernel {
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
            log.error("%s: Outgoing frame was null, ignoring", identifier);
            return 1;
        }
        if (this->networkDevice == nullptr) {
            log.error("%s: Outgoing device was null, discarding frame", identifier);
            return 1;
        }
        //interface selection happens in routing module
        // -> we don't know source address before this point here!
        ethernetFrame->setSourceAddress(this->ethernetAddress);
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

        uint8_t byteBlockError = byteBlock->sendOutVia(this->networkDevice);
        if (byteBlockError) {
            delete byteBlock;
            return byteBlockError;
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
}