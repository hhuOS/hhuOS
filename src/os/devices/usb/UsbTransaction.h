/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef __UsbTransaction_include__
#define __UsbTransaction_include__

#include <lib/util/LinkedList.h>
#include "devices/usb/ehci/AsyncListQueue.h"

/**
 * Represents a generic USB transaction.
 *
 * @author Filip Krakowski
 */
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