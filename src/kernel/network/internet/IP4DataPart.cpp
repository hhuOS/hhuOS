//
// Created by hannes on 08.07.21.
//

#include "IP4DataPart.h"

IP4DataPart::IP4DataPart() = default;

IP4DataPart::~IP4DataPart() = default;

IP4DataPart::IP4ProtocolType IP4DataPart::getIP4ProtocolType() {
    return do_getIP4ProtocolType();
}
