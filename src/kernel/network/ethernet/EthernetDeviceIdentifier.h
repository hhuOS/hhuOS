//
// Created by hannes on 20.06.21.
//

#ifndef HHUOS_ETHERNETDEVICEIDENTIFIER_H
#define HHUOS_ETHERNETDEVICEIDENTIFIER_H


#include <lib/string/String.h>

class EthernetDeviceIdentifier {
private:
    char *characters = nullptr;
    uint8_t length = 0;

public:
    explicit EthernetDeviceIdentifier(String *string);

    explicit EthernetDeviceIdentifier(uint8_t deviceIndex);

    char *getCharacters();

    String asString();

    virtual ~EthernetDeviceIdentifier();

    bool equals(EthernetDeviceIdentifier *other);
};


#endif //HHUOS_ETHERNETDEVICEIDENTIFIER_H
