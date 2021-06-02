//
// Created by hannes on 02.06.21.
//

#ifndef HHUOS_ARPMESSAGE_H
#define HHUOS_ARPMESSAGE_H


#include <kernel/network/NetworkByteBlock.h>

class ARPMessage {
private:
    NetworkByteBlock *input;

public:
    ARPMessage(NetworkByteBlock *input);
    //TODO: Let other ARPMessages extend this one!
};


#endif //HHUOS_ARPMESSAGE_H
