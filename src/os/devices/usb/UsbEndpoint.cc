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

#include <kernel/log/Logger.h>
#include "UsbEndpoint.h"
#include "Usb.h"

const String UsbEndpoint::LOG_NAME = String("USB");

UsbEndpoint::UsbEndpoint(Descriptor *descriptor) {
    parse(descriptor);
}

uint8_t UsbEndpoint::getAddress() const {
    return address;
}

void UsbEndpoint::setAddress(uint8_t address) {
    UsbEndpoint::address = address;
}

uint16_t UsbEndpoint::getMaxPacketSize() const {
    return maxPacketSize;
}

void UsbEndpoint::setMaxPacketSize(uint16_t maxPacketSize) {
    UsbEndpoint::maxPacketSize = maxPacketSize;
}

uint8_t UsbEndpoint::getInterval() const {
    return interval;
}

void UsbEndpoint::setInterval(uint8_t interval) {
    UsbEndpoint::interval = interval;
}

UsbEndpoint::TransferType UsbEndpoint::getTransferType() const {
    return transferType;
}

void UsbEndpoint::setTransferType(UsbEndpoint::TransferType transferType) {
    UsbEndpoint::transferType = transferType;
}

void UsbEndpoint::parse(UsbEndpoint::Descriptor *descriptor) {

    UsbEndpoint::descriptor = *descriptor;

    address = descriptor->address & (uint8_t) 0x07;
    maxPacketSize = descriptor->maxPacketSize;
    interval = descriptor->interval;

    transferType = (TransferType) (descriptor->attributes & 0x3);
    direction = (Direction) (descriptor->address >> 7);
}

void UsbEndpoint::print() {

    Logger::trace(LOG_NAME, "|--------------------------------------------------------------|");
    Logger::trace(LOG_NAME, "| USB Endpoint Descriptor ");
    Logger::trace(LOG_NAME, "|--------------------------------------------------------------|");
    Logger::trace(LOG_NAME, "| Length:                         %d", descriptor.length);
    Logger::trace(LOG_NAME, "| Type:                           %x", descriptor.type);
    Logger::trace(LOG_NAME, "| Address:                        %x", descriptor.address);
    Logger::trace(LOG_NAME, "| Attributes:                     %x", descriptor.attributes);
    Logger::trace(LOG_NAME, "| Max Packet Size:                %x", descriptor.maxPacketSize);
    Logger::trace(LOG_NAME, "| Polling Interval:               %x", descriptor.interval);
    Logger::trace(LOG_NAME, "|--------------------------------------------------------------|");
    Logger::trace(LOG_NAME, "");

}

UsbEndpoint::Direction UsbEndpoint::getDirection() const {
    return direction;
}
