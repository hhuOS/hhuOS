#include <kernel/services/DebugService.h>
#include <kernel/Kernel.h>
#include <kernel/services/InputService.h>
#include <kernel/services/GraphicsService.h>
#include "devices/usb/UsbDevice.h"


#include "devices/usb/ehci/AsyncListQueue.h"


void waitForReturn() {
    Keyboard *kb = ((InputService*)Kernel::getService(InputService::SERVICE_NAME))->getKeyboard();

    while (!kb->isKeyPressed(28));
}

uint8_t UsbDevice::numDevices = 0;

IOport sysB(0x61);

UsbDevice::UsbDevice(AsyncListQueue::QueueHead *control, uint8_t portNumber) {
    UsbDevice::control = control;
    this->portNumber = portNumber;
    timeService = (TimeService*) Kernel::getService(TimeService::SERVICE_NAME);
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

    USB_TRACE("Successfully set Device Address to %d\n", address);

    return UsbDevice::Status::OK;
}

UsbDevice::Status UsbDevice::issueTransaction(AsyncListQueue::QueueHead *queue, UsbTransaction *transaction) {
    TextDriver *stream = ((GraphicsService *) Kernel::getService(GraphicsService::SERVICE_NAME))->getTextDriver();
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
            USB_TRACE("ERROR: Timeout\n");
            return UsbDevice::Status::TIMEOUT;
        }
    }

    AsyncListQueue::TransferStatus status = transaction->getStatus();

    if (status != AsyncListQueue::TransferStatus::OK) {
        stream->clear();
        USB_TRACE("ERROR: %s\n", AsyncListQueue::statusToString(status));
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

    USB_TRACE("Successfully read Device Descriptor\n");

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

    USB_TRACE("|--------------------------------------------------------------|\n");
    USB_TRACE("| USB Device Descriptor\n");
    USB_TRACE("|--------------------------------------------------------------|\n");
    USB_TRACE("| Length:                         %d\n", descriptor.length);
    USB_TRACE("| Type:                           %d\n", descriptor.type);
    USB_TRACE("| USB Spec:                       %x\n", descriptor.usbRelease);
    USB_TRACE("| Device Class:                   %x\n", descriptor.deviceClass);
    USB_TRACE("| Device Subclass:                %x\n", descriptor.deviceSubClass);
    USB_TRACE("| Device Protocol:                %x\n", descriptor.deviceProtocol);
    USB_TRACE("| Max Packet Size:                %d\n", descriptor.maxPacketSize);
    USB_TRACE("| Vendor ID:                      %x\n", descriptor.vendorId);
    USB_TRACE("| Product ID:                     %x\n", descriptor.productId);
    USB_TRACE("| Device Release:                 %x\n", descriptor.deviceRelease);
    USB_TRACE("| Manufacturer String ID:         %x\n", descriptor.manufacturerIndex);
    USB_TRACE("| Product String ID:              %x\n", descriptor.productIndex);
    USB_TRACE("| Serial Number String ID:        %x\n", descriptor.serialIndex);
    USB_TRACE("| Number of Configurations:       %d\n", descriptor.numConfigs);
    USB_TRACE("|--------------------------------------------------------------|\n");
    USB_TRACE("\n");

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
    TextDriver *stream = ((GraphicsService *) Kernel::getService(GraphicsService::SERVICE_NAME))->getTextDriver();

    DebugService *debugService = (DebugService*) Kernel::getService(DebugService::SERVICE_NAME);

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
