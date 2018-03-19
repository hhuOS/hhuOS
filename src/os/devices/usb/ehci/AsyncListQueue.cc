#include "devices/usb/Usb.h"
#include "AsyncListQueue.h"
#include "AsyncListQueue.h"

extern "C" {
    #include "lib/libc/stdlib.h"
    #include "lib/libc/string.h"
}

AsyncListQueue::AsyncListQueue() {
    init();
}

AsyncListQueue::QueueHead *AsyncListQueue::getHead() const {
    return head;
}

void AsyncListQueue::init() {
    head = tail = createQueueHead(true, 0x00, 0x00, 0x0000, 0, 0, false);
}

AsyncListQueue::QueueHead *
AsyncListQueue::createQueueHead(bool head, uint8_t device, uint8_t endpoint, uint16_t packetSize, uint8_t multiplier,
                                uint8_t speed, bool dataToggleControl) {
    QueueHead *queueHead = (QueueHead*) aligned_alloc(4096, sizeof(QueueHead));

    queueHead->link                     = (uint32_t) queueHead | 0x2;

    queueHead->endpointState[0]         |= (device      & bitMask(7))   << 0;
    queueHead->endpointState[0]         |= (endpoint    & bitMask(4))   << 8;
    queueHead->endpointState[0]         |= (speed       & bitMask(2))   << 12;
    queueHead->endpointState[0]         |= (dataToggleControl)          << 14;
    queueHead->endpointState[0]         |= (head        & bitMask(1))   << 15;
    queueHead->endpointState[0]         |= (packetSize  & bitMask(11))  << 16;

    queueHead->endpointState[1]         |= (multiplier  & bitMask(2))   << 30;

    queueHead->overlay.nextQTD          = 0x1;
    queueHead->overlay.altQTD           = 0x1;

    return queueHead;
}

void AsyncListQueue::insertQueueHead(AsyncListQueue::QueueHead *queueHead) {
    queueHead->link = (uint32_t) head | 0x2;
    tail->link = (uint32_t) queueHead | 0x2;
}

uint8_t *AsyncListQueue::createBuffer(AsyncListQueue::TransferDescriptor *descriptor) {

    descriptor->buffer0 = (uint32_t) aligned_alloc(4096, 4096);
    descriptor->buffer1 = (uint32_t) aligned_alloc(4096, 4096);
    descriptor->buffer2 = (uint32_t) aligned_alloc(4096, 4096);
    descriptor->buffer3 = (uint32_t) aligned_alloc(4096, 4096);
    descriptor->buffer4 = (uint32_t) aligned_alloc(4096, 4096);

    descriptor->extBuffer0 = 0x0;
    descriptor->extBuffer1 = 0x0;
    descriptor->extBuffer2 = 0x0;
    descriptor->extBuffer3 = 0x0;
    descriptor->extBuffer4 = 0x0;

    return (uint8_t*) descriptor->buffer0;
}

AsyncListQueue::TransferDescriptor *
AsyncListQueue::createTransferDescriptor(uint8_t pid, uint8_t next, uint16_t totalBytes, bool dataToggle, uint32_t *data) {

    AsyncListQueue::TransferDescriptor *td =
            (AsyncListQueue::TransferDescriptor*) aligned_alloc(32, sizeof(AsyncListQueue::TransferDescriptor));

    td->nextQTD                     = next;
    td->altQTD                      = 0x1;

    td->token                       |= (pid & bitMask(2)) << 8;
    td->token                       |= (totalBytes & bitMask(15)) << 16;
    td->token                       |= (dataToggle & bitMask(1)) << 31;
    td->token                       |= (0x1) << 7;

    uint8_t *buffer = createBuffer(td);

    if (totalBytes == 0) {
        return td;
    }

    if (pid == PID_SETUP || pid == PID_OUT) {
        memcpy(buffer, data, totalBytes);
    } else if (pid == PID_IN) {
        *data = (uint32_t) buffer;
    }

    return td;
}

AsyncListQueue::TransferDescriptor *AsyncListQueue::createSetupTD(uint32_t *data) {
    return createTransferDescriptor(PID_SETUP, 0x1, 0x8, false, data);
}

AsyncListQueue::TransferDescriptor *AsyncListQueue::createInTD(uint16_t totalBytes, bool dataToggle, uint32_t *data) {
    return createTransferDescriptor(PID_IN, 0x1, totalBytes, dataToggle, data);
}

AsyncListQueue::TransferDescriptor *AsyncListQueue::createOutTD(uint16_t totalBytes, bool dataToggle, uint32_t *data) {
    return createTransferDescriptor(PID_OUT, 0x1, totalBytes, dataToggle, data);
}

const char *AsyncListQueue::statusToString(AsyncListQueue::TransferStatus status) {
    switch (status) {
        case OK:
            return "Ok";
        case TRANSACTION_ERROR:
            return "Transaction Error";
        case MISSED_FRAME:
            return "Missed Frame";
        case HALTED:
            return "Halted";
        case TIMEOUT:
            return "Timeout";
        case BUFFER_ERROR:
            return "Buffer Error";
        case BABBLE:
            return "Babble";
    }

    return "Unknown";
}

void AsyncListQueue::replaceWith(AsyncListQueue::QueueHead *queueHead) {
    head = tail = queueHead;
}

AsyncListQueue::TransferDescriptor::~TransferDescriptor() {
    delete (uint32_t*) (buffer0 & 0xFFFFF000);
    delete (uint32_t*) (buffer1 & 0xFFFFF000);
    delete (uint32_t*) (buffer2 & 0xFFFFF000);
    delete (uint32_t*) (buffer3 & 0xFFFFF000);
    delete (uint32_t*) (buffer4 & 0xFFFFF000);
}
