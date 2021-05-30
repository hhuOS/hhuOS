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

void EthernetDevice::sendEthernetFrame(EthernetFrame *ethernetFrame) {
    if(ethernetFrame== nullptr){
        return;
    }
    ethernetFrame->setSourceAddress(this->ethernetAddress);
    uint16_t frameLength = ethernetFrame->getTotalLengthInBytes();
    if(frameLength==0){
        log.error("EthernetFrame had zero length, discarding frame");
        return;
    }
    auto *byteBlock=new NetworkByteBlock(frameLength);
    if(byteBlock->isNull()){
        log.error("Could not allocate byteBlock, discarding frame");
        delete ethernetFrame;
        return;
    }
    if(ethernetFrame->copyDataTo(byteBlock)){
        log.error("Could not copy EthernetFrame data to byteBlock, discarding frame");
        byteBlock->freeBytes();
        delete ethernetFrame;
        return;
    }
    if(byteBlock->getCurrentIndex()!=(frameLength-1)){
        log.error("Could not completely copy EthernetFrame data to byteBlock, discarding frame");
        byteBlock->freeBytes();
        delete ethernetFrame;
        return;
    }
    this->networkDevice->sendPacket(byteBlock->getBytes(),frameLength);
    byteBlock->freeBytes();
    delete ethernetFrame;
}

uint8_t EthernetDevice::connectedTo(NetworkDevice *networkDevice) {
    return this->networkDevice == networkDevice;
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
