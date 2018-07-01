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

#ifndef __UsbDevice_include__
#define __UsbDevice_include__

#include "devices/usb/ehci/AsyncListQueue.h"
#include "devices/usb/UsbConfiguration.h"
#include "devices/usb/Usb.h"
#include "UsbTransaction.h"
#include "kernel/services/TimeService.h"

#include <cstdint>
#include <lib/util/LinkedList.h>

#define DEBUG_TRANSACTION 0

/**
 * Represents a USB device.
 *
 * @author Filip Krakowski
 */
class UsbDevice {

public:

    UsbDevice(AsyncListQueue::QueueHead *control, uint8_t portNumber);

    typedef struct tagDEVICE_DESCRIPTOR {
        uint8_t     length;
        uint8_t     type;
        uint16_t    usbRelease;
        uint8_t     deviceClass;
        uint8_t     deviceSubClass;
        uint8_t     deviceProtocol;
        uint8_t     maxPacketSize;
        uint16_t    vendorId;
        uint16_t    productId;
        uint16_t    deviceRelease;
        uint8_t     manufacturerIndex;
        uint8_t     productIndex;
        uint8_t     serialIndex;
        uint8_t     numConfigs;
    } Descriptor;

    enum Status {
        OK, TIMEOUT, ERROR
    };

    uint8_t getNumConfigurations() const;

    void setNumConfigurations(uint8_t numConfigurations);

    UsbConfiguration *getConfiguration(uint8_t index) const;

    void addConfiguration(UsbConfiguration *configurations);

    uint8_t getAddress() const;

    uint8_t getPortNumber() const;

protected:

    UsbDevice::Status issueTransaction(AsyncListQueue::QueueHead *queue, UsbTransaction *transaction);

    AsyncListQueue::QueueHead *control;

    LinkedList<UsbConfiguration> configurations;

    UsbEndpoint *findEndpoint(UsbEndpoint::TransferType, UsbEndpoint::Direction);

    char *manufacturer;
    char *product;
    char *serial;

    static const String LOG_NAME;

private:

    void init();

    UsbDevice::Descriptor descriptor;

    uint8_t address;
    uint8_t numConfigurations;
    uint8_t currentConfiguration;
    uint8_t portNumber;

    UsbDevice::Status setAddress(uint8_t address);

    UsbDevice::Status setConfiguration(uint8_t configuration);

    UsbDevice::Descriptor getDeviceDescriptor();

    UsbConfiguration::Descriptor *getConfigurationDescriptor(uint8_t id);

    TimeService *timeService;

    char *getString(uint8_t index);

    void insertTransferDescriptor(AsyncListQueue::QueueHead *queue, AsyncListQueue::TransferDescriptor *descriptor);

    static uint8_t numDevices;

    void print();

    void printTransaction(AsyncListQueue::QueueHead *queueHead, UsbTransaction *transaction);
};

#endif
