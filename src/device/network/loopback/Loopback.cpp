//
// Created by hannes on 19.04.21.
//

#include <cstdio>
#include "Loopback.h"

void Loopback::sendPacket(void *address, uint16_t length){
    printf("Method sendPacket() has been called");
}

void Loopback::getMacAddress(uint8_t *buf){
    if(buf== nullptr){
        printf("No valid buffer for MAC address given! Exit");
        return;
    }
    for(int i=0;i<6;i++){
        buf[i]=0;
    }
}