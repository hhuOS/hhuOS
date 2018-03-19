#ifndef __UsbDevice_include__
#define __UsbDevice_include__

#include "devices/usb/ehci/AsyncListQueue.h"
#include "devices/usb/UsbConfiguration.h"
#include "devices/usb/Usb.h"
#include "UsbTransaction.h"
#include "kernel/services/TimeService.h"

#include <stdint.h>
#include <lib/LinkedList.h>

#define DEBUG_TRANSACTION 0

/**
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
