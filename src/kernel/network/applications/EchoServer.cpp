//
// Created by hannes on 13.06.21.
//

#include <lib/libc/printf.h>
#include "EchoServer.h"

EchoServer::EchoServer(uint16_t port) {
    this->socket = new Kernel::UDP4Socket(port);
    isRunning = new Atomic<bool>;
    isRunning->set(false);
    serverThread =
}

EchoServer::~EchoServer() {
    delete socket;
    delete isRunning;
    if(serverThread->hasFinished()){
        delete serverThread;
    }
}

uint8_t EchoServer::start() {
    if (
            serverThread == nullptr ||
            isRunning== nullptr ||
            socket == nullptr ||
            socket->bind()
        ) {
        return 1;
    }
    isRunning->set(true);
    serverThread->start();
    return 0;
}

uint8_t EchoServer::stop() {
    if (socket == nullptr) {
        return 1;
    }
    return socket->close();
}