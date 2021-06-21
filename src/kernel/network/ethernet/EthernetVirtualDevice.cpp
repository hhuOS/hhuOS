//
// Created by hannes on 21.06.21.
//

#include "EthernetVirtualDevice.h"

Kernel::EthernetVirtualDevice::EthernetVirtualDevice(Management *systemManagement, EthernetDeviceIdentifier *identifier,
                                                     NetworkDevice *networkDevice) : EthernetDevice(systemManagement,
                                                                                                    identifier,
                                                                                                    networkDevice) {

}

Kernel::EthernetVirtualDevice::~EthernetVirtualDevice() {
    delete ethernetAddress;
    delete sendBuffer;
    delete sendLock;
}

uint8_t Kernel::EthernetVirtualDevice::sendEthernetFrame(EthernetFrame *ethernetFrame) {
    if (ethernetFrame == nullptr) {
        log.error("%s: Outgoing frame was null, ignoring", identifier);
        //ethernetFrame will be deleted in EthernetModule later
        //-> no 'delete ethernetFrame' here!
        return 1;
    }
    if (this->networkDevice == nullptr) {
        log.error("%s: Outgoing device was null, discarding frame", identifier);
        //ethernetFrame will be deleted in EthernetModule later
        //-> no 'delete ethernetFrame' here!
        return 1;
    }
    if (sendLock == nullptr || sendBuffer == nullptr) {
        log.error("%s: sendLock or sendBuffer  was null, discarding frame", identifier);
        //ethernetFrame will be deleted in EthernetModule later
        //-> no 'delete ethernetFrame' here!
        return 1;
    }
    //The frame's attributes will be deleted after sending
    //-> copy it here!
    ethernetFrame->setSourceAddress(new EthernetAddress(this->ethernetAddress));
    auto *byteBlock = new NetworkByteBlock(ethernetFrame->getLengthInBytes());

    //ethernetFrame will be deleted in EthernetModule later
    //-> no 'delete ethernetFrame' here!
    if (ethernetFrame->copyTo(byteBlock)) {
        log.error("%s: Copy to byteBlock failed, discarding frame", identifier);
        delete byteBlock;
        return 1;
    }
    if (!byteBlock->isCompletelyFilled()) {
        log.error("%s: Copy to byteBlock incomplete, discarding frame", identifier);
        delete byteBlock;
        return 1;
    }

    size_t blockLength = byteBlock->getLength();

    if (blockLength == 0) {
        //It's not an error if nothing needs to be done
        delete byteBlock;
        return 0;
    }

    if (blockLength > ETHERNET_MAX_FRAME_SIZE) {
        log.error("%s: %d outgoing bytes are too much, discarding frame", identifier, blockLength);
        delete byteBlock;
        return 1;
    }

    sendLock->acquire();

    byteBlock->copyTo(sendBuffer);
    networkDevice->sendPacket(sendBuffer, blockLength);

    sendLock->release();

    delete byteBlock;
    return 0;
}
