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

#include "devices/usb/Usb.h"

Usb::Request Usb::getStringDescriptor(uint8_t offset) {
    return { 0x80, GET_DESCRIPTOR, offset, STRING_DESCRIPTOR, LANGUAGE_ENGLISH, 0x0100};
}

Usb::Request Usb::getConfigurationDescriptor(uint8_t offset) {
    return { 0x80, GET_DESCRIPTOR, offset, CONFIGURATION_DESCRIPTOR, 0x0000, 0x0100};
}

Usb::Request Usb::getInterfaceDescriptor(uint8_t offset) {
    return { 0x80, GET_DESCRIPTOR, offset, INTERFACE_DESCRIPTOR, 0x0000, 0x0100};
}

Usb::Request Usb::setAddress(uint8_t address) {
    return { REQUEST_SET, SET_ADDRESS, address, 0x00, 0x0000, 0x0000};
}

Usb::Request Usb::setConfiguration(uint8_t configuration) {
    return { REQUEST_SET, SET_CONFIGURATION, configuration, 0x00, 0x0000, 0x0000};
}

Usb::Request Usb::getConfiguration() {
    return { REQUEST_GET, GET_CONFIGURATION, 0x00, 0x00, 0x0000, 0x0001};
}

Usb::Request Usb::bulkReset(uint16_t interface) {
    return { 0x21, BULK_RESET, 0x00, 0x00, interface, 0x0000};
}

Usb::Request Usb::getMaxLun(uint16_t interface) {
    return { REQUEST_GET | 0x21, MAX_LUN, 0x00, 0x00, interface, 0x0001};
}

Usb::Request Usb::getDeviceDescriptor() {
    return { REQUEST_GET, GET_DESCRIPTOR, 0x00, DEVICE_DESCRIPTOR, 0x0000, 0x0011};
}

void Usb::fixString(char* string) {
    uint8_t start;
    for (start = 2; string[start] != '\0'; start += 2) {
        string[start / 2] = string[start];
        string[start] = '\0';
    }
}
