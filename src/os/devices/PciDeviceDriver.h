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

#ifndef HHUOS_PCIDEVICEDRIVER_H
#define HHUOS_PCIDEVICEDRIVER_H

#define PCI_DEVICE_DRIVER_IMPLEMENT_CREATE_INSTANCE(TYPE) \
        PciDeviceDriver *createInstance() const override { return new TYPE(); }

#include <cstdint>
#include <lib/util/HashSet.h>
#include "Pci.h"

class PciDeviceDriver {

public:

    enum SetupMethod {
        BY_ID,
        BY_CLASS,
        BY_PROGRAM_INTERFACE
    };

    PciDeviceDriver() = default;

    PciDeviceDriver(const PciDeviceDriver &copy) = default;

    virtual ~PciDeviceDriver() = default;

    virtual PciDeviceDriver *createInstance() const = 0;

    virtual Util::Array<Util::Pair<uint16_t, uint16_t>> getIdPairs() const {
        Util::Pair<uint16_t, uint16_t> pair(0, 0);
        Util::Array<Util::Pair<uint16_t, uint16_t>> ret(1);

        ret[0] = pair;

        return ret;
    };

    virtual uint8_t getBaseClass() const {
        return 0;
    };

    virtual uint8_t getSubClass() const {
        return 0;
    };

    virtual uint8_t getProgramInterface() const {
        return 0;
    }

    virtual SetupMethod getSetupMethod() const = 0;

    virtual void setup(const Pci::Device &device) = 0;
};

#endif
