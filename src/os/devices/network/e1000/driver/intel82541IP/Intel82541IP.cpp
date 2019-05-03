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


#include <devices/network/e1000/receive/descriptor/field/RsBufferedSimple.h>
#include <devices/network/e1000/receive/descriptor/ReceiveDescriptorSimple.h>
#include <kernel/Kernel.h>
#include <devices/network/e1000/transmit/descriptor/legacy/field/LegacyCommandSimple.h>
#include <devices/network/e1000/general/BmSimple.h>
#include <devices/network/e1000/transmit/descriptor/legacy/field/TransmitStatusSimple.h>
#include <kernel/memory/SystemManagement.h>
#include <kernel/interrupts/IntDispatcher.h>
#include <devices/network/e1000/general/RgSimple.h>
#include <devices/network/e1000/McBuffered.h>
#include <filesystem/FileSystem.h>
#include <devices/network/e1000/nodes/MacAddressNode.h>
#include <devices/network/e1000/transmit/TransmitRingSimple.h>
#include <devices/network/e1000/receive/ReceiveRingSimple.h>
#include <devices/network/e1000/descriptorRing/HrReceive.h>
#include <devices/network/e1000/descriptorRing/HrTransmit.h>
#include "Intel82541IP.h"
#include "BufferedReceiveErrors82541IP.h"
#include "MainControl82541IP.h"
#include "TransmitControl82541IP.h"
#include "ReceiveControl82541IP.h"
#include "InterruptEnable82541IP.h"
#include "InterruptDisable82541IP.h"
#include "BufferedInterruptCause82541IP.h"
#include "TransmitDescriptor82541IP.h"
#include "Eeprom82541IP.h"


void Intel82541IP::setup(const Pci::Device &dev) {
    E1000::initialize(dev, log, this);
}

void Intel82541IP::trigger(InterruptFrame &frame) {
    interruptCause->readAndClear();

    if(interruptCause->hasLinkStatusChanged()) {
        deviceControl->setLinkUp(true);
    }

    if(interruptCause->isReceiverOverrun()) {
        //log.info("Receiver Overrun (rx_head = %u).", *((uint32_t *) (mmioBase + 0x2810)));
    }

    if (interruptCause->isReceiveDescriptorMinimumThresholdReached()) {
        receiver->receivePoll(mmioBase);
    }

    if(interruptCause->hasReceiveTimerInterrupt()) {
        receiver->receivePoll(mmioBase);
    }

    if(interruptCause->hasUnhandledInterrupts()) {
        //log.info("Unhandled interrupt(s) received: %08x", interruptCause->getInterrupts());
    }

    interruptCause->readAndClear();
}

void Intel82541IP::plugin() {
    IntDispatcher::getInstance().assign(IntDispatcher::FREE3, *this);
    Pic::getInstance().allow(Pic::Interrupt::FREE3);
}

void Intel82541IP::setDeviceControl(DeviceControl *control) {
    control->autoSpeedDetection(true);
    control->setLinkUp(true);
    control->vlanMode(false);
    control->resetInternalPhy(false);
    control->manage();
}

void Intel82541IP::setInterruptControl(InterruptEnable *enable, InterruptDisable *disable) {
    disable->clearReserved();
    disable->manage();

    enable->linkStatusChange();
    enable->mdioAccessComplete();
    //todo remove?
    enable->generalPurposeInterrupts1(0b11);

    enable->receiveDescriptorMinimumThresholdHit();
    enable->receiverFifoOverrun();
    enable->receiveTimer();
    enable->smallReceivePacketDetection();

    enable->transmitDescriptorLowThresholdHit();
    enable->manage();
}

void Intel82541IP::setTransmitControl(TransmitControl *control) {
    control->enableTransmit(true);
    control->padShortPackets(true);
    control->manage();
}

void Intel82541IP::setReceiveControl(ReceiveControl *control) {
    control->storeBadPackets(true);
    control->longPacketReception(true);
    control->broadCastAcceptMode(true);
    control->unicastPromiscuous(true);
    control->multicastPromiscuous(true);

    control->bufferSize(rxBufferSize::ext_small);

    control->descriptorMinimumThresholdSize(rxRingLength::half);
    control->stripEthernetCrc(true);
    control->receiver(true);
    control->manage();
}

void Intel82541IP::initializeAttributeClasses(Descriptors<TransmitDescriptor *> *transmitDescriptors,
                                              Descriptors<ReceiveDescriptor *> *receiveDescriptors,
                                              uint64_t phyTransmitBlock,
                                              uint64_t phyReceiveBlock) {
    eeprom              = new Eeprom82541IP(new RgSimple((uint32_t *) (mmioBase + 0x00014), new BmSimple<uint32_t>()));
    deviceControl       = new MainControl82541IP(new RgSimple((uint32_t *) (mmioBase + 0x00000), new BmSimple<uint32_t>()));
    transmit            = new TransmitControl82541IP(new RgSimple((uint32_t *) (mmioBase + 0x00400), new BmSimple<uint32_t>()));
    receive             = new ReceiveControl82541IP(new RgSimple((uint32_t *) (mmioBase + 0x00100), new BmSimple<uint32_t>()));
    interruptEnable     = new InterruptEnable82541IP(new RgSimple((uint32_t *) (mmioBase + 0x000D0), new BmSimple<uint32_t>()));
    interruptDisable    = new InterruptDisable82541IP(new RgSimple((uint32_t *) (mmioBase + 0x000D8), new BmSimple<uint32_t>()));

    interruptCause      = new BufferedInterruptCause82541IP((uint32_t *) (mmioBase + 0xC0));
    transmitter         = new TransmitRingSimple(transmitDescriptors, new HrTransmit(mmioBase + 0x3800u, phyTransmitBlock, E1000::transmitDescriptors));
    receiver            = new ReceiveRingSimple(receiveDescriptors, new HrReceive(mmioBase + 0x2800u, phyReceiveBlock, E1000::receiveDescriptors), &interruptBuffer);
    mac                 = new McBuffered();
}

void Intel82541IP::loadMac() {
    mac->read(eeprom);
}

void Intel82541IP::createNodes() {
    static uint32_t cardCounter = 0;

    auto *fs = Kernel::getService<FileSystem>();
    String path = String::format("/dev/network/intel82541IP_%u", cardCounter);
    fs->createDirectory(path);

    fs->addVirtualNode(path, new MacAddressNode(mac->getStringRepresentation() + "\n"));

    cardCounter++;
}

Descriptors<TransmitDescriptor *> *Intel82541IP::createTransmitDescriptorBlock(uint8_t *buffer) {
    auto descriptors = new Util::ArrayList<TransmitDescriptor *>();

    for(int i = 0; i < transmitDescriptors; i++) {
        uint8_t *address = buffer + i * descriptorBytes;
        clearBufferExcerpt(address, descriptorBytes);

        auto command = new LegacyCommandSimple(address + 11, new BmSimple<uint8_t>());
        auto status  = new TransmitStatusSimple(address + 12);

        descriptors->add(new TransmitDescriptor82541IP(address, command, status));
    }

    return new Descriptors<TransmitDescriptor *>(descriptors);
}

Descriptors<ReceiveDescriptor *> * Intel82541IP::createReceiveDescriptorBlock(uint8_t *buffer) {
    auto descriptors = new Util::ArrayList<ReceiveDescriptor *>();

    for(int i = 0; i < receiveDescriptors; i++) {
        uint8_t *address = buffer + i * descriptorBytes;
        clearBufferExcerpt(address, descriptorBytes);

        auto status     = new RsBufferedSimple(address + 12);
        auto errors     = new BufferedReceiveErrors82541IP(address + 13);

        auto virtualPacketBuffer    = (uint8_t *) SystemManagement::getInstance().mapIO(rxBufferSize::ext_small);
        auto physicalPacketBuffer   = SystemManagement::getInstance().getPhysicalAddress(virtualPacketBuffer);

        ReceiveDescriptor *descriptor = new ReceiveDescriptorSimple(address, virtualPacketBuffer, errors, status);
        descriptor->writeAddress((uint64_t) physicalPacketBuffer);

        descriptor->clearStatus(); //todo überflüssig da ja clearBufferExcerpt verwendet wird, oder ? nur wenn status mit 0 initialisiert ist
        descriptors->add(descriptor);
    }
    return new Descriptors<ReceiveDescriptor *>(descriptors);
}

Util::Array<Util::Pair<uint16_t, uint16_t>> Intel82541IP::getIdPairs() const {
    Util::Pair<uint16_t, uint16_t> i82541IP(0x8086, 0x107C);
    Util::Array<Util::Pair<uint16_t, uint16_t>> ret(1);
    ret[0] = i82541IP;
    return ret;
}




