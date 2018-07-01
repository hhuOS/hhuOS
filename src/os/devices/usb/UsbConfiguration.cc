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
#include "UsbConfiguration.h"

const String UsbConfiguration::LOG_NAME = String("USB");

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

    Logger::trace(LOG_NAME, "|--------------------------------------------------------------|");
    Logger::trace(LOG_NAME, "| USB Configuration Descriptor");
    Logger::trace(LOG_NAME, "|--------------------------------------------------------------|");
    Logger::trace(LOG_NAME, "| Length:                         %d", descriptor.length);
    Logger::trace(LOG_NAME, "| Type:                           %x", descriptor.type);
    Logger::trace(LOG_NAME, "| Total Length:                   %d", descriptor.totalLength);
    Logger::trace(LOG_NAME, "| Number of Interfaces:           %d", descriptor.numInterfaces);
    Logger::trace(LOG_NAME, "| Configuration Value:            %x", descriptor.configValue);
    Logger::trace(LOG_NAME, "| Configuration String ID:        %x", descriptor.configString);
    Logger::trace(LOG_NAME, "| Attributes:                     %x", descriptor.attributes);
    Logger::trace(LOG_NAME, "| Max Power:                      %dmA", descriptor.maxPower * 2);
    Logger::trace(LOG_NAME, "|--------------------------------------------------------------|");
    Logger::trace(LOG_NAME, "");

    for (uint8_t i = 0; i < interfaces.length(); i++) {
        interfaces.get(i)->print();
    }
}

UsbInterface *UsbConfiguration::getInterface(uint8_t index) {
    return interfaces.get(index);
}
