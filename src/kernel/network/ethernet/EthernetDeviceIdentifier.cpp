//
// Created by hannes on 20.06.21.
//

#include "EthernetDeviceIdentifier.h"

EthernetDeviceIdentifier::EthernetDeviceIdentifier(String *string) {
    auto *stringAsChars = (char *) *string;

    length = string->length() + 1;
    characters = new char[length];
    for (uint8_t i = 0; i < 4; i++) {
        this->characters[i] = stringAsChars[i];
    }
    characters[length - 1] = '\0';
}

EthernetDeviceIdentifier::EthernetDeviceIdentifier(uint8_t deviceIndex) {
    auto ethString = String::format("eth%d", deviceIndex);
    auto identifierChars = (char *) ethString;

    length = ethString.length() + 1;
    characters = new char[length];
    for (uint8_t i = 0; i < 4; i++) {
        this->characters[i] = identifierChars[i];
    }
    characters[length - 1] = '\0';
}

EthernetDeviceIdentifier::~EthernetDeviceIdentifier() {
    delete characters;
}

String EthernetDeviceIdentifier::asString() {
    return String::format("%s", characters);
}

bool EthernetDeviceIdentifier::equals(EthernetDeviceIdentifier *other) {
    if (this->length != other->length || this->characters == nullptr || other->characters == nullptr) {
        return false;
    }
    for (uint8_t i = 0; i < length; i++) {
        if (this->characters[i] != other->characters[i]) {
            return false;
        }
    }
    return true;
}
