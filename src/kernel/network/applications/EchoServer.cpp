
// Created by hannes on 13.06.21.


#include <kernel/network/NetworkDefinitions.h>
#include "EchoServer.h"

EchoServer::EchoServer(size_t inputBufferSize) {
    attributes.log = &log;
    attributes.socket = new Kernel::UDP4ServerSocket(ECHO_PORT_NUMBER);
    attributes.inputBufferSize = inputBufferSize;
    attributes.inputBuffer = new uint8_t[inputBufferSize + 1];
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
    if (attributes.inputBuffer == nullptr || attributes.socket == nullptr || attributes.isRunning == nullptr ||
        attributes.log == nullptr || serverThread == nullptr || attributes.inputBufferSize == 0) {
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
    if (attributes.socket == nullptr || attributes.isRunning == nullptr || serverThread == nullptr) {
        return 1;
    }
    attributes.isRunning->set(false);
    attributes.socket->close();
    serverThread->join();
    return 0;
}

void EchoServer::EchoThread::run() {
    uint8_t addressBytes[IP4ADDRESS_LENGTH];
    size_t bytesReceived = 0;
    IP4Header *ip4Header = nullptr;
    UDP4Header *udp4Header = nullptr;

    while (attributes.isRunning->get()) {
        if (attributes.socket->
                receive(&bytesReceived, attributes.inputBuffer, attributes.inputBufferSize, &ip4Header, &udp4Header)
            || bytesReceived == 0) {
            if (attributes.isRunning->get()) {
                (*attributes.log).error("Error while receiving data, stopping");
            } else {
                (*attributes.log).info("Socket is shutting down, not receiving anything");
            }
            delete ip4Header;
            delete udp4Header;
            return;
        }
        //Set end character for printout
        attributes.inputBuffer[bytesReceived] = 0;
        ip4Header->getSourceAddress()->copyTo(addressBytes);
        auto *senderAddress = new IP4Address(addressBytes);
        uint16_t sourcePort = udp4Header->getSourcePort();

        delete udp4Header;
        udp4Header = nullptr;

        delete ip4Header;
        ip4Header = nullptr;

        (*attributes.log)
                .info("Incoming datagram from %s with content '%s', sending response",
                      (char *) senderAddress->asString(), attributes.inputBuffer);

        if (attributes.socket->send(senderAddress, sourcePort, attributes.inputBuffer, bytesReceived)) {
            (*attributes.log).error("Sending response failed, stopping");
            return;
        }
    }
}

