#include "UsbEndpoint.h"
#include "Usb.h"

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

    USB_TRACE("|--------------------------------------------------------------|\n");
    USB_TRACE("| USB Endpoint Descriptor \n");
    USB_TRACE("|--------------------------------------------------------------|\n");
    USB_TRACE("| Length:                         %d\n", descriptor.length);
    USB_TRACE("| Type:                           %x\n", descriptor.type);
    USB_TRACE("| Address:                        %x\n", descriptor.address);
    USB_TRACE("| Attributes:                     %x\n", descriptor.attributes);
    USB_TRACE("| Max Packet Size:                %x\n", descriptor.maxPacketSize);
    USB_TRACE("| Polling Interval:               %x\n", descriptor.interval);
    USB_TRACE("|--------------------------------------------------------------|\n");
    USB_TRACE("\n");

}

UsbEndpoint::Direction UsbEndpoint::getDirection() const {
    return direction;
}
