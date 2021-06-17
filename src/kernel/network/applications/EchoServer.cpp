
// Created by hannes on 13.06.21.


#include <lib/libc/printf.h>
#include <kernel/network/NetworkDefinitions.h>
#include "EchoServer.h"

EchoServer::EchoServer(size_t inputBufferSize) {
    attributes.log = &log;
    attributes.socket = new Kernel::UDP4Socket(ECHO_PORT_NUMBER);
    attributes.inputBufferSize = inputBufferSize;
    attributes.inputBuffer = new uint8_t(inputBufferSize);
    attributes.isRunning = new Atomic<bool>;
    attributes.isRunning->set(false);

    serverThread = new EchoThread(attributes);
}

EchoServer::~EchoServer() {
    cleanup();
}

void EchoServer::cleanup() const {
    delete attributes.inputBuffer;
    delete attributes.socket;
    delete attributes.isRunning;
    delete serverThread;
}

uint8_t EchoServer::start() {
    if (
            attributes.inputBuffer == nullptr ||
            attributes.socket == nullptr ||
            attributes.isRunning == nullptr ||
            attributes.log == nullptr ||
            serverThread == nullptr
            ) {
        cleanup();
        return 1;
    }
    if (attributes.socket->bind()) {
        log.error("Binding socket in EchoServer failed");
        cleanup();
        return 1;
    }
    attributes.isRunning->set(true);
    serverThread->start();
    return 0;
}

uint8_t EchoServer::stop() {
    if (
            attributes.socket == nullptr ||
            attributes.isRunning == nullptr ||
            serverThread == nullptr
            ) {
        return 1;
    }
    attributes.isRunning->set(false);
    while (!serverThread->hasFinished()) {}
    return attributes.socket->close();
}

void EchoServer::EchoThread::run() {
    int bytesReceived;
    IP4Header *ip4Header = nullptr;
    UDP4Header *udp4Header = nullptr;

    while (attributes.isRunning->get()) {
        bytesReceived = attributes.socket->receive(
                attributes.inputBuffer,
                attributes.inputBufferSize,
                &ip4Header, &udp4Header
        );
        if (bytesReceived <= 0) {
            (*attributes.log).error("Error while receiving data, stopping");
            delete ip4Header;
            delete udp4Header;
            return;
        }
        (*attributes.log).info(
                "Incoming datagram from %s with content '%s', sending response",
                ip4Header->getSourceAddress()->asChars(),
                attributes.inputBuffer
        );

        if (attributes.socket->send(
                ip4Header->getSourceAddress(),
                udp4Header->getSourcePort(),
                attributes.inputBuffer,
                static_cast<size_t>(bytesReceived)
        )
                ) {
            (*attributes.log).error("Sending response failed, stopping");
            delete ip4Header;
            delete udp4Header;
            return;
        }
        delete ip4Header;
        delete udp4Header;
    }
}

