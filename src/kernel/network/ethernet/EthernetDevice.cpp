//
// Created by hannes on 26.05.21.
//

#include <kernel/network/NetworkByteBlock.h>
#include "EthernetDevice.h"

namespace Kernel {
    EthernetDevice::EthernetDevice(uint8_t *sendBuffer, EthernetDeviceIdentifier *identifier,
                                   NetworkDevice *networkDevice) {
        this->identifier = identifier;

        this->sendLock = new Spinlock();
        sendLock->release();

        this->networkDevice = networkDevice;
        this->ethernetAddress = new EthernetAddress(networkDevice);

        this->sendBuffer = sendBuffer;
    }

    EthernetDevice::EthernetDevice(uint8_t *sendBuffer, void *physicalBufferAddress,
                                   EthernetDeviceIdentifier *identifier,
                                   NetworkDevice *networkDevice) {
        this->identifier = identifier;

        this->sendLock = new Spinlock();
        sendLock->release();

        this->networkDevice = networkDevice;
        this->ethernetAddress = new EthernetAddress(networkDevice);

        this->sendBuffer = sendBuffer;
        this->physicalBufferAddress = physicalBufferAddress;
    }

    EthernetDevice::~EthernetDevice() {
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
        if (sendLock == nullptr || sendBuffer == nullptr || this->networkDevice == nullptr) {
            log.error("%s: sendLock, sendBuffer or outgoing device was null, discarding frame", identifier);
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
        //Reset currentIndex to zero to prepare reading all content to sendBuffer
        byteBlock->resetIndex();

        size_t blockLength = byteBlock->getLength();

        if (blockLength == 0) {
            //It's not an error if nothing needs to be done
            delete byteBlock;
            return 0;
        }

        if (blockLength > EthernetHeader::getMaximumFrameLength()) {
            log.error("%s: %d outgoing bytes are too much, discarding frame", identifier, blockLength);
            delete byteBlock;
            return 1;
        }

        sendLock->acquire();

        if (byteBlock->readStraightTo(sendBuffer, blockLength)) {
            sendLock->release();
            log.error("%s: Could not copy outgoing data to sendBuffer, discarding frame", identifier);
            delete byteBlock;
            return 1;
        }

        if (physicalBufferAddress != nullptr) {
            networkDevice->sendPacket(physicalBufferAddress, static_cast<uint16_t>(blockLength));
        } else {
            networkDevice->sendPacket(sendBuffer, static_cast<uint16_t>(blockLength));
        }
        memset(sendBuffer, 0, blockLength);

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

    void *EthernetDevice::getPhysicalBufferAddress() const {
        return physicalBufferAddress;
    }

    uint8_t *EthernetDevice::getSendBuffer() const {
        return sendBuffer;
    }
}