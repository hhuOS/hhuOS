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

#ifndef __UsbConfiguration_include__
#define __UsbConfiguration_include__

#include "devices/usb/Usb.h"
#include "devices/usb/UsbInterface.h"
#include "kernel/log/Logger.h"

#include <cstdint>

/**
 * Represents USB device configuration.
 *
 * @author Filip Krakowski
 */
class UsbConfiguration {

public:

    typedef struct {
        uint8_t     length;                 // Length
        uint8_t     type;                   // Type
        uint16_t    totalLength;            // Total combined Length
        uint8_t     numInterfaces;          // Number of Interfaces
        uint8_t     configValue;            // Configuration Value
        uint8_t     configString;           // Configuration String ID
        uint8_t     attributes;             // Attributes
        uint8_t     maxPower;               // Maximum Power (mA)
    } Descriptor;

    UsbConfiguration(UsbConfiguration::Descriptor *descriptor);

    uint8_t getId() const;

    void setId(uint8_t id);

    uint8_t getMaxPower() const;

    void setMaxPower(uint8_t maxPower);

    char *getName() const;

    void setName(char *name);

    bool isSupportsRemoteWakeup() const;

    void setSupportsRemoteWakeup(bool supportsRemoteWakeup);

    bool isIsSelfPowered() const;

    void setIsSelfPowered(bool isSelfPowered);

    uint8_t getNumInterfaces() const;

    void setNumInterfaces(uint8_t numInterfaces);

    UsbInterface *getInterface(uint8_t index);

    void print();

private:

    static Logger &log;

    UsbConfiguration::Descriptor descriptor;

    char *name;

    uint8_t id;
    uint8_t maxPower;
    uint8_t numInterfaces;

    bool supportsRemoteWakeup;
    bool isSelfPowered;

    LinkedList<UsbInterface> interfaces;

    void parse(Descriptor *descriptor);
};


#endif
