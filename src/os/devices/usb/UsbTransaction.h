#ifndef __UsbTransaction_include__
#define __UsbTransaction_include__

#include <lib/LinkedList.h>
#include "devices/usb/ehci/AsyncListQueue.h"

class UsbTransaction {

public:

    UsbTransaction();

    enum Type {
        UHCI, EHCI
    };

    AsyncListQueue::TransferDescriptor *getTransferDescriptor(uint32_t index);

    void add(AsyncListQueue::TransferDescriptor *transfer);

    uint32_t size();

    AsyncListQueue::TransferStatus getStatus();

    static const uint32_t   TIMEOUT = 500;

private:

    LinkedList<AsyncListQueue::TransferDescriptor> transfers;

    uint32_t* data;
};

#endif