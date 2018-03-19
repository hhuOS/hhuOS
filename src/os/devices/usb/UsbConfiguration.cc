#include "UsbConfiguration.h"


UsbConfiguration::UsbConfiguration(UsbConfiguration::Descriptor *descriptor) {
    parse(descriptor);
}

uint8_t UsbConfiguration::getId() const {
    return id;
}

void UsbConfiguration::setId(uint8_t id) {
    UsbConfiguration::id = id;
}

uint8_t UsbConfiguration::getMaxPower() const {
    return maxPower;
}

void UsbConfiguration::setMaxPower(uint8_t maxPower) {
    UsbConfiguration::maxPower = maxPower;
}

char *UsbConfiguration::getName() const {
    return name;
}

void UsbConfiguration::setName(char *name) {
    UsbConfiguration::name = name;
}

bool UsbConfiguration::isSupportsRemoteWakeup() const {
    return supportsRemoteWakeup;
}

void UsbConfiguration::setSupportsRemoteWakeup(bool supportsRemoteWakeup) {
    UsbConfiguration::supportsRemoteWakeup = supportsRemoteWakeup;
}

bool UsbConfiguration::isIsSelfPowered() const {
    return isSelfPowered;
}

void UsbConfiguration::setIsSelfPowered(bool isSelfPowered) {
    UsbConfiguration::isSelfPowered = isSelfPowered;
}

uint8_t UsbConfiguration::getNumInterfaces() const {
    return numInterfaces;
}

void UsbConfiguration::setNumInterfaces(uint8_t numInterfaces) {
    UsbConfiguration::numInterfaces = numInterfaces;
}

void UsbConfiguration::parse(UsbConfiguration::Descriptor *descriptor) {

    UsbConfiguration::descriptor = *descriptor;

    id = descriptor->configValue;
    numInterfaces = descriptor->numInterfaces;
    maxPower = descriptor->maxPower;

    UsbInterface *interface;
    UsbInterface::Descriptor *interfaceDescriptor =
            (UsbInterface::Descriptor*) ((uint32_t) descriptor + descriptor->length);

    interface = new UsbInterface(interfaceDescriptor);

    interfaces.add(interface);
}

void UsbConfiguration::print() {

    USB_TRACE("|--------------------------------------------------------------|\n");
    USB_TRACE("| USB Configuration Descriptor\n");
    USB_TRACE("|--------------------------------------------------------------|\n");
    USB_TRACE("| Length:                         %d\n", descriptor.length);
    USB_TRACE("| Type:                           %x\n", descriptor.type);
    USB_TRACE("| Total Length:                   %d\n", descriptor.totalLength);
    USB_TRACE("| Number of Interfaces:           %d\n", descriptor.numInterfaces);
    USB_TRACE("| Configuration Value:            %x\n", descriptor.configValue);
    USB_TRACE("| Configuration String ID:        %x\n", descriptor.configString);
    USB_TRACE("| Attributes:                     %x\n", descriptor.attributes);
    USB_TRACE("| Max Power:                      %dmA\n", descriptor.maxPower * 2);
    USB_TRACE("|--------------------------------------------------------------|\n");
    USB_TRACE("\n");

    for (uint8_t i = 0; i < interfaces.length(); i++) {
        interfaces.get(i)->print();
    }
}

UsbInterface *UsbConfiguration::getInterface(uint8_t index) {
    return interfaces.get(index);
}
