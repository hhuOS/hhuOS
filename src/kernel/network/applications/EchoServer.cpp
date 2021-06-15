//
// Created by hannes on 13.06.21.
//

#include <lib/libc/printf.h>
#include "EchoServer.h"

EchoServer::EchoServer() {
    socket =
        new Kernel::UDP4Socket(
                new UDP4Port(ECHO_PORT_NUMBER)
                );
    isRunning = new Atomic<bool>;
    isRunning->set(false);

    attributes.socket=this->socket;
    attributes.isRunning=this->isRunning;
    serverThread = new EchoThread(attributes);
}

EchoServer::~EchoServer() {
    delete socket;
    delete isRunning;
    delete serverThread;
}

uint8_t EchoServer::start() {
    if (
            socket == nullptr ||
            isRunning== nullptr ||
            serverThread == nullptr ||
            socket->bind()
        ) {
        return 1;
    }
    isRunning->set(true);
    serverThread->start();
    return 0;
}

uint8_t EchoServer::stop() {
    if (
            socket == nullptr ||
            isRunning== nullptr ||
            serverThread == nullptr
        ) {
        return 1;
    }
    isRunning->set(false);
    while(!serverThread->hasFinished()){}
    return socket->close();
}

void EchoServer::EchoThread::run() {
    uint16_t listeningPort = 0;
    attributes.socket->copyListeningPortTo(&listeningPort);

//    while(attributes.isRunning->get()) {
        printf("Hello world! Listening on port %d\n", listeningPort);
//    }
}