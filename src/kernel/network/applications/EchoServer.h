//
// Created by hannes on 13.06.21.
//

#ifndef HHUOS_ECHOSERVER_H
#define HHUOS_ECHOSERVER_H

#define ECHO_PORT_NUMBER 7 //RFC 862

#include <cstdint>
#include <kernel/network/udp/sockets/UDP4Socket.h>

class EchoServer {
private:
    typedef struct threadAttributes{
        NetworkByteBlock *inputBuffer = nullptr;
        Kernel::UDP4Socket *socket = nullptr;
        Atomic<bool> *isRunning = nullptr;
    } attr_t;

    class EchoThread : public Kernel::KernelThread {
    private:
        attr_t attributes;
    public:
        explicit EchoThread(attr_t attributes){
            this->attributes=attributes;
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
};


#endif //HHUOS_ECHOSERVER_H
