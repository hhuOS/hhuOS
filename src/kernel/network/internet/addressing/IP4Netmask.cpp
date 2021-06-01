//
// Created by hannes on 15.05.21.
//

#include "IP4Netmask.h"

IP4Netmask::IP4Netmask(uint8_t bitCount) {
    this->bitCount = bitCount;
    if (this->bitCount > 32) {
        this->bitCount = 32;
    }
    uint8_t fullByteCount= bitCount / 8;
    for(uint8_t i=0; i <= fullByteCount; i++){
        //Set all Bytes with at least one '1' to its maximum of 255,
        //the trick with '-1' works because each Byte is unsigned!
        this->netmask[i]=-1;
    }

    //Check if one Byte is not completely filled with '1'
    uint8_t remainingBits = bitCount % 8;
    if(remainingBits!=0){
        //If one Byte is not completely filled with '1', bit shift it left for the number of '0' in it
        //->this sets all other bits to '0', our netmask is filled with exactly [bitCount] bits now
        this->netmask[fullByteCount]=this->netmask[fullByteCount]<<(8-remainingBits);
    }
}

IP4Address *IP4Netmask::extractNetPart(IP4Address *ip4Address) {
    return ip4Address->calculateAND(this->netmask);
}

uint8_t IP4Netmask::getBitCount() const {
    return this->bitCount;
}

String IP4Netmask::asString() {
    return String::format("%d.%d.%d.%d /%d", netmask[0], netmask[1], netmask[2], netmask[3], bitCount);
}
