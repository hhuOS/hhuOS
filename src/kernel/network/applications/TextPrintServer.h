//
// Created by hannes on 13.06.21.
//

#ifndef HHUOS_TEXTPRINTSERVER_H
#define HHUOS_TEXTPRINTSERVER_H


#include <cstdint>
#include <kernel/network/udp/sockets/UDP4Socket.h>
#include <lib/async/SimpleThread.h>

class TextPrintServer {
private:
    Kernel::UDP4Socket *socket = nullptr;
    SimpleThread *serverThread = nullptr;
    Atomic<bool> *isRunning = nullptr;

public:
    explicit TextPrintServer(uint16_t port);

    uint8_t start();

    uint8_t stop();

    virtual ~TextPrintServer();
};


#endif //HHUOS_TEXTPRINTSERVER_H
