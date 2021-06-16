//
// Created by hannes on 13.06.21.
//

#include <lib/libc/printf.h>
#include "EchoServer.h"

EchoServer::EchoServer(size_t inputBufferSize) {
    attributes.socket =
        new Kernel::UDP4Socket(
                new UDP4Port(ECHO_PORT_NUMBER)
                );
    attributes.inputBuffer = new NetworkByteBlock(inputBufferSize);
    attributes.isRunning = new Atomic<bool>;
    attributes.isRunning->set(false);

    serverThread = new EchoThread(attributes);
}

EchoServer::~EchoServer() {
    delete attributes.inputBuffer;
    delete attributes.socket;
    delete attributes.isRunning;
    delete serverThread;
}

uint8_t EchoServer::start() {
    if (
            attributes.socket == nullptr ||
            attributes.isRunning== nullptr ||
            serverThread == nullptr ||
            attributes.inputBuffer->getLength() == 0 ||
            attributes.socket->bind()
        ) {
        return 1;
    }
    attributes.isRunning->set(true);
    serverThread->start();
    return 0;
}

uint8_t EchoServer::stop() {
    if (
            attributes.socket == nullptr ||
            attributes.isRunning== nullptr ||
            serverThread == nullptr
        ) {
        return 1;
    }
    attributes.isRunning->set(false);
    while(!serverThread->hasFinished()){}
    return attributes.socket->close();
}

void EchoServer::EchoThread::run() {
    size_t bytesReceived = 0;
    uint16_t listeningPort = 0;

    attributes.socket->copyListeningPortTo(&listeningPort);

    while(attributes.isRunning->get()) {
//        bytesReceived = attributes.inputBuffer->getLength();
//        attributes.inputBuffer->resetIndex();
//
//        attributes.socket->receive(
//                attributes.inputBuffer,
//                attributes.inputBuffer->getLength()
//                );
//        attributes.log.info("Incoming datagram, sending response");
//
//        bytesReceived-=attributes.inputBuffer->bytesRemaining();
//        attributes.socket->send(attributes.inputBuffer, bytesReceived);
    }
}