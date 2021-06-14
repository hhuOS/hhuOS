//
// Created by hannes on 14.06.21.
//

#ifndef HHUOS_UDP4SOCKETCONTROLLER_H
#define HHUOS_UDP4SOCKETCONTROLLER_H


#include <kernel/network/NetworkByteBlock.h>
#include "UDP4Port.h"

namespace Kernel {

    class UDP4SocketController {
    private:
        UDP4Port *listeningPort = nullptr;
        NetworkByteBlock *receiveBuffer = nullptr;
        Spinlock *open = nullptr, *close = nullptr;

    public:
        UDP4SocketController(NetworkByteBlock *receiveBuffer, Spinlock *open, Spinlock *close);

        [[nodiscard]] UDP4Port *getListeningPort() const;

        uint8_t process(NetworkByteBlock *input);
    };
}

#endif //HHUOS_UDP4SOCKETCONTROLLER_H
