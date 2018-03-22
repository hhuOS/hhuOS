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

#ifndef __AsyncListQueue_include__
#define __AsyncListQueue_include__

#include <stdint.h>
#include "lib/Bits.h"

/**
 * @author Filip Krakowski
 */
class AsyncListQueue {

public:

    struct TransferDescriptor {

        /* 0x10 - Next Queue Element Transfer Descriptor */
        volatile uint32_t    nextQTD;

        /* 0x14 - Alternate Next Queue Element Transfer Descriptor */
        volatile uint32_t    altQTD;

        /* 0x18 - Transfer Token */
        volatile uint32_t    token;

        /* 0x1C - Buffer 0 */
        volatile uint32_t    buffer0;

        /* 0x1C - Buffer 1 */
        volatile uint32_t    buffer1;

        /* 0x1C - Buffer 2 */
        volatile uint32_t    buffer2;

        /* 0x1C - Buffer 3 */
        volatile uint32_t    buffer3;

        /* 0x1C - Buffer 4 */
        volatile uint32_t    buffer4;

        /* 0x20 - Extended Buffer 0 */
        volatile uint32_t    extBuffer0;

        /* 0x20 - Extended Buffer 1 */
        volatile uint32_t    extBuffer1;

        /* 0x20 - Extended Buffer 2 */
        volatile uint32_t    extBuffer2;

        /* 0x20 - Extended Buffer 3 */
        volatile uint32_t    extBuffer3;

        /* 0x20 - Extended Buffer 4 */
        volatile uint32_t    extBuffer4;

        ~TransferDescriptor();

    };

    struct QueueHead{

        /* 0x00 - Queue Head Horizontal Link Pointer */
        volatile uint32_t    link;

        /* 0x04 - Endpoint Capabilities/Characteristics */
        volatile uint32_t    endpointState[2];

        /* 0x0C - Current Element Transaction Descriptor Link Pointer */
        volatile uint32_t    currentQTD;

        volatile TransferDescriptor overlay;

    } ;

    enum TransferStatus {
        OK, TIMEOUT, HALTED, BUFFER_ERROR, BABBLE, TRANSACTION_ERROR, MISSED_FRAME
    };

    AsyncListQueue();

    QueueHead *getHead() const;

    void replaceWith(QueueHead *queueHead);

    void insertQueueHead(QueueHead *queueHead);

    static QueueHead *
    createQueueHead(bool head, uint8_t device, uint8_t endpoint, uint16_t packetSize, uint8_t multiplier,
                        uint8_t speed, bool dataToggleControl);

    static TransferDescriptor *createSetupTD(uint32_t *data);
    static TransferDescriptor *createInTD(uint16_t totalBytes, bool dataToggle, uint32_t *data);
    static TransferDescriptor *createOutTD(uint16_t totalBytes, bool dataToggle, uint32_t *data);

    static const uint8_t PID_OUT    = 0x00;
    static const uint8_t PID_IN     = 0x01;
    static const uint8_t PID_SETUP  = 0x02;

    static const char *statusToString(TransferStatus status);

private:

    static uint8_t *createBuffer(TransferDescriptor *descriptor);

    static TransferDescriptor *createTransferDescriptor(uint8_t pid, uint8_t next, uint16_t totalBytes, bool dataToggle, uint32_t *data);

    QueueHead *head;
    QueueHead *tail;

    void init();

    /* Queue Element Transfer Descriptor */

    static const BitField   TOKEN_STATUS;
    static const BitField   TOKEN_PID_CODE;
    static const BitField   TOKEN_ERR_COUNT;
    static const BitField   TOKEN_CUR_PAGE;
    static const BitField   TOKEN_IOC;
    static const BitField   TOKEN_TOTAL_BYTES;
    static const BitField   TOKEN_DATA_TOGGLE;
};




#endif
