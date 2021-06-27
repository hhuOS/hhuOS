//
// Created by hannes on 14.06.21.
//

#include <kernel/network/internet/IP4Header.h>
#include <kernel/event/network/UDP4SendEvent.h>
#include "UDP4SocketController.h"

namespace Kernel {
    UDP4SocketController::UDP4SocketController(NetworkEventBus *eventBus) {
        this->eventBus = eventBus;
        readLock = new Spinlock();
        readLock->acquire();

        writeLock = new Spinlock();
        writeLock->release();

        isClosed = new Atomic<bool>;
        isClosed->set(true);

        inputBuffer= new Util::RingBuffer<UDP4InputEntry*>(8);
    }

    UDP4SocketController::~UDP4SocketController() {
        if (isClosed != nullptr) {
            isClosed->set(true);
        }
        delete isClosed;
        isClosed = nullptr;

        if (writeLock != nullptr) {
            writeLock->release();
            delete writeLock;
            writeLock = nullptr;
        }

        if (readLock != nullptr) {
            readLock->release();
            delete readLock;
            readLock = nullptr;
        }
        //TODO: Add input buffer deletion
    }

    uint8_t UDP4SocketController::startup() {
        if (writeLock == nullptr || isClosed == nullptr) {
            return 1;
        }
        writeLock->acquire();
        isClosed->set(false);
        writeLock->release();
        return 0;
    }

    uint8_t UDP4SocketController::shutdown() {
        if (writeLock == nullptr || isClosed == nullptr || readLock == nullptr) {
            return 1;
        }
        writeLock->acquire();
        isClosed->set(true);

        //When isClosed==true,
        // readers AND writers simply open their own locks and return 1
        //-> no critical access to central data anymore, we can open both locks now
        writeLock->release();
        readLock->release();
        return 0;
    }

    uint8_t UDP4SocketController::notify(IP4Header *incomingIP4Header, UDP4Header *incomingUDP4Header,
                                         NetworkByteBlock *input) {
        //Directly return if one of the main elements is not initialized
        if (readLock == nullptr || writeLock == nullptr || isClosed == nullptr) {
            //Return error, this will tell UDP4Module to drop incoming data
            return 1;
        }
        //writeLock is opened after receive()
        //-> we will wait here until a reader is finished!
        writeLock->acquire();
        //Check if we are not started yet or already closing
        if (isClosed->get()) {
            log.error("Socket is closed, dropping incoming data");
            //Let all writers come to this point here and return 1
            //-> release writeLock again instead of readLock!
            writeLock->release();
            //Return error, this will tell UDP4Module to drop incoming data
            return 1;
        }
        //Check if given parameters are valid
        if (incomingIP4Header == nullptr || incomingUDP4Header == nullptr || input == nullptr) {
            log.error("Incoming IP4Header, UDP4Header or Input was null, returning");
            //We are not closed here, but parameters are invalid
            //-> next step will be a read as usual
            //-> open readLock to start next round
            readLock->release();
            //Return error, this will tell UDP4Module to drop incoming data
            return 1;
        }
        inputBuffer->push(new UDP4InputEntry(incomingUDP4Header,incomingIP4Header,input));
        readLock->release();
        return 0;
    }

    uint8_t
    UDP4SocketController::receive(size_t *totalBytesRead, void *targetBuffer, size_t length,
                                  IP4Header **ip4HeaderVariable,
                                  UDP4Header **udp4HeaderVariable) {

        if (readLock == nullptr || writeLock == nullptr || isClosed == nullptr) {
            return 1;
        }
        readLock->acquire();
        if (isClosed->get()) {
            log.error("Socket is closed, not reading any data");
            //Let all readers come to this point here and return 1
            //-> release readLock again instead of writeLock!
            readLock->release();
            if (totalBytesRead != nullptr) {
                *totalBytesRead = 0;
            }
            return 1;
        }
        inputBuffer->pop()->copyTo(totalBytesRead, targetBuffer, length, ip4HeaderVariable, udp4HeaderVariable);
        //Start next round by allowing next writer
        writeLock->release();
        return 0;
    }

    uint8_t UDP4SocketController::publishSendEvent(
            IP4Address *destinationAddress, uint16_t sourcePort, uint16_t destinationPort, NetworkByteBlock *outData) {
        if (destinationAddress == nullptr || outData == nullptr) {
            return 1;
        }
        //Send data to UDP4Module via EventBus for further processing
        eventBus->publish(new UDP4SendEvent(destinationAddress, sourcePort, destinationPort, outData));
        return 0;
    }
}