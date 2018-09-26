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

#include <kernel/services/DebugService.h>
#include <kernel/Kernel.h>
#include <kernel/services/InputService.h>
#include <kernel/services/GraphicsService.h>
#include <kernel/log/Logger.h>
#include "devices/usb/UsbDevice.h"


#include "devices/usb/ehci/AsyncListQueue.h"

Logger &UsbDevice::log = Logger::get("USB");

void waitForReturn() {
    Keyboard *kb = Kernel::getService<InputService>()->getKeyboard();

    while (!kb->isKeyPressed(28));
}

IOport sysB(0x61);

uint8_t UsbDevice::numDevices = 0;

UsbDevice::UsbDevice(AsyncListQueue::QueueHead *control, uint8_t portNumber) {
    UsbDevice::control = control;
    this->portNumber = portNumber;
    timeService = Kernel::getService<TimeService>();
    init();
}

void UsbDevice::init() {

    UsbDevice::numDevices++;

    UsbDevice::descriptor = getDeviceDescriptor();

    manufacturer = getString(descriptor.manufacturerIndex);
    product = getString(descriptor.productIndex);
    serial = getString(descriptor.serialIndex);

    UsbConfiguration::Descriptor *configurationDescriptor = getConfigurationDescriptor(0x0);

    UsbConfiguration *usbConfiguration = new UsbConfiguration(configurationDescriptor);

    addConfiguration(usbConfiguration);

    setAddress(UsbDevice::numDevices);

    setConfiguration(0x0);

    print();
}

uint8_t UsbDevice::getNumConfigurations() const {
    return numConfigurations;
}

void UsbDevice::setNumConfigurations(uint8_t numConfigurations) {
    UsbDevice::numConfigurations = numConfigurations;
}

UsbConfiguration *UsbDevice::getConfiguration(uint8_t index) const {
    return configurations.get(index);
}

void UsbDevice::addConfiguration(UsbConfiguration *configuration) {
    configurations.add(configuration);
}

uint8_t UsbDevice::getAddress() const {
    return address;
}

UsbDevice::Status UsbDevice::setAddress(uint8_t address) {
    Usb::Request request = Usb::setAddress(address);

    UsbTransaction *transaction = new UsbTransaction();

    transaction->add( AsyncListQueue::createSetupTD((uint32_t*) &request) );
    transaction->add( AsyncListQueue::createInTD(0, true, nullptr) );

    issueTransaction(control, transaction);

    control->endpointState[0] = (control->endpointState[0] & ~bitMask(7)) | (address & bitMask(7));

    UsbDevice::address = address;

    log.trace("Successfully set Device Address to %d", address);

    return UsbDevice::Status::OK;
}

UsbDevice::Status UsbDevice::issueTransaction(AsyncListQueue::QueueHead *queue, UsbTransaction *transaction) {
    TextDriver *stream = Kernel::getService<GraphicsService>()->getTextDriver();
    uint32_t size = transaction->size();

    for (uint32_t i = 0; i < size - 1; i++) {
        transaction->getTransferDescriptor(i)->nextQTD = (uint32_t) transaction->getTransferDescriptor(i + 1);
    }

    AsyncListQueue::TransferDescriptor *last = transaction->getTransferDescriptor(size - 1);

    last->nextQTD = 0x1;

    insertTransferDescriptor(queue, transaction->getTransferDescriptor(0));

#if DEBUG_TRANSACTION
    msleep(50);
    printTransaction(queue, transaction);
#endif

    uint32_t then = timeService->getSystemTime();
    while ( last->token & (1 << 7) ) {
        if ( then + UsbTransaction::TIMEOUT < timeService->getSystemTime()) {
            stream->clear();
            log.trace("ERROR: Timeout");
            return UsbDevice::Status::TIMEOUT;
        }
    }

    AsyncListQueue::TransferStatus status = transaction->getStatus();

    if (status != AsyncListQueue::TransferStatus::OK) {
        stream->clear();
        log.trace("ERROR: %s", AsyncListQueue::statusToString(status));
        return UsbDevice::Status ::ERROR;
    }

    return UsbDevice::Status::OK;
}

UsbDevice::Descriptor UsbDevice::getDeviceDescriptor() {
    uint32_t bufferAddr;
    Usb::Request request = Usb::getDeviceDescriptor();

    UsbTransaction *transaction = new UsbTransaction();

    transaction->add( AsyncListQueue::createSetupTD((uint32_t*) &request) );
    transaction->add( AsyncListQueue::createInTD(sizeof(UsbDevice::Descriptor), false, &bufferAddr) );
    transaction->add( AsyncListQueue::createOutTD(0, true, nullptr) );

    issueTransaction(control, transaction);

    UsbDevice::Descriptor ret = *(UsbDevice::Descriptor*) bufferAddr;

    delete transaction;

    log.trace("Successfully read Device Descriptor");

    return ret;
}

void
UsbDevice::insertTransferDescriptor(AsyncListQueue::QueueHead *queue, AsyncListQueue::TransferDescriptor *descriptor) {
    while ( queue->overlay.token & (1 << 7) );
    queue->overlay.nextQTD = (uint32_t) descriptor;
}

UsbConfiguration::Descriptor *UsbDevice::getConfigurationDescriptor(uint8_t id) {
    uint32_t bufferAddr;
    Usb::Request request = Usb::getConfigurationDescriptor(id);

    UsbTransaction *transaction = new UsbTransaction();

    transaction->add( AsyncListQueue::createSetupTD((uint32_t*) &request) );
    transaction->add( AsyncListQueue::createInTD(0x100, false, &bufferAddr) );
    transaction->add( AsyncListQueue::createOutTD(0, true, nullptr) );

    issueTransaction(control, transaction);

    return (UsbConfiguration::Descriptor*) bufferAddr;
}

char *UsbDevice::getString(uint8_t index) {

    if (index == 0x0) {
        return "null";
    }

    uint32_t bufferAddr;
    Usb::Request request = Usb::getStringDescriptor(index);

    UsbTransaction *transaction = new UsbTransaction();

    transaction->add( AsyncListQueue::createSetupTD((uint32_t*) &request) );
    transaction->add( AsyncListQueue::createInTD(0x100, false, &bufferAddr) );
    transaction->add( AsyncListQueue::createOutTD(0, true, nullptr) );

    issueTransaction(control, transaction);

    Usb::fixString((char*) bufferAddr + 2);

    return (char*) bufferAddr + 2;
}

UsbDevice::Status UsbDevice::setConfiguration(uint8_t configuration) {
    Usb::Request request = Usb::setConfiguration(configuration);

    UsbTransaction *transaction = new UsbTransaction();

    transaction->add( AsyncListQueue::createSetupTD((uint32_t*) &request) );
    transaction->add( AsyncListQueue::createInTD(0x0, true, nullptr) );

    issueTransaction(control, transaction);

    UsbDevice::currentConfiguration = configuration;

    return OK;
}

void UsbDevice::print() {

    log.trace("|--------------------------------------------------------------|");
    log.trace("| USB Device Descriptor");
    log.trace("|--------------------------------------------------------------|");
    log.trace("| Length:                         %d", descriptor.length);
    log.trace("| Type:                           %d", descriptor.type);
    log.trace("| USB Spec:                       %x", descriptor.usbRelease);
    log.trace("| Device Class:                   %x", descriptor.deviceClass);
    log.trace("| Device Subclass:                %x", descriptor.deviceSubClass);
    log.trace("| Device Protocol:                %x", descriptor.deviceProtocol);
    log.trace("| Max Packet Size:                %d", descriptor.maxPacketSize);
    log.trace("| Vendor ID:                      %x", descriptor.vendorId);
    log.trace("| Product ID:                     %x", descriptor.productId);
    log.trace("| Device Release:                 %x", descriptor.deviceRelease);
    log.trace("| Manufacturer String ID:         %x", descriptor.manufacturerIndex);
    log.trace("| Product String ID:              %x", descriptor.productIndex);
    log.trace("| Serial Number String ID:        %x", descriptor.serialIndex);
    log.trace("| Number of Configurations:       %d", descriptor.numConfigs);
    log.trace("|--------------------------------------------------------------|");
    log.trace("");

    for(uint8_t i = 0; i < configurations.length(); i++) {
        configurations.get(i)->print();
    }
}

UsbEndpoint *UsbDevice::findEndpoint(UsbEndpoint::TransferType type, UsbEndpoint::Direction direction) {

    UsbConfiguration *configuration;
    UsbInterface *interface;
    UsbEndpoint *endpoint;
    for (uint8_t i = 0; i < configurations.length(); i++) {
        configuration = configurations.get(i);
        for (uint8_t j = 0; j < configuration->getNumInterfaces(); j++) {
            interface = configuration->getInterface(j);
            for (uint8_t k = 0; k < interface->getNumEndpoints(); k++) {
                endpoint = interface->getEndpoint(k);

                if (endpoint->getTransferType() == type && endpoint->getDirection() == direction) {
                    return endpoint;
                }
            }
        }
    }

    return nullptr;
}

void UsbDevice::printTransaction(AsyncListQueue::QueueHead *queue, UsbTransaction *transaction) {
    TextDriver *stream = Kernel::getService<GraphicsService>()->getTextDriver();

    DebugService *debugService = Kernel::getService<DebugService>();

    stream->clear();

    debugService->dumpMemory((uint32_t) queue, 2);

    waitForReturn();

    AsyncListQueue::TransferDescriptor *current;
    for (uint8_t i = 0; i < transaction->size(); i++) {
        current = transaction->getTransferDescriptor(i);
        debugService->dumpMemory((uint32_t) current, 4);
        debugService->dumpMemory(current->buffer0 & ~0xFFF, 6);

        waitForReturn();
    }
}

uint8_t UsbDevice::getPortNumber() const {
    return portNumber;
}
