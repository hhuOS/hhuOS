#include "devices/usb/UsbTransaction.h"

UsbTransaction::UsbTransaction() {
    
}

AsyncListQueue::TransferDescriptor *UsbTransaction::getTransferDescriptor(uint32_t index) {
    return transfers.get(index);
}

uint32_t UsbTransaction::size() {
    return transfers.length();
}

void
UsbTransaction::add(AsyncListQueue::TransferDescriptor *transfer) {
    transfers.add(transfer);
}

AsyncListQueue::TransferStatus UsbTransaction::getStatus() {
    AsyncListQueue::TransferDescriptor *descriptor;

    for (uint32_t i = 0; i < transfers.length(); i++) {
        descriptor = transfers.get(i);

        if (descriptor->token & (1 << 6)) {
            return AsyncListQueue::TransferStatus::HALTED;
        }

        if (descriptor->token & (1 << 4)) {
            return AsyncListQueue::TransferStatus::BABBLE;
        }

        if (descriptor->token & (1 << 2)) {
            return AsyncListQueue::TransferStatus::MISSED_FRAME;
        }

        if (descriptor->token & (1 << 3)) {
            return AsyncListQueue::TransferStatus::TRANSACTION_ERROR;
        }

        if (descriptor->token & (1 << 5)) {
            return AsyncListQueue::TransferStatus::BUFFER_ERROR;
        }
    }

    return AsyncListQueue::TransferStatus::OK;
}

