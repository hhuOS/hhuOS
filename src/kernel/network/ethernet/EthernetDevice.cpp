//
// Created by hannes on 26.05.21.
//

#include <kernel/network/DebugPrintout.h>
#include <kernel/network/NetworkByteBlock.h>
#include "EthernetDevice.h"

namespace Kernel {

    EthernetDevice::EthernetDevice(const String &identifier, NetworkDevice *networkDevice, uint8_t *sendBuffer,
                                   void *physicalBufferAddress) {
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

    uint8_t EthernetDevice::sendEthernetFrame(EthernetFrame *ethernetFrame) {
        if (ethernetFrame == nullptr) {
            log.error("%s: Outgoing frame was null, ignoring", (char *) this->identifier);
            //ethernetFrame will be deleted in EthernetModule later
            //-> no 'delete ethernetFrame' here!
            return 1;
        }
        if (sendLock == nullptr || sendBuffer == nullptr || this->networkDevice == nullptr) {
            log.error("%s: sendLock, sendBuffer or outgoing device was null, discarding frame",
                      (char *) this->identifier);
            //ethernetFrame will be deleted in EthernetModule later
            //-> no 'delete ethernetFrame' here!
            return 1;
        }
        //The frame's attributes will be deleted after sending
        //-> copy it here!
        auto *addressCopy = new EthernetAddress(this->ethernetAddress);
        if (ethernetFrame->setSourceAddress(addressCopy)) {
            log.error("%s: Could not set frame's source address, discarding", (char *) this->identifier);
            return 1;
        }
        auto *output = new NetworkByteBlock(ethernetFrame->length());

        //ethernetFrame will be deleted in EthernetModule later
        //-> no 'delete ethernetFrame' here!
        if (ethernetFrame->copyTo(output)) {
            log.error("%s: Copy to byteBlock failed, discarding frame",
                      (char *) this->identifier);
            delete output;
            return 1;
        }
        if (output->bytesRemaining() != 0) {
            log.error("%s: Copy to byteBlock incomplete, discarding frame",
                      (char *) this->identifier);
            delete output;
            return 1;
        }
        //Reset currentIndex to zero to prepare reading all content to sendBuffer
        if (output->resetIndex()) {
            log.error("%s: Index reset for output byteBlock failed, discarding",
                      (char *) this->identifier);
            delete output;
            return 1;
        }

        uint16_t blockLength = output->getLength();

        if (blockLength == 0) {
            //It's not an error if nothing needs to be done
            delete output;
            return 0;
        }

        if (blockLength > EthernetHeader::getMaximumFrameLength()) {
            log.error("%s: %d outgoing bytes are too much, discarding frame",
                      (char *) this->identifier, blockLength);
            delete output;
            return 1;
        }

#if PRINT_OUTGOING_BYTES == 1
        printf("\nOutgoing Bytes (%d per line, %d total):\n", BYTES_PER_LINE, blockLength);
        output->printBytes(0, blockLength - (uint16_t) 1, BYTES_PER_LINE);
#endif
        sendLock->acquire();

        if (output->readStraightTo(sendBuffer, blockLength)) {
            sendLock->release();
            log.error("%s: Could not copy outgoing data to sendBuffer, discarding frame",
                      (char *) this->identifier);
            delete output;
            return 1;
        }

        //Send via MMIO if we are a physical device
        if (physicalBufferAddress != nullptr) {
            networkDevice->sendPacket(physicalBufferAddress, blockLength);
        } else {
            //Send directly inMemory if we are a virtual device (like Loopback)
            networkDevice->sendPacket(sendBuffer, blockLength);
        }
        memset(sendBuffer, 0, blockLength);

        sendLock->release();

        delete output;
        return 0;
    }

    bool EthernetDevice::connectedTo(NetworkDevice *otherDevice) {
        if (networkDevice == nullptr || otherDevice == nullptr) {
            return false;
        }
        return this->networkDevice == otherDevice;
    }

    String EthernetDevice::asString() {
        if (this->identifier.isEmpty() || ethernetAddress == nullptr) {
            return "NULL";
        }
        return "\n    ID:  '" + this->identifier + "',\n    MAC: " + String(ethernetAddress->asChars());
    }

    bool EthernetDevice::equals(EthernetDevice *compare) {
        if (networkDevice == nullptr || compare == nullptr) {
            return false;
        }
        return this->networkDevice == compare->networkDevice;
    }

    bool EthernetDevice::sameIdentifierAs(const String &other) {
        return this->identifier == other;
    }

    bool EthernetDevice::isDestinationOf(EthernetHeader *ethernetHeader) {
        if (this->ethernetAddress == nullptr || ethernetHeader == nullptr) {
            return false;
        }
        return ethernetHeader->destinationIs(this->ethernetAddress);
    }

    uint8_t EthernetDevice::copyAddressTo(uint8_t *target) {
        if (this->ethernetAddress == nullptr || target == nullptr) {
            return 1;
        }
        return this->ethernetAddress->copyTo(target);
    }

    uint8_t EthernetDevice::copyIdentifierTo(String *target) {
        if (this->identifier.isEmpty() || target == nullptr || !(*target).isEmpty()) {
            return 1;
        }
        *target += this->identifier;
        return 0;
    }
}