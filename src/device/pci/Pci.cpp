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

#include "device/storage/ahci/AhciDevice.h"
#include "kernel/core/Kernel.h"
#include "kernel/log/Logger.h"
#include "lib/libc/sprintf.h"
#include "lib/file/tar/Archive.h"
#include "kernel/multiboot/Structure.h"
#include "Pci.h"

#include "kernel/memory/manager/IOMemoryManager.h"
#include "PciDeviceDriver.h"

Logger &Pci::log = Logger::get("PCI");

// PCI registers
const IoPort Pci::CONFIG_ADDRESS = IoPort(0xCF8);
const IoPort Pci::CONFIG_DATA = IoPort(0xCFC);

Util::ArrayList<Pci::Device> Pci::pciDevices;
Util::ArrayList<PciDeviceDriver*> Pci::deviceDrivers;
Util::HashMap<String, Pci::Vendor> Pci::vendors;

Spinlock Pci::databaseLock;

StorageService *Pci::storageService = nullptr;

void Pci::prepareRegister(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    uint32_t address;
    uint32_t lbus = (uint32_t) bus;
    uint32_t ldevice = (uint32_t) device;
    uint32_t lfunc = (uint32_t) function;

    address = (uint32_t)((lbus << 16) | (ldevice << 11) |
        (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));

    CONFIG_ADDRESS.outdw(address);
}

uint32_t Pci::readDoubleWord(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    prepareRegister(bus, device, function, offset);
    return CONFIG_DATA.indw();
}

uint16_t Pci::readWord(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    prepareRegister(bus, device, function, offset);
    return (uint16_t)(( CONFIG_DATA.indw() >> ((offset & 0x02) * 8)) & 0xFFFF);
}

uint8_t Pci::readByte(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    prepareRegister(bus, device, function, offset);
    return (uint8_t)(( CONFIG_DATA.indw() >> ((offset & 0x03) * 8)) & 0xFF);
}

void Pci::writeDoubleWord(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint32_t value) {
    prepareRegister(bus, device, function, offset);
    CONFIG_DATA.outdw(value);
}

void Pci::writeWord(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint32_t value) {
    prepareRegister(bus, device, function, offset);
    CONFIG_DATA.outw(value);
}

void Pci::writeByte(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint32_t value) {
    prepareRegister(bus, device, function, offset);
    CONFIG_DATA.outb((offset & 0x03), value);
}

uint16_t Pci::getVendorId(uint8_t bus, uint8_t device, uint8_t function) {
    return readWord(bus, device, function, PCI_HEADER_VENDOR);
}

uint16_t Pci::getDeviceId(uint8_t bus, uint8_t device, uint8_t function) {
    return readWord(bus, device, function, PCI_HEADER_DEVICE);
}

uint8_t Pci::getClassId(uint8_t bus, uint8_t device, uint8_t function) {
    return readByte(bus, device, function, PCI_HEADER_CLASS);
}

uint8_t Pci::getSubclassId(uint8_t bus, uint8_t device, uint8_t function) {
    return readByte(bus, device, function, PCI_HEADER_SUBCLASS);
}

uint8_t Pci::getSecondaryBus(uint8_t bus, uint8_t device, uint8_t function) {
    return readByte(bus, device, function, PCI_HEADER_SECONDARY_BUS);
}

uint8_t Pci::getHeaderType(uint8_t bus, uint8_t device, uint8_t function) {
    return readByte(bus, device, function, PCI_HEADER_TYPE);
}

uint16_t Pci::getCommand(uint8_t bus, uint8_t device, uint8_t function) {
    return readWord(bus, device, function, PCI_HEADER_COMMAND);
}

uint16_t Pci::getStatus(uint8_t bus, uint8_t device, uint8_t function) {
    return readWord(bus, device, function, PCI_HEADER_STATUS);
}

uint8_t Pci::getRevision(uint8_t bus, uint8_t device, uint8_t function) {
    return readByte(bus, device, function, PCI_HEADER_REVISION);
}

uint8_t Pci::getCacheLineSize(uint8_t bus, uint8_t device, uint8_t function) {
    return readByte(bus, device, function, PCI_HEADER_CLS);    
}

uint8_t Pci::getMasterLatencyTimer(uint8_t bus, uint8_t device, uint8_t function) {
    return readByte(bus, device, function, PCI_HEADER_MLT);    
}

uint32_t Pci::getAbar(uint8_t bus, uint8_t device, uint8_t function) {
    return readDoubleWord(bus, device, function, PCI_HEADER_BAR5);    
}

uint16_t Pci::getSubsysId(uint8_t bus, uint8_t device, uint8_t function) {
    return readWord(bus, device, function, PCI_HEADER_SSID);    
}

uint16_t Pci::getSubsysVendorId(uint8_t bus, uint8_t device, uint8_t function) {
    return readWord(bus, device, function, PCI_HEADER_SSVID);
}

uint8_t Pci::getCapabilityPointer(uint8_t bus, uint8_t device, uint8_t function) {
    return readByte(bus, device, function, PCI_HEADER_CAP);   
}

uint16_t Pci::getInterruptInfo(uint8_t bus, uint8_t device, uint8_t function) {
    return readWord(bus, device, function, PCI_HEADER_INTR);
}

uint8_t Pci::getProgrammingInterface(uint8_t bus, uint8_t device, uint8_t function) {
    return readByte(bus, device, function, PCI_HEADER_PROGIF);
}

Pic::Interrupt Pci::getInterruptLine(uint8_t bus, uint8_t device, uint8_t function) {
    return Pic::Interrupt(readByte(bus, device, function, PCI_HEADER_INTR));
}

void Pci::enableBusMaster(uint8_t bus, uint8_t device, uint8_t function) {

    uint16_t cmd = getCommand(bus, device, function);
    uint16_t sts = getStatus(bus, device, function);

    cmd |= PCI_HEADER_COMMAND_BME;

    uint32_t value = (uint32_t) sts << 16 | (uint32_t) cmd;

    writeDoubleWord(bus, device, function, PCI_HEADER_COMMAND, value);
}

void Pci::enableIoSpace(uint8_t bus, uint8_t device, uint8_t function) {

    uint16_t cmd = getCommand(bus, device, function);
    uint16_t sts = getStatus(bus, device, function);

    cmd |= PCI_HEADER_COMMAND_IO;

    uint32_t value = (uint32_t) sts << 16 | (uint32_t) cmd;

    writeDoubleWord(bus, device, function, PCI_HEADER_COMMAND, value);
}

void Pci::enableMemorySpace(uint8_t bus, uint8_t device, uint8_t function) {
    uint16_t cmd = getCommand(bus, device, function);
    uint16_t sts = getStatus(bus, device, function);

    cmd |= PCI_HEADER_COMMAND_MSE;

    uint32_t value = (uint32_t) sts << 16 | (uint32_t) cmd;

    writeDoubleWord(bus, device, function, PCI_HEADER_COMMAND, value);
}

Pci::Device Pci::readDevice(uint8_t bus, uint8_t device, uint8_t function) {

    Device dev = {};

    dev.bus = bus;
    dev.device = device;
    dev.function = function;
    dev.vendorId = getVendorId(bus, device, function);
    dev.deviceId = getDeviceId(bus, device, function);
    dev.revision = getRevision(bus, device, function);
    dev.ssid = getSubsysId(bus, device, function);
    dev.ssvid = getSubsysVendorId(bus, device, function);
    dev.pi = getProgrammingInterface(bus, device, function);
    dev.baseClass = getClassId(bus, device, function);
    dev.subClass = getSubclassId(bus, device, function);
    dev.cap = getCapabilityPointer(bus, device, function);
    dev.intr = getInterruptLine(bus, device, function);

    return dev;
}

uint8_t Pci::findCapability(uint8_t bus, uint8_t device, uint8_t function, uint8_t capId) {
    uint8_t cap = getCapabilityPointer(bus, device, function);

    while (cap != 0x0) {
        if ( readByte(bus, device, function, cap) == capId ) {
            break;
        }

        cap = readByte(bus, device, function, cap + 1);
    }

    return cap;
}

void Pci::checkFunction(uint8_t bus, uint8_t device, uint8_t function) {
    uint8_t secondaryBus;

    uint8_t baseClass = getClassId(bus, device, function);
    uint8_t subClass = getSubclassId(bus, device, function);

    if ( (baseClass == CLASS_BRIDGE_DEVICE) && (subClass == SUBCLASS_PCI_TO_PCI) ) {
         log.info("Found PCI-to-PCI Bridge on bus %u", bus);
         secondaryBus = getSecondaryBus(bus, device, function);
         scanBus(secondaryBus);
    }

    Device dev = readDevice(bus, device, function);

    log.info("Found PCI-Device %04x:%04x on bus %u", dev.vendorId, dev.deviceId, bus);

    pciDevices.add(dev);
}

void Pci::checkDevice(uint8_t bus, uint8_t device) {
    uint8_t function = 0;

    uint16_t vendorId = getVendorId(bus, device, function);
    if(vendorId == INVALID_VENDOR) return;
    checkFunction(bus, device, function);
    uint16_t headerType = getHeaderType(bus, device, function);
    if( (headerType & MULTIFUNCTION_BIT) != 0 ) {
        for(function = 1; function < NUM_FUNCTIONS; function++) {
            if(getVendorId(bus, device, function) != INVALID_VENDOR) {
                checkFunction(bus, device, function);
            }
        }
    }
}

void Pci::scanBus(uint8_t bus) {
    uint8_t device;

    for(device = 0; device < NUM_DEVICES; device++) {
         checkDevice(bus, device);
     }
}

void Pci::scan() {
    
    storageService = Kernel::getService<StorageService>();

    uint8_t function;
    uint8_t bus;
    uint8_t headerType = getHeaderType(0, 0, 0);

    if( (headerType & MULTIFUNCTION_BIT) == 0) {
        scanBus(0);
    } else {
        for(function = 0; function < NUM_FUNCTIONS; function++) {
            if(getVendorId(0, 0, function) != INVALID_VENDOR) {
                break;
            }
            bus = function;
            scanBus(bus);
        }
    }
}

Util::ArrayList<Pci::Device>& Pci::getDevices() {
    return pciDevices;
}

void Pci::setupDeviceDriver(PciDeviceDriver &driver) {
    for(const auto &device : pciDevices) {
        if(driver.getSetupMethod() == PciDeviceDriver::BY_CLASS) {
            if(device.baseClass == driver.getBaseClass() && device.subClass == driver.getSubClass()) {
                PciDeviceDriver *newDriver = driver.createInstance();

                newDriver->setup(device);

                deviceDrivers.add(newDriver);
            }
        } else if(driver.getSetupMethod() == PciDeviceDriver::BY_PROGRAM_INTERFACE) {
            if(device.baseClass == driver.getBaseClass() && device.subClass == driver.getSubClass() && device.pi == driver.getProgramInterface()) {
                PciDeviceDriver *newDriver = driver.createInstance();

                newDriver->setup(device);

                deviceDrivers.add(newDriver);
            }
        } else if(driver.getSetupMethod() == PciDeviceDriver::BY_ID) {
            for(Util::Pair<uint16_t, uint16_t> pair : driver.getIdPairs()) {
                if (device.vendorId == pair.first && device.deviceId == pair.second) {
                    PciDeviceDriver *newDriver = driver.createInstance();

                    newDriver->setup(device);

                    deviceDrivers.add(newDriver);
                }
            }
        }
    }
}

void Pci::uninstallDeviceDriver(PciDeviceDriver &driver) {
    //TODO: Implement removing of pci drivers.
}

void Pci::setVendorName(const String &vendorId, const Vendor &vendor) {

    Vendor tmp = vendor;

    tmp.devices = new Util::HashMap<String, String>();

    vendors.put(vendorId, tmp);
}

String Pci::getVendorName(const String &vendorId) {

    databaseLock.acquire();

    String ret;

    if (vendors.containsKey(vendorId)) {

        ret = vendors.get(vendorId).name;
    }

    databaseLock.release();

    return "";
}

void Pci::parseDatabase() {

    databaseLock.acquire();

    File *idsFile = File::open("/initrd/pci/pci.ids", "r");

    if(idsFile == nullptr) {
        log.error("Unable to open PCI database file!");
        databaseLock.release();

        return;
    }

    char *content;

    *idsFile >> content;

    Util::Array<String> lines = String(content).split('\n');

    String vendorId, deviceId, deviceName;

    Vendor vendor;

    for (auto &line : lines) {

        if (line[0] == '#' || line.strip().isEmpty()) {

            continue;

        } else {

            if (line[0] == '\t' && line[1] == '\t') {
                // Ignore subdevices for now
            } else if (line[0] == '\t') {

                deviceId = line.strip().split(' ')[0].toUpperCase();

                deviceName = line.split(' ', 2)[1].strip();

                vendor = vendors.get(vendorId);

                vendor.devices->put(deviceId, deviceName);

                vendors.put(vendorId, vendor);

            } else {

                vendorId = line.strip().split(' ')[0].toUpperCase();

                vendor.name = line.split(' ', 2)[1].strip();

                vendor.devices = new Util::HashMap<String, String>();

                vendors.put(vendorId, vendor);
            }
        }
    }

    // Add QEMU VGA entry
    vendorId = "1234";
    deviceId = "1111";
    vendor.name = "QEMU";
    vendor.devices = new Util::HashMap<String, String>();
    vendor.devices->put(deviceId, "Standard VGA");
    vendors.put(vendorId, vendor);

    databaseLock.release();
}

String Pci::getIdentifier(const String &vendorId, const String &deviceId) {

    String ret;

    databaseLock.acquire();

    if (vendors.containsKey(vendorId)) {

        Vendor vendor = vendors.get(vendorId);

        ret += vendor.name + ": ";

        if (vendor.devices->containsKey(deviceId)) {

            ret += vendor.devices->get(deviceId);

        } else {

            ret += "<unknown>";
        }
    }

    databaseLock.release();

    return ret;
}

bool Pci::Device::operator!=(const Pci::Device &other) {

    return vendorId != other.vendorId && deviceId != other.deviceId;
}

bool Pci::Vendor::operator!=(const Pci::Vendor &other) const {

    return name != other.name;
}

bool Pci::Vendor::operator==(const Pci::Vendor &other) const {

    return name == other.name;
}

Pci::Vendor::operator uint32_t() const {

    return (uint32_t) name;
}
