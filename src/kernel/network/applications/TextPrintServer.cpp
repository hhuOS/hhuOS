//
// Created by hannes on 13.06.21.
//

#include <lib/libc/printf.h>
#include "TextPrintServer.h"

TextPrintServer::TextPrintServer(uint16_t port) {
    this->socket = new Kernel::UDP4Socket(port);
    isRunning = new Atomic<bool>;
    isRunning->set(false);
    serverThread = new SimpleThread([]{
        printf("Hello world!");
    });
}

TextPrintServer::~TextPrintServer() {
    delete socket;
    delete isRunning;
    if(serverThread->hasFinished()){
        delete serverThread;
    }
}

uint8_t TextPrintServer::start() {
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

uint8_t TextPrintServer::stop() {
    if (socket == nullptr) {
        return 1;
    }
    return socket->close();
}