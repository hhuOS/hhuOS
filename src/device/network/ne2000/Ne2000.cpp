/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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
 * The Ne2000 driver is based on a bachelor's thesis, written by Marcel Thiel.
 * The original source code can be found here: https://github.com/Spectranis/bachelorthesis-mthiel/tree/ne2k_dev
 */

#include "Ne2000.h"

#include "kernel/service/NetworkService.h"
#include "kernel/log/Log.h"
#include "lib/util/time/Timestamp.h"
#include "kernel/service/InterruptService.h"
#include "device/bus/pci/Pci.h"
#include "device/bus/pci/PciDevice.h"
#include "kernel/service/Service.h"
#include "lib/util/async/Thread.h"
#include "lib/util/collection/Array.h"

namespace Kernel {
enum InterruptVector : uint8_t;
struct InterruptFrame;
}  // namespace Kernel

namespace Device::Network {

Ne2000::Ne2000(const PciDevice &device) : pciDevice(device) {
    LOG_INFO("Configuring PCI registers");
    uint16_t command = pciDevice.readWord(Pci::COMMAND);
    command |= Pci::IO_SPACE;
    pciDevice.writeWord(Pci::COMMAND, command);

    /** Source for "~0x3": https://wiki.osdev.org/Ne2000, Initialization and MAC Address */
    uint16_t ioBaseAddress = pciDevice.readDoubleWord(Pci::BASE_ADDRESS_0) & ~0x3;
    baseRegister = IoPort(ioBaseAddress);

    LOG_INFO("Resetting device");
    baseRegister.writeByte(baseRegister.readByte(RESET), RESET);

    /** Wait until Reset Status is 0 */
    while(!(baseRegister.readByte(P0_ISR) & ISR_RST)) {
        Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(1));
    }

    LOG_INFO("Initializing device");
    /** 1) CR Register: Page 0, disable DMA, Stop NIC */
    baseRegister.writeByte(COMMAND,STOP_DMA | STP | PAGE0);
    /** 2) Initialize DCR */
    baseRegister.writeByte(P0_DCR,DCR_AR | DCR_FT1 | DCR_LS);
    /** 3) Clear RBCR0, RBCR1 */
    baseRegister.writeByte(P0_RBCR0,0);
    baseRegister.writeByte(P0_RBCR1,0);

    /** 4) Initialize RCR */
    baseRegister.writeByte(P0_RCR,RCR_AR | RCR_AB | RCR_AM);

    /** 5) Set NIC to Loopback-mode -> TCR = 02/04h */
    baseRegister.writeByte(P0_TCR,TCR_LB0);

    /** 6) Initialize Receive Buffer Ring ( BNDRY, ReceiveStartPage, ReceiveStopPage )
     * P.156  http://www.bitsavers.org/components/national/_dataBooks/1988_National_Data_Communications_Local_Area_Networks_UARTs_Handbook.pdf#page=156
     * Accessed: 2024-03-29
     */
    baseRegister.writeByte(P0_TPSR, TRANSMIT_START_PAGE);
    baseRegister.writeByte(P0_PSTART, RECEIVE_START_PAGE);
    baseRegister.writeByte(P0_BNRY, RECEIVE_START_PAGE + 1);
    baseRegister.writeByte(P0_PSTOP, RECEIVE_STOP_PAGE);

    /** 7) Clear ISR */
    baseRegister.writeByte(P0_ISR,0xFF);

    /** 8) Initialize IMR  */
    baseRegister.writeByte(P0_IMR,IMR_PRXE | IMR_PTXE | IMR_OVWE);

    /** 9) Switch to P1, disable DMA and Stop the NIC */
    baseRegister.writeByte(COMMAND,STP | STOP_DMA | PAGE1);

    /** Read 32 Byte from IOPort */
    uint8_t buffer[32];
    for (uint16_t i = 0; i < 32; i++) {
        buffer[i] = baseRegister.readByte(ioPort);
    }

    /** 9) i) Initialize Physical Address Register: PAR0-PAR5
     * each mac address bit is written two times into the buffer
     */
    baseRegister.writeByte(P1_PAR0,buffer[0]);
    baseRegister.writeByte(P1_PAR1,buffer[2]);
    baseRegister.writeByte(P1_PAR2,buffer[4]);
    baseRegister.writeByte(P1_PAR3,buffer[6]);
    baseRegister.writeByte(P1_PAR4,buffer[8]);
    baseRegister.writeByte(P1_PAR5,buffer[10]);

    /** 9) ii) Initialize Multicast Address Register: MAR0-MAR7 with 0xFF */
    for(uint8_t i = 0; i < 8; i++) {
        baseRegister.writeByte(P1_MAR0 + i, 0xFF);
    }

    /** P.156 http://www.bitsavers.org/components/national/_dataBooks/1988_National_Data_Communications_Local_Area_Networks_UARTs_Handbook.pdf#page=156
     * Accessed: 2024-03-29
     */
    currentNextPagePointer = RECEIVE_START_PAGE + 1;

    /** 9) iii) Initialize Current Pointer: CURR */
    baseRegister.writeByte(P1_CURR, currentNextPagePointer);

    /** 10) Start NIC */
    baseRegister.writeByte(COMMAND,STOP_DMA | STA | PAGE0);

    /** 11) Initialize TCR and RCR */
    baseRegister.writeByte(P0_TCR,0);
    baseRegister.writeByte(P0_RCR,RCR_AR | RCR_AB | RCR_AM);
}

void Ne2000::initializeAvailableCards() {
    auto &networkService = Kernel::Service::getService<Kernel::NetworkService>();
    auto devices = Pci::search(VENDOR_ID, DEVICE_ID);

    for (const auto &device: devices) {
        auto *ne2000 = new Ne2000(device);
        networkService.registerNetworkDevice(ne2000, "eth");
        ne2000->plugin();
    }
}

Util::Network::MacAddress Ne2000::getMacAddress() const {
    /** Go to P1, STOP and block Remote DMA */
    baseRegister.writeByte(COMMAND, STOP_DMA | STP | PAGE1);
    /** Read MAC from PAR0 - PAR5 */
    uint8_t buffer[6];
    for(uint8_t i = 0; i < 6; i++) {
        buffer[i] = baseRegister.readByte(P1_PAR0 + i);
    }

    /** COMMAND: Start NIC */
    baseRegister.writeByte(COMMAND, STA | PAGE0);

    return Util::Network::MacAddress(buffer);
}

void Ne2000::handleOutgoingPacket(const uint8_t *packet, uint32_t packetLength) {
    /** Check if Ne2000 is ready to transmit */
    while(!readyToTransmit()){
        Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(1));
    }

    /** Important to do before every transmit to ensure reliable NIC operation */
    dummyReadBeforeWrite();

    /** 1) Load RBCR with packet size */
    baseRegister.writeByte(P0_RBCR0, packetLength & 0xFF);
    baseRegister.writeByte(P0_RBCR1, packetLength >> 8);
    /** 2) Clear RDC Interrupt */
    baseRegister.writeByte(P0_ISR, ISR_RDC);
    /** 3) Load RSAR with 0 (low bits) and Page Number (high bits)*/
    baseRegister.writeByte(P0_RSAR0, 0);
    baseRegister.writeByte(P0_RSAR1, TRANSMIT_START_PAGE);
    /** 4) Set COMMAND to remote write */
    baseRegister.writeByte(COMMAND, REMOTE_WRITE | STA | PAGE0);

    /** 5) Write packet to remote DMA */
    for (uint32_t i = 0; i < packetLength; i++) {
        baseRegister.writeByte(ioPort, packet[i]);
    }

    /** 6) Poll ISR until remote DMA Bit is set */
    while((baseRegister.readByte(P0_ISR) & ISR_RDC) == 0){
        Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(1));
    }

    /** 7) Clear ISR RDC Interrupt */
    baseRegister.writeByte(P0_ISR, ISR_RDC);

    /** Set TBCR Bits before Transmit and TPSR Bit */
    baseRegister.writeByte(P0_TBCR0, packetLength & 0xFF);
    baseRegister.writeByte(P0_TBCR1, packetLength >> 8);
    baseRegister.writeByte(P0_TPSR, TRANSMIT_START_PAGE);

    /** Set TXP Bit to send packet */
    baseRegister.writeByte(COMMAND, STA | TXP | STOP_DMA | PAGE0);
}

bool Ne2000::readyToTransmit() {
    return !(baseRegister.readByte(COMMAND) & TXP);
}

void Ne2000::dummyReadBeforeWrite() {
    baseRegister.writeByte(COMMAND, STA | STOP_DMA | PAGE0);

    /** 1) Save CRDA bit */
    uint16_t oldCrda = baseRegister.readByte(P0_CRDA0) + (baseRegister.readByte(P0_CRDA1) << 8);

    /** 2.1 ) Set RBCR > 0 */
    baseRegister.writeByte(P0_RBCR0, 0x01);
    baseRegister.writeByte(P0_RBCR1, 0);
    /** 2.2) Set RSAR to unused address */
    baseRegister.writeByte(P0_RSAR0, TRANSMIT_START_PAGE);
    baseRegister.writeByte(P0_RSAR1, 0);
    /** 3) Issue Dummy Remote READ Command */
    baseRegister.writeByte(COMMAND, STA | REMOTE_READ | PAGE0);

    /** 4) Mandatory Delay between Dummy Read and Write to ensure dummy read was successful */
    /** Wait until crda value has changed */
    while (oldCrda == (baseRegister.readByte(P0_CRDA0) + (baseRegister.readByte(P0_CRDA1) << 8))){
        Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(1));
    }
}

void Ne2000::trigger([[maybe_unused]] const Kernel::InterruptFrame &frame, [[maybe_unused]] Kernel::InterruptVector slot) {
    /** Just to be sure go to Page0 and disable remote DMA */
    baseRegister.writeByte(COMMAND, STOP_DMA | PAGE0);
    /** Disable all Interrupts */
    baseRegister.writeByte(P0_IMR, 0);

    /** Get current Interrupts */
    uint8_t interrupt = baseRegister.readByte(P0_ISR);

    /** Packet Received Interrupt */
    if (interrupt & ISR_PRX) {
        /** Reset Interrupt */
        baseRegister.writeByte(P0_ISR, ISR_PRX);
        processReceivedPackets();
    }

    /** Packet Transmission Interrupt */
    if (interrupt & ISR_PTX) {
        /** Reset Interrupt */
        baseRegister.writeByte(P0_ISR, ISR_PTX);

        /** Check TX Status if only PTX Bit is set, remove last packet from send buffer */
        uint8_t tsr = baseRegister.readByte(P0_TSR);
        if (tsr == TSR_PTX) {
            freeLastSendBuffer();
        } else {
            /** TODO: Handle errors
            if (tsr & TSR_COL) {
                LOG_ERROR("Transmit failed: Transmit Collided");
            }
            if (tsr & TSR_ABT) {
                LOG_ERROR("Transmit failed: Transmit Aborted");
            }
            if (tsr & TSR_CRS) {
                LOG_ERROR("Transmit failed: Carrier Sense Lost");
            }
            if (tsr & TSR_FU) {
                LOG_ERROR("Transmit failed: FIFU underrun");
            }
            if (tsr & TSR_CDH) {
                LOG_ERROR("Transmit failed: CD Heartbeat");
            }
            if (tsr & TSR_COL) {
                LOG_ERROR("Transmit failed: Out of Window Collision");
            } */

            freeLastSendBuffer();
        }
    }

    /** Overwrite Interrupt */
    if (interrupt & ISR_OVW) {
        handleOverwriteWarning();
    }

    /** Send EOI (EndOfInterrupt) and re-enable Interrupts */
    baseRegister.writeByte(P0_IMR, IMR_PRXE | IMR_PTXE | IMR_OVWE);
}

void Ne2000::handleOverwriteWarning() {
    /** Save TXP bit*/
    uint8_t txp = baseRegister.readByte(COMMAND) & TXP;

    /** Issue STOP Command and wait at least 1.6 ms */
    baseRegister.writeByte(COMMAND, STOP | PAGE0);
    Util::Async::Thread::sleep(Util::Time::Timestamp::ofMicroseconds(1600));

    /** Clear RBCR Registers */
    baseRegister.writeByte(P0_RBCR0, 0);
    baseRegister.writeByte(P0_RBCR1, 0);

    /** Check if retransmit is necessary */
    bool resend = false;
    if(txp && (baseRegister.readByte(P0_ISR) & (ISR_PTX | ISR_TXE))){
        resend = true;
    }

    /** Put NIC in loopback and exec STA */
    baseRegister.writeByte(P0_TCR, TCR_LB0);
    baseRegister.writeByte(COMMAND, STA | PAGE0);

    /** Remove packets still in buffer */
    processReceivedPackets();

    /** Reset OVW Bit */
    baseRegister.writeByte(P0_ISR, ISR_OVW);

    /** urn loopback off and start normal operation */
    baseRegister.writeByte(P0_TCR, 0);

    /** Resend packet if needed */
    if(resend){
        baseRegister.writeByte(COMMAND, STA | TXP | STOP_DMA | PAGE0);
    }
}

void Ne2000::processReceivedPackets() {
    /** Get Curr Register */
    baseRegister.writeByte(COMMAND, STA | STOP_DMA | PAGE1);
    uint8_t current = baseRegister.readByte(P1_CURR);
    baseRegister.writeByte(COMMAND, STA | STOP_DMA | PAGE0);

    /**
     * Loop until all packets received are processed
     * As long as CURR and the currentNextPagePointer are not equal, there are packets to process
     */
    while (current != currentNextPagePointer) {
        /** Get NIC Header */
        baseRegister.writeByte(P0_RBCR0, sizeof(PacketHeader));
        baseRegister.writeByte(P0_RBCR1, 0);
        baseRegister.writeByte(P0_RSAR0, 0);
        baseRegister.writeByte(P0_RSAR1, currentNextPagePointer);

        baseRegister.writeByte(COMMAND, STA | REMOTE_READ | PAGE0);

        /** Read NIC Header for the received packet from the buffer ring */
        PacketHeader packetHeader{};
        packetHeader.rsr = baseRegister.readByte(ioPort);
        packetHeader.next = baseRegister.readByte(ioPort);
        packetHeader.length = baseRegister.readByte(ioPort) + (baseRegister.readByte(ioPort) << 8) - sizeof(PacketHeader);

        /** Check if packet received is intact and has max Ethernet packet length */
        if ((packetHeader.rsr) & RSR_PRX && packetHeader.length <= MAX_ETHERNET_PACKET_SIZE) {
            uint8_t packet[MAX_ETHERNET_PACKET_SIZE];

            /** Write packet length into RBCR */
            baseRegister.writeByte(P0_RBCR0, packetHeader.length & 0XFF);
            baseRegister.writeByte(P0_RBCR1, packetHeader.length >> 8);
            /** Set RSAR0 to nicHeaderLength to skip the packet header during the read operation */
            baseRegister.writeByte(P0_RSAR0, sizeof(PacketHeader));
            baseRegister.writeByte(P0_RSAR1, currentNextPagePointer);

            baseRegister.writeByte(COMMAND, STA | REMOTE_READ | PAGE0);

            /** Read Packet Data from I/O Port and write it into packet */
            for (uint16_t i = 0; i < packetHeader.length; i++) {
                packet[i] = baseRegister.readByte(ioPort);
            }

            /** Handover packet to NetworkDevice */
            handleIncomingPacket(packet, packetHeader.length);
        }

        /**
         * Update Pointer to the next packet and BNRY register
         * https://datasheetspdf.com/pdf-file/549771/NationalSemiconductor/DP8390D/1 Page 11
         */
        currentNextPagePointer = packetHeader.next;
        if ((packetHeader.next - 1) < RECEIVE_START_PAGE) {
            baseRegister.writeByte(P0_BNRY, RECEIVE_STOP_PAGE - 1);
        } else{
            baseRegister.writeByte(P0_BNRY, currentNextPagePointer - 1);
        }

        /** Read current register to prepare for the next packet */
        baseRegister.writeByte(COMMAND, STA | STOP_DMA | PAGE1);
        current = baseRegister.readByte(P1_CURR);
        baseRegister.writeByte(COMMAND, STA | STOP_DMA | PAGE0);
    }

    /** Clear RDC interrupt */
    baseRegister.writeByte(P0_ISR, ISR_RDC);

}

void Ne2000::plugin() {
    auto &interruptService = Kernel::Service::getService<Kernel::InterruptService>();
    interruptService.assignInterrupt(static_cast<Kernel::InterruptVector>(pciDevice.getInterruptLine() + 32), *this);
    interruptService.allowHardwareInterrupt(pciDevice.getInterruptLine());
}

}