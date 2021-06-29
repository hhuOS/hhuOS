//
// Created by hannes on 14.06.21.
//

#include <kernel/network/internet/IP4Header.h>
#include <kernel/event/network/UDP4SendEvent.h>
#include <lib/system/Result.h>
#include <lib/system/Call.h>
#include "UDP4SocketController.h"

namespace Kernel {
    //Private method!
    void UDP4SocketController::yield() {
        Standard::System::Result result{};
        Standard::System::Call::execute(Standard::System::Call::SCHEDULER_YIELD, result, 0);
    }

    UDP4SocketController::UDP4SocketController(NetworkEventBus *eventBus, size_t bufferSize) {
        this->eventBus = eventBus;
        accessLock = new Spinlock();

        isClosed = new Atomic<bool>;
        isClosed->set(true);

        if (bufferSize == 0) {
            bufferSize = 1;
        }
        inputBuffer = new Util::RingBuffer<UDP4InputEntry *>(bufferSize);
    }

    UDP4SocketController::~UDP4SocketController() {
        if (isClosed != nullptr) {
            isClosed->set(true);
        }
        delete isClosed;
        if (accessLock != nullptr) {
            accessLock->release();
        }
        delete accessLock;

        if (inputBuffer != nullptr) {
            while (!inputBuffer->isEmpty()) {
                delete inputBuffer->pop();
            }
            delete inputBuffer;
        }
    }

    uint8_t UDP4SocketController::startup() {
        if (isClosed == nullptr) {
            return 1;
        }
        isClosed->set(false);
        return 0;
    }

    uint8_t UDP4SocketController::shutdown() {
        if (isClosed == nullptr) {
            return 1;
        }
        isClosed->set(true);
        return 0;
    }

    uint8_t UDP4SocketController::notify(IP4Header *incomingIP4Header, UDP4Header *incomingUDP4Header,
                                         NetworkByteBlock *incomingInput) {
        if (isClosed == nullptr || accessLock == nullptr || inputBuffer == nullptr) {
            log.error("Internal elements not initialized, not notifying socket");
            return 1;
        }
        if (isClosed->get()) {
            log.error("Socket is closed, dropping incoming data");
            return 1;
        }
        //Check if given parameters are valid
        if (incomingIP4Header == nullptr || incomingUDP4Header == nullptr || incomingInput == nullptr) {
            log.error("Incoming IP4Header, UDP4Header or Input was null, returning");
            return 1;
        }
        accessLock->acquire();
        inputBuffer->push(new UDP4InputEntry(incomingUDP4Header, incomingIP4Header, incomingInput));
        accessLock->release();
        return 0;
    }

    uint8_t
    UDP4SocketController::receive(size_t *totalBytesRead, void *targetBuffer, size_t length, IP4Header **ip4HeaderVariable, UDP4Header **udp4HeaderVariable) {
        if (isClosed == nullptr || accessLock == nullptr || inputBuffer == nullptr) {
            log.error("Internal elements not initialized, not receiving");
            return 1;
        }
        if (isClosed->get()) {
            log.error("Socket is closed, not reading any data. Please call 'bind()' first!");
            return 1;
        }

        if (targetBuffer == nullptr || length == 0) {
            log.error("Given target buffer was null or given length was zero, not receiving");
            return 1;
        }

        while (!isClosed->get()) {
            accessLock->acquire();
            if (!inputBuffer->isEmpty()) {
                auto *entry = inputBuffer->pop();
                if (entry->copyTo(totalBytesRead, targetBuffer, length, ip4HeaderVariable, udp4HeaderVariable)) {
                    log.error("Could not copy incoming data to application buffer, delete data");
                    delete entry;
                    accessLock->release();
                    return 1;
                }
                delete entry;
                accessLock->release();
                return 0;
            }
            accessLock->release();
            //release processor, be gentle to the system :D
            yield();
        }
        // The caller MUST NOT receive anything here,
        // processing should be stopped and sending is already disabled
        //-> return error
        return 1;
    }

    uint8_t UDP4SocketController::publishSendEvent(
            IP4Address *destinationAddress, uint16_t sourcePort, uint16_t destinationPort, NetworkByteBlock *outData) {
        if (eventBus == nullptr || isClosed == nullptr) {
            log.error("Internal elements not initialized, not sending");
            return 1;
        }
        if (isClosed->get()) {
            log.error("Socket closed, not sending");
            return 1;
        }
        if (destinationAddress == nullptr || sourcePort == 0 || destinationPort == 0 || outData == nullptr) {
            log.error("Given attributes are invalid, not sending");
            return 1;
        }
        //Send data to UDP4Module via EventBus for further processing
        eventBus->publish(new UDP4SendEvent(destinationAddress, sourcePort, destinationPort, outData));
        return 0;
    }
}