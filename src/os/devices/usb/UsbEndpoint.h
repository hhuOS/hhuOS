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

#ifndef __UsbEndpoint_include__
#define __UsbEndpoint_include__

#include <cstdint>

/**
 * Represents a USB device endpoint.
 *
 * @author Filip Krakowski
 */
class UsbEndpoint {

public:

    typedef struct {
        uint8_t     length;                 // Length
        uint8_t     type;                   // Type
        uint8_t     address;                // Endpoint Address
        uint8_t     attributes;             // Attributes
        uint16_t    maxPacketSize;          // Max Packet Size
        uint8_t     interval;               // Polling Interval
    } Descriptor;

    /* Subclass Codes */
    const static uint8_t SUBCLASS_SCSI = 0x06;

    /* Protocol Codes */
    const static uint8_t PROTOCOL_BBB = 0x50;

    enum Direction {
        OUT, IN
    };

    enum TransferType {
        CONTROL, ISOCHRONOUS, BULK, INTERRUPT
    };

    UsbEndpoint(Descriptor *descriptor);

    uint8_t getAddress() const;

    void setAddress(uint8_t address);

    uint16_t getMaxPacketSize() const;

    void setMaxPacketSize(uint16_t maxPacketSize);

    uint8_t getInterval() const;

    void setInterval(uint8_t interval);

    TransferType getTransferType() const;

    void setTransferType(TransferType transferType);

    Direction getDirection() const;

    void print();

private:

    static const String LOG_NAME;

    UsbEndpoint::Descriptor descriptor;

    uint8_t address;
    uint8_t interval;
    uint16_t maxPacketSize;

    TransferType transferType;
    Direction direction;

    void parse(Descriptor *descriptor);
};

#endif