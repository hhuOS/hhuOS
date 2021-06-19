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
        //make sure that deleteData() is only called once
        //-> don't call it in notifySocket() nor in receive()!
        deleteData();

        //When isClosed==true,
        // readers AND writers simply open their own locks and return 1
        //-> no critical access to central data anymore, we can open both locks now
        writeLock->release();
        readLock->release();
        return 0;
    }

    uint8_t UDP4SocketController::notifySocket(IP4Header *incomingIP4Header, UDP4Header *incomingUDP4Header,
                                               NetworkByteBlock *input) {
        //Directly return if one of the main elments is not initialized
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
        this->ip4Header = incomingIP4Header;
        this->udp4Header = incomingUDP4Header;
        this->content = input;
        readLock->release();
        return 0;
    }

    uint8_t
    UDP4SocketController::receive(size_t *totalBytesRead, void *targetBuffer, size_t length,
                                  IP4Header **ip4HeaderVariable,
                                  UDP4Header **udp4HeaderVariable) {

        if (readLock == nullptr || writeLock == nullptr || isClosed == nullptr ||
            targetBuffer == nullptr || length == 0
                ) {
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
        if (ip4Header == nullptr || udp4Header == nullptr || content == nullptr) {
            log.error("IP4Header, UDP4Header or Content was null, return");
            //We are not closed here, but previous writing has failed anyway
            //-> next step will be a read as usual
            //-> open writeLock or we will freeze here when doing shutdown!
            //  --> shutdown waits for writeLock...
            writeLock->release();
            if (totalBytesRead != nullptr) {
                *totalBytesRead = 0;
            }
            return 1;
        }
        if (totalBytesRead != nullptr) {
            //count bytes read if requested
            *totalBytesRead = content->bytesRemaining();
        }
        if (length > content->bytesRemaining()) {
            length = content->bytesRemaining();
        }

        //Cleanup if reading fails
        if (content->read(targetBuffer, length)) {
            deleteData();
            writeLock->release();
            return 1;
        }
        if (totalBytesRead != nullptr) {
            *totalBytesRead = *totalBytesRead - content->bytesRemaining();
        }
        if (ip4HeaderVariable == nullptr) {
            //delete IP4Header if not requested
            delete this->ip4Header;
        } else {
            *ip4HeaderVariable = this->ip4Header;
        }
        if (udp4HeaderVariable == nullptr) {
            //delete UDP4Header if not requested
            delete this->udp4Header;
        } else {
            *udp4HeaderVariable = this->udp4Header;
        }

        delete content;
        content = nullptr;

        //Headers are in use somewhere else
        //-> no delete here! Just set them to nullptr to avoid using them again
        this->ip4Header = nullptr;
        this->udp4Header = nullptr;
        //Start next round by allowing next writer
        writeLock->release();
        return 0;
    }

    uint8_t UDP4SocketController::publishSendEvent(IP4Address *destinationAddress, UDP4Datagram *outDatagram) {
        if (destinationAddress == nullptr || outDatagram == nullptr) {
            return 1;
        }
        eventBus->publish(
                new UDP4SendEvent(destinationAddress, outDatagram)
        );
        return 0;
    }

    //Private method!
    void UDP4SocketController::deleteData() {
        delete this->ip4Header;
        delete this->udp4Header;
        delete this->content;
        this->ip4Header = nullptr;
        this->udp4Header = nullptr;
        this->content = nullptr;
    }
}