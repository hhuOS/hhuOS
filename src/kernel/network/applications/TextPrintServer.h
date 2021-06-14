//
// Created by hannes on 13.06.21.
//

#ifndef HHUOS_TEXTPRINTSERVER_H
#define HHUOS_TEXTPRINTSERVER_H


#include <cstdint>
#include <kernel/network/udp/sockets/UDP4Socket.h>

class TextPrintServer {
private:
    uint16_t port = 0;
    Kernel::UDP4Socket *socket = nullptr;

public:
    explicit TextPrintServer(uint16_t port);

    uint8_t start();

    uint8_t stop();
};


#endif //HHUOS_TEXTPRINTSERVER_H
