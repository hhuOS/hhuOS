//
// Created by hannes on 13.06.21.
//

#ifndef HHUOS_ECHOSERVER_H
#define HHUOS_ECHOSERVER_H


#include <cstdint>
#include <kernel/network/udp/sockets/UDP4Socket.h>

class EchoServer {
private:
    Kernel::UDP4Socket *socket = nullptr;
    SimpleThread *serverThread = nullptr;
    Atomic<bool> *isRunning = nullptr;

    typedef struct threadAttributes{
        Kernel::UDP4Socket *socket = nullptr;
        Atomic<bool> *isRunning = nullptr;
    } attr_t;

    attr_t attributes;
public:
    explicit EchoServer(uint16_t port);

    uint8_t start();

    uint8_t stop();

    virtual ~EchoServer();
};


#endif //HHUOS_ECHOSERVER_H
