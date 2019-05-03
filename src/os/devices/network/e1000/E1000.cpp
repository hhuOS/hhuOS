/*
 * Copyright (C) 2018/19 Thiemo Urselmann
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
 *
 * Note:
 * All references marked with [...] refer to the following developers manual.
 * Intel Corporation. PCI/PCI-X Family of Gigabit Ethernet Controllers Software Developer’s Manual.
 * 317453006EN.PDF Revision 4.0. 2009.
 */

#include <kernel/Kernel.h>
#include <kernel/memory/SystemManagement.h>
#include "E1000.h"
#include "kernel/services/NetworkService.h"

E1000::E1000() : interruptBuffer(1024) {
    eventBus = Kernel::getService<EventBus>();
}

void E1000::initialize(const Pci::Device &dev, Logger &driverLog, E1000 *driver) {
    driverLog.info("Determining Descriptor Params");
    setUpDescriptorParams(128, 128);

    driverLog.info("Enabling Bus Master");
    Pci::enableBusMaster(dev.bus, dev.device, dev.function);

    driverLog.info("Mapping MMIO-Space");
    uint32_t bar0 = Pci::readDoubleWord(dev.bus, dev.device, dev.function, Pci::PCI_HEADER_BAR0);
    mmioBase = (uint8_t *) SystemManagement::getInstance().mapIO(bar0 & ~0xFu, mmioSize);

    driverLog.info("Allocating Descriptor Buffers");
    auto trmDscBuffer   = (uint8_t *) SystemManagement::getInstance().mapIO(transmitBlockSize);
    auto rcvDscBuffer   = (uint8_t *) SystemManagement::getInstance().mapIO(receiveBlockSize);
    auto trmPhyBuffer   = (uint64_t)  SystemManagement::getInstance().getPhysicalAddress(trmDscBuffer);
    auto rcvPhyBuffer   = (uint64_t)  SystemManagement::getInstance().getPhysicalAddress(rcvDscBuffer);

    driverLog.info("Creating Descriptor Blocks");
    auto trmDescriptors = createTransmitDescriptorBlock(trmDscBuffer);
    auto rcvDescriptors = createReceiveDescriptorBlock(rcvDscBuffer);

    driverLog.info("Allocate Main Classes");
    initializeAttributeClasses(trmDescriptors, rcvDescriptors, trmPhyBuffer, rcvPhyBuffer);

    driverLog.info("Initializing Descriptor Rings");
    transmitter->initialize();
    receiver->initialize();

    driverLog.info("Setting Controls");
    setDeviceControl(deviceControl);
    setInterruptControl(interruptEnable, interruptDisable);
    setTransmitControl(transmit);
    setReceiveControl(receive);

    driverLog.info("Loading MAC-Address");
    loadMac();

    driverLog.info("Registering to Network Service");
    Kernel::getService<NetworkService>()->registerDevice(*driver);

    driverLog.info("Setting Interrupts");
    plugin();
    InterruptManager::getInstance().registerInterruptHandler(this);
    interruptCause->readAndClear();

    driverLog.info("Creating Virtual Nodes");
    createNodes();
}

uint32_t E1000::calculateBlockSize(uint32_t descriptors) {
    return (((descriptors * descriptorBytes) / pageSize) + 1) * pageSize;
}

void E1000::setUpDescriptorParams(uint16_t receiveSets, uint16_t transmitSets) {
    receiveDescriptors = receiveSets * descriptorSet;
    transmitDescriptors = transmitSets * descriptorSet;

    transmitBlockSize = calculateBlockSize(transmitDescriptors);
    receiveBlockSize = calculateBlockSize(receiveDescriptors);
}

void E1000::clearBufferExcerpt(uint8_t *address, uint32_t bytes) {
    for (uint32_t i = 0; i < bytes; i++) {
        *(address + i) = 0;
    }
}

uint8_t E1000::getSubClass() const {
    return Pci::SUBCLASS_ETHERNET_CONTROLLER;
}

uint8_t E1000::getBaseClass() const {
    return Pci::CLASS_NETWORK_CONTROLLER;
}

PciDeviceDriver::SetupMethod E1000::getSetupMethod() const {
    return PciDeviceDriver::BY_ID;
}

void E1000::sendPacket(void *address, uint16_t length) {
    if(transmitter == nullptr) {
        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE, "E1000 is not configured correctly: transmitter is nullptr");
    }

    transmitter->sendPacket(reinterpret_cast<uint64_t *>(address), length);
}

void E1000::getMacAddress(uint8_t *buf) {
    mac->getByteRepresentation(buf);
}

void E1000::parseInterruptData() {
    Util::Pair<void*, uint16_t> descriptor = interruptBuffer.pop();

    Util::SmartPointer<Event> event = Util::SmartPointer<Event>(new ReceiveEvent(descriptor.first, descriptor.second));

    eventBus->publish(event);
}

bool E1000::hasInterruptData() {
    return !interruptBuffer.isEmpty();
}



