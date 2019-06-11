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

#include "device/network/e1000/transmit/TransmitRingSimple.h"
#include "device/network/e1000/descriptorRing/HrTransmit.h"
#include "device/network/e1000/descriptorRing/HrReceive.h"
#include "device/network/e1000/McBuffered.h"
#include "device/network/e1000/receive/ReceiveRingSimple.h"
#include "device/network/e1000/general/RgSimple.h"
#include "device/network/e1000/general/BmSimple.h"
#include "filesystem/core/Filesystem.h"
#include "device/network/e1000/nodes/MacAddressNode.h"
#include "device/network/e1000/transmit/descriptor/legacy/field/LegacyCommandSimple.h"
#include "device/network/e1000/transmit/descriptor/legacy/field/TransmitStatusSimple.h"
#include "device/network/e1000/receive/descriptor/field/RsBufferedSimple.h"
#include "kernel/core/Management.h"
#include "device/network/e1000/receive/descriptor/ReceiveDescriptorSimple.h"
#include "kernel/interrupt/InterruptDispatcher.h"
#include "Intel82540EM.h"
#include "Eeprom82540EM.h"
#include "TransmitDescriptor82540EM.h"
#include "BufferedReceiveErrors82540EM.h"
#include "MainControl82540EM.h"
#include "TransmitControl82540EM.h"
#include "ReceiveControl82540EM.h"
#include "InterruptDisable82540EM.h"
#include "BufferedInterruptCause82540EM.h"
#include "InterruptEnable82540EM.h"

void Intel82540EM::setup(const Pci::Device &dev) {
    E1000::initialize(dev, log, this);
}

Util::Array<Util::Pair<uint16_t, uint16_t>> Intel82540EM::getIdPairs() const {
    Util::Pair<uint16_t, uint16_t> i82541IP(0x8086, 0x100E);

    Util::Array<Util::Pair<uint16_t, uint16_t>> ret(1);
    ret[0] = i82541IP;

    return ret;
}

Descriptors<TransmitDescriptor *> *Intel82540EM::createTransmitDescriptorBlock(uint8_t *buffer) {
    auto descriptors = new Util::ArrayList<TransmitDescriptor *>();

    for(int i = 0; i < transmitDescriptors; i++) {
        uint8_t *address = buffer + i * descriptorBytes;
        clearBufferExcerpt(address, descriptorBytes);

        auto command = new LegacyCommandSimple(address + 11, new BmSimple<uint8_t>());
        auto status  = new TransmitStatusSimple(address + 12);

        descriptors->add(new TransmitDescriptor82540EM(address, command, status));
    }

    return new Descriptors<TransmitDescriptor *>(descriptors);
}

Descriptors<ReceiveDescriptor *> *Intel82540EM::createReceiveDescriptorBlock(uint8_t *buffer) {
    auto descriptors = new Util::ArrayList<ReceiveDescriptor *>();

    for(int i = 0; i < receiveDescriptors; i++) {
        uint8_t *address = buffer + i * descriptorBytes;
        clearBufferExcerpt(address, descriptorBytes);

        auto status     = new RsBufferedSimple(address + 12);
        auto errors     = new BufferedReceiveErrors82540EM(address + 13);

        auto virtualPacketBuffer    = (uint8_t *) Kernel::Management::getInstance().mapIO(rxBufferSize::ext_small);
        auto physicalPacketBuffer   = Kernel::Management::getInstance().getPhysicalAddress(virtualPacketBuffer);

        ReceiveDescriptor *descriptor = new ReceiveDescriptorSimple(address, virtualPacketBuffer, errors, status);
        descriptor->writeAddress((uint64_t) physicalPacketBuffer);

        descriptor->clearStatus();

        descriptors->add(descriptor);
    }
    return new Descriptors<ReceiveDescriptor *>(descriptors);
}

void Intel82540EM::initializeAttributeClasses(Descriptors<TransmitDescriptor *> *transmitDescriptors,
                                              Descriptors<ReceiveDescriptor *> *receiveDescriptors,
                                              uint64_t phyTransmitBlock, uint64_t phyReceiveBlock) {
    eeprom              = new Eeprom82540EM(new RgSimple((uint32_t *) (mmioBase + 0x00014), new BmSimple<uint32_t>()));
    deviceControl       = new MainControl82540EM(new RgSimple((uint32_t *) (mmioBase + 0x00000), new BmSimple<uint32_t>()));
    transmit            = new TransmitControl82540EM(new RgSimple((uint32_t *) (mmioBase + 0x00400), new BmSimple<uint32_t>()));
    receive             = new ReceiveControl82540EM(new RgSimple((uint32_t *) (mmioBase + 0x00100), new BmSimple<uint32_t>()));
    interruptEnable     = new InterruptEnable82540EM(new RgSimple((uint32_t *) (mmioBase + 0x000D0), new BmSimple<uint32_t>()));
    interruptDisable    = new InterruptDisable82540EM(new RgSimple((uint32_t *) (mmioBase + 0x000D8), new BmSimple<uint32_t>()));

    interruptCause      = new BufferedInterruptCause82540EM((uint32_t *) (mmioBase + 0xC0));
    transmitter = new TransmitRingSimple(transmitDescriptors, new HrTransmit(mmioBase + 0x3800u, phyTransmitBlock, E1000::transmitDescriptors));
    receiver = new ReceiveRingSimple(receiveDescriptors, new HrReceive(mmioBase + 0x2800u, phyReceiveBlock, E1000::receiveDescriptors), &interruptBuffer);
    mac = new McBuffered();
}

void Intel82540EM::setDeviceControl(DeviceControl *control) {
    control->autoSpeedDetection(true);
    control->setLinkUp(true);
    control->vlanMode(false);
    control->resetInternalPhy(false);
    control->manage();
}

void Intel82540EM::setInterruptControl(InterruptEnable *enable, InterruptDisable *disable) {disable->clearReserved();
    disable->manage();

    enable->linkStatusChange();
    enable->mdioAccessComplete();
    enable->generalPurposeInterrupts1(0b11);

    enable->receiveDescriptorMinimumThresholdHit();
    enable->receiverFifoOverrun();
    enable->receiveTimer();
    enable->smallReceivePacketDetection();

    enable->transmitDescriptorLowThresholdHit();
    enable->manage();
}

void Intel82540EM::setTransmitControl(TransmitControl *control) {
    control->enableTransmit(true);
    control->padShortPackets(true);
    control->manage();
}

void Intel82540EM::setReceiveControl(ReceiveControl *control) {
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

void Intel82540EM::trigger(Kernel::InterruptFrame &frame) {
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

    if( interruptCause->hasUnhandledInterrupts() ) {
        //log.info("Unhandled interrupt(s) received: %08x", interruptCause->getInterrupts());
    }

    interruptCause->readAndClear();
}

void Intel82540EM::loadMac() {
    mac->read(eeprom);
}

void Intel82540EM::plugin() {
    Kernel::InterruptDispatcher::getInstance().assign(Kernel::InterruptDispatcher::FREE3, *this);
    Pic::getInstance().allow(Pic::Interrupt::FREE3);
}

void Intel82540EM::createNodes() {
    static uint32_t cardCounter = 0;

    auto *fs = Kernel::System::getService<Filesystem>();
    String path = String::format("/dev/network/intel82540EM_%u", cardCounter);
    fs->createDirectory(path);

    fs->addVirtualNode(path, new MacAddressNode(mac->getStringRepresentation() + "\n"));

    cardCounter++;
}
