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

#ifndef __Usb_include__
#define __Usb_include__

#include <cstdint>

#include "lib/libc/printf.h"

/**
 * Universal Serial Bus helper class.
 *
 * @author Filip Krakowski
 */
class Usb {

public:

    typedef struct {
        uint8_t     type;
        uint8_t     request;
        uint8_t     valueLo;
        uint8_t     valueHi;
        uint16_t    index;
        uint16_t    length;
    } Request;

    /* Request Types */
    const static uint8_t    REQUEST_GET                     = 0x80;
    const static uint8_t    REQUEST_SET                     = 0x00;

    /* Requests */
    const static uint8_t    GET_STATUS                      = 0x00;
    const static uint8_t    CLEAR_FEATURE                   = 0x01;
    const static uint8_t    SET_FEATURE                     = 0x03;
    const static uint8_t    SET_ADDRESS                     = 0x05;
    const static uint8_t    GET_DESCRIPTOR                  = 0x06;
    const static uint8_t    SET_DESCRIPTOR                  = 0x07;
    const static uint8_t    GET_CONFIGURATION               = 0x08;
    const static uint8_t    SET_CONFIGURATION               = 0x09;
    const static uint8_t    BULK_RESET                      = 0xFF;
    const static uint8_t    MAX_LUN                         = 0xFE;

    /* Descriptor Types */
    const static uint8_t    DEVICE_DESCRIPTOR               = 0x01;
    const static uint8_t    CONFIGURATION_DESCRIPTOR        = 0x02;
    const static uint8_t    STRING_DESCRIPTOR               = 0x03;
    const static uint8_t    INTERFACE_DESCRIPTOR            = 0x04;
    const static uint8_t    ENDPOINT_DESCRIPTOR             = 0x05;
    const static uint8_t    DEVICE_QUALIFIER_DESCRIPTOR     = 0x06;
    const static uint8_t    OTHER_SPEED_DESCRIPTOR          = 0x07;
    const static uint8_t    INTERFACE_POWER_DESCRIPTOR      = 0x08;

    /* USB Language IDs */
    const static uint16_t   LANGUAGE_ENGLISH                = 0x0409;

    static Request getDeviceDescriptor();
    static Request getStringDescriptor(uint8_t offset);
    static Request getConfigurationDescriptor(uint8_t offset);
    static Request getInterfaceDescriptor(uint8_t offset);
    static Request setAddress(uint8_t address);
    static Request setConfiguration(uint8_t configuration);
    static Request getConfiguration();
    static Request bulkReset(uint16_t interface);
    static Request getMaxLun(uint16_t interface);

    static void fixString(char *string);

private:


};

#endif