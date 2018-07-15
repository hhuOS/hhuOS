/*
 * Copyright (C) 2018  Filip Krakowski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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

