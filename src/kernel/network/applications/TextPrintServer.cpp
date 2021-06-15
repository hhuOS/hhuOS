//
// Created by hannes on 13.06.21.
//

#include "TextPrintServer.h"

TextPrintServer::TextPrintServer(uint16_t port) {
    this->socket = new Kernel::UDP4Socket(port);
}

uint8_t TextPrintServer::start() {
    if (
            socket == nullptr ||
            socket->bind()
        ) {
        return 1;
    }
}

uint8_t TextPrintServer::stop() {
    if (socket == nullptr) {
        return 1;
    }
    return socket->close();
}
