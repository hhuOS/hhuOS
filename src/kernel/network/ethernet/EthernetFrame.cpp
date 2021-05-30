//
// Created by hannes on 15.05.21.
//

#include "EthernetFrame.h"

EthernetFrame::EthernetFrame(EthernetAddress *destinationAddress, EthernetDataPart *ethernetDataPart) {
    this->ethernetDataPart = ethernetDataPart;
    destinationAddress->copyTo(header.destinationAddress);

    uint16_t etherTypInt = ethernetDataPart->getEtherTypeAsInt();
    memcpy(header.etherType,&etherTypInt,2);
}

//Private method!
size_t EthernetFrame::getHeaderSizeInBytes() {
    return sizeof(ethHeader_t);
}

uint8_t EthernetFrame::copyDataTo(uint8_t *byteBlock) {
    if(this->ethernetDataPart->getLengthInBytes()>ETHERNETDATAPART_MAX_LENGTH ||
            getHeaderSizeInBytes() > ETHERNETHEADER_MAX_LENGTH ||
            byteBlock == nullptr){
        return 1;
    }
    memcpy(byteBlock,&this->header,getHeaderSizeInBytes());
    //Header bytes have been copied to byte block, increment position to show beginning of data part
    return this->ethernetDataPart->copyDataTo(byteBlock+getHeaderSizeInBytes());
}

uint16_t EthernetFrame::getTotalLengthInBytes() {
    return getHeaderSizeInBytes() + ethernetDataPart->getLengthInBytes();
}

EthernetFrame::EthernetFrame(void *packet, uint16_t length) {
//TODO:Implement parsing from incoming data
}

EthernetDataPart::EtherType EthernetFrame::getEtherType() const {
    return EthernetDataPart::parseIntAsEtherType((uint16_t)*header.etherType);
}

EthernetDataPart *EthernetFrame::getDataPart() const {
    return ethernetDataPart;
}

void EthernetFrame::setSourceAddress(EthernetAddress *sourceAddress) {
    sourceAddress->copyTo(header.sourceAddress);
}
