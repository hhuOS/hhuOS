//
// Created by hannes on 26.05.21.
//

#include <kernel/network/NetworkByteBlock.h>
#include <kernel/core/Management.h>
#include <kernel/network/NetworkDefinitions.h>
#include "EthernetDevice.h"

namespace Kernel {
    EthernetDevice::EthernetDevice(Management *systemManagement, EthernetDeviceIdentifier *identifier,
                                   NetworkDevice *networkDevice) {
        ETHERNET_MAX_FRAME_SIZE = EthernetHeader::getHeaderLength() + ETHERNET_MTU;

        this->identifier = identifier;

        this->sendLock = new Spinlock();
        sendLock->release();

        this->networkDevice = networkDevice;
        this->ethernetAddress = new EthernetAddress(networkDevice);

        this->systemManagement = systemManagement;
        this->sendBuffer = (uint8_t *)systemManagement->mapIO(ETHERNET_MAX_FRAME_SIZE);

    }

    EthernetDevice::~EthernetDevice() {
        systemManagement->freeIO(sendBuffer);
        delete ethernetAddress;
        delete sendLock;
    }

    EthernetDeviceIdentifier *EthernetDevice::getIdentifier() const {
        return identifier;
    }

    uint8_t EthernetDevice::sendEthernetFrame(EthernetFrame *ethernetFrame) {
        if (ethernetFrame == nullptr) {
            log.error("%s: Outgoing frame was null, ignoring", identifier);
            //ethernetFrame will be deleted in EthernetModule later
            //-> no 'delete ethernetFrame' here!
            return 1;
        }
        if (sendLock == nullptr || sendBuffer == nullptr || systemManagement == nullptr) {
            log.error("%s: sendLock,sendBuffer or memoryManagement was null, discarding frame", identifier);
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

        if(blockLength==0){
            //It's not an error if nothing needs to be done
            delete byteBlock;
            return 0;
        }

        if(blockLength > ETHERNET_MAX_FRAME_SIZE){
            log.error("%s: %d outgoing bytes are too much, discarding frame",identifier, blockLength);
            delete byteBlock;
            return 1;
        }

        sendLock->acquire();

        byteBlock->copyTo(sendBuffer);
        networkDevice->sendPacket(systemManagement->getPhysicalAddress(sendBuffer), blockLength);

        sendLock->release();

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