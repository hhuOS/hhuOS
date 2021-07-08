//
// Created by hannes on 08.07.21.
//

#include "EthernetDataPart.h"

EthernetDataPart::EthernetDataPart() = default;

EthernetDataPart::~EthernetDataPart() = default;

EthernetDataPart::EtherType EthernetDataPart::getEtherType() {
    return do_getEtherType();
}

String EthernetDataPart::asString(const String &spacing) {
    if (spacing.isEmpty()) {
        return "EMPTY_SPACING";
    }
    return do_asString(spacing);
}
