//
// Created by hannes on 14.06.21.
//

#include "UDP4SocketController.h"

namespace Kernel {
    UDP4SocketController::UDP4SocketController(NetworkByteBlock *receiveBuffer, Spinlock *open, Spinlock *close)
            : receiveBuffer(receiveBuffer),
              open(open),
              close(close) {}

    UDP4Port *UDP4SocketController::getListeningPort() const {
        return listeningPort;
    }

    uint8_t UDP4SocketController::process(NetworkByteBlock *input) {
        if (
                receiveBuffer == nullptr ||
                input == nullptr
                ) {
            return 1;
        }
        receiveBuffer->append(input, input->getLength());
        open->release();
        return 0;
    }
}