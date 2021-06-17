//
// Created by hannes on 13.06.21.
//

#ifndef HHUOS_ECHOSERVER_H
#define HHUOS_ECHOSERVER_H

#include <cstdint>
#include <kernel/network/udp/sockets/UDP4ServerSocket.h>

class EchoServer {
private:
    Kernel::Logger &log = Kernel::Logger::get("EchoServer");

    typedef struct threadAttributes {
        Kernel::UDP4ServerSocket *socket = nullptr;
        Atomic<bool> *isRunning = nullptr;
        Kernel::Logger *log = nullptr;
        uint8_t *inputBuffer = nullptr;
        size_t inputBufferSize = 0;
    } attr_t;

    class EchoThread : public Kernel::KernelThread {
    private:
        attr_t attributes;
    public:
        explicit EchoThread(attr_t attributes) {
            this->attributes = attributes;
        }

        void run() override;
    };

    attr_t attributes;
    EchoThread *serverThread = nullptr;
public:
    explicit EchoServer(size_t inputBufferSize);

    uint8_t start();

    uint8_t stop();

    virtual ~EchoServer();

    void cleanup() const;
};


#endif //HHUOS_ECHOSERVER_H
