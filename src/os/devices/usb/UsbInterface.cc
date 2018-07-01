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
#include "UsbInterface.h"
#include "Usb.h"


UsbInterface::UsbInterface(UsbInterface::Descriptor *descriptor) {
    parse(descriptor);
}

uint8_t UsbInterface::getId() const {
    return id;
}

void UsbInterface::setId(uint8_t id) {
    UsbInterface::id = id;
}

uint8_t UsbInterface::getNumEndpoints() const {
    return numEndpoints;
}

void UsbInterface::setNumEndpoints(uint8_t numEndpoints) {
    UsbInterface::numEndpoints = numEndpoints;
}

uint8_t UsbInterface::getInterfaceClass() const {
    return interfaceClass;
}

void UsbInterface::setInterfaceClass(uint8_t interfaceClass) {
    UsbInterface::interfaceClass = interfaceClass;
}

uint8_t UsbInterface::getInterfaceSubClass() const {
    return interfaceSubClass;
}

void UsbInterface::setInterfaceSubClass(uint8_t interfaceSubClass) {
    UsbInterface::interfaceSubClass = interfaceSubClass;
}

uint8_t UsbInterface::getInterfaceProtocol() const {
    return interfaceProtocol;
}

void UsbInterface::setInterfaceProtocol(uint8_t interfaceProtocol) {
    UsbInterface::interfaceProtocol = interfaceProtocol;
}

char *UsbInterface::getName() const {
    return name;
}

void UsbInterface::setName(char *name) {
    UsbInterface::name = name;
}

UsbEndpoint *UsbInterface::getEndpoint(uint8_t index) const {
    return endpoints.get(index);
}

void UsbInterface::addEndpoint(UsbEndpoint *endpoint) {
    endpoints.add(endpoint);
}

void UsbInterface::parse(UsbInterface::Descriptor *descriptor) {

    UsbInterface::descriptor = *descriptor;

    id = descriptor->number;
    numEndpoints = descriptor->numEndpoints;
    interfaceClass = descriptor->classCode;
    interfaceSubClass = descriptor->subClassCode;
    interfaceProtocol = descriptor->protocolCode;

    UsbEndpoint *endpoint;
    UsbEndpoint::Descriptor *endpointDescriptor = (UsbEndpoint::Descriptor*) ((uint32_t) descriptor + descriptor->length);

    for(uint8_t i = 0; i < numEndpoints; i++) {
        endpoint = new UsbEndpoint(endpointDescriptor);

        endpoints.add(endpoint);

        endpointDescriptor = (UsbEndpoint::Descriptor*) ((uint32_t) endpointDescriptor + endpointDescriptor->length);
    }
}

void UsbInterface::print() {

    Logger::trace("USB", "|--------------------------------------------------------------|");
    Logger::trace("USB", "| USB Interface Descriptor");
    Logger::trace("USB", "|--------------------------------------------------------------|");
    Logger::trace("USB", "| Length:                         %d", descriptor.length);
    Logger::trace("USB", "| Type:                           %x", descriptor.type);
    Logger::trace("USB", "| Interface Number:               %d", descriptor.number);
    Logger::trace("USB", "| Alternate Setting:              %x", descriptor.altSetting);
    Logger::trace("USB", "| Number of Endpoints:            %d", descriptor.numEndpoints);
    Logger::trace("USB", "| Interface Class:                %x", descriptor.classCode);
    Logger::trace("USB", "| Interface Subclass:             %x", descriptor.subClassCode);
    Logger::trace("USB", "| Interface Protocol:             %x", descriptor.protocolCode);
    Logger::trace("USB", "| Interface String ID:            %x", descriptor.nameIndex);
    Logger::trace("USB", "|--------------------------------------------------------------|");
    Logger::trace("USB", "");

    for (uint8_t i = 0; i < endpoints.length(); i++) {
        endpoints.get(i)->print();
    }
}
