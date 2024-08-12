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

#ifndef HHUOS_NE2000_H
#define HHUOS_NE2000_H

#include <stdint.h>

#include "device/network/NetworkDevice.h"
#include "device/cpu/IoPort.h"
#include "kernel/interrupt/InterruptHandler.h"
#include "lib/util/network/MacAddress.h"
#include "device/bus/pci/PciDevice.h"

namespace Kernel {
enum InterruptVector : uint8_t;
struct InterruptFrame;
}  // namespace Kernel

namespace Device::Network {

class Ne2000 : public NetworkDevice, Kernel::InterruptHandler {

public:
    /**
     * Initialize Ne2000 according to manual
     * P.29 https://datasheetspdf.com/pdf-file/549771/NationalSemiconductor/DP8390D/1
     * Accessed: 2024-03-29
     */
    explicit Ne2000(const PciDevice &device);

    /**
     * Copy Constructor.
     */
    Ne2000(const Ne2000 &other) = delete;

    /**
     * Assignment operator.
     */
    Ne2000 &operator=(const Ne2000 &other) = delete;

    /**
     * Destructor.
     */
    ~Ne2000() override = default;


    /**
     * Initialize cards that are supported by this driver
     */
    static void initializeAvailableCards();

    /**
     * Read MacAddress form PAR0 - PAR5 registers
     */

    [[nodiscard]] Util::Network::MacAddress getMacAddress() const override;

    void plugin() override;

    /**
     * Implemented according to flow Chart "Interrupt Service Routine" Page 2
     * http://www.osdever.net/documents/WritingDriversForTheDP8390.pdf
     * Accessed: 2024-03-29
     */
    void trigger(const Kernel::InterruptFrame &frame, Kernel::InterruptVector slot) override;

protected:

    /**
     * Transmit a packet via the NIC. Overwrites the virtual method of NetworkDevice
     * http://www.osdever.net/documents/WritingDriversForTheDP8390.pdf
     * Page 3 FIGURE 3.Driver Send Routine
     * https://wiki.osdev.org/Ne2000
     * https://datasheetspdf.com/pdf-file/549771/NationalSemiconductor/DP8390D/1
     * Page 12 Chapter 8.0 Packet Transmission, TRANSMISSION
     * Accessed 2024-03-30
     */
    void handleOutgoingPacket(const uint8_t *packet, uint32_t packetLength) override;

private:

    /**
     * Page Register as defined in DP8390D
     * P.17f. https://datasheetspdf.com/pdf-file/549771/NationalSemiconductor/DP8390D/1
     * Accessed: 2024-03-29
     */
    enum PageRegisters : uint8_t {
        COMMAND     = 0x00,         /** R|W COMMAND used for P0, P1, P2 */
        /** P0 Write */
        P0_PSTART   = 0x01,        /** W Page Start Register  */
        P0_PSTOP    = 0x02,        /** W Page Stop Register  */
        P0_BNRY     = 0x03,        /** R|W Boundary Pointer  P0 */
        P0_TPSR     = 0x04,        /** W Transmit Page Start Address  */
        P0_TBCR0    = 0x05,        /** W Transmit Byte Count Register 0  */
        P0_TBCR1    = 0x06,        /** W Transmit Byte Count Register 1  */
        P0_ISR      = 0x07,        /** R|W Interrupt Status Register P0 */
        P0_RSAR0    = 0x08,        /** W Remote Start Address Register 0 */
        P0_RSAR1    = 0x09,        /** W Remote Start Address Register 1 */
        P0_RBCR0    = 0x0A,        /** W Remote Byte Count Register 0 */
        P0_RBCR1    = 0x0B,        /** W Remote Byte Count Register 1 */
        P0_RCR      = 0x0C,        /** W Receive Configuration Register */
        P0_TCR      = 0x0D,        /** W Transmit Configuration Register*/
        P0_DCR      = 0x0E,        /** W Data Configuration Register */
        P0_IMR      = 0x0F,        /** W Interrupt Mask Register */
        /** P0 Read Registers*/
        P0_CLDA0    = 0x01,        /** R Current Local DMA Address 0  */
        P0_CLDA1    = 0x02,        /** R Current Local DMA Address 1  */
        P0_TSR      = 0x04,        /** R Transmit Status Register  */
        P0_NCR      = 0x05,        /** R Number of Collisions Register  */
        P0_FIFO     = 0x06,        /** R FIFO */
        P0_CRDA0    = 0x08,        /** R Current Remote DMA Address 0 */
        P0_CRDA1    = 0x09,        /** R Current Remote DMA Address 1 */
        P0_RSR      = 0x0C,        /** R Receive Status Register */
        P0_CNTR0    = 0x0D,        /** R Tally Counter 0 (Frame Alignment Errors) */
        P0_CNTR1    = 0x0E,        /** R Tally Counter 1 (CRC Errors) */
        P0_CNTR2    = 0x0F,        /** R Tally Counter 2 (Missed Packet Error) */
        /** P1 Read and Write Registers */
        P1_PAR0     = 0x01,        /** R|W Physical Address Register 0 */
        P1_PAR1     = 0x02,        /** R|W Physical Address Register 1 */
        P1_PAR2     = 0x03,        /** R|W Physical Address Register 2 */
        P1_PAR3     = 0x04,        /** R|W Physical Address Register 3 */
        P1_PAR4     = 0x05,        /** R|W Physical Address Register 4 */
        P1_PAR5     = 0x06,        /** R|W Physical Address Register 5 */
        P1_CURR     = 0x07,        /** R|W Current Page Register */
        P1_MAR0     = 0x08,        /** R|W Multicast Address Register 0 */
        P1_MAR1     = 0x09,        /** R|W Multicast Address Register 1 */
        P1_MAR2     = 0x0A,        /** R|W Multicast Address Register 2 */
        P1_MAR3     = 0x0B,        /** R|W Multicast Address Register 3 */
        P1_MAR4     = 0x0C,        /** R|W Multicast Address Register 4 */
        P1_MAR5     = 0x0D,        /** R|W Multicast Address Register 5 */
        P1_MAR6     = 0x0E,        /** R|W Multicast Address Register 6 */
        P1_MAR7     = 0x0F,        /** R|W Multicast Address Register 7 */
        /** P2 Registers are only for diagnostic purposes. They should not be accessed during normal operation */
        /** P2 Write Registers */
        P2_CLDA0    = 0x01,        /** W Current Local DMA Address 0 */
        P2_CLDA1    = 0x02,        /** W Current Local DMA Address 1 */
        P2_RNPP     = 0x03,        /** R|W Remote Next Packet Pointer */
        P2_LNPP     = 0x05,        /** R|W Local Next Packet Pointer */
        P2_UPPER    = 0x06,        /** R|W Address Counter (Upper) */
        P2_LOWER    = 0x07,        /** R|W Address Counter (Lower) */
        /** P2 Read */
        P2_PSTART   = 0x01,        /** R Page Start Register */
        P2_PSTOP    = 0x02,        /** R Page Stop Register */
        P2_TPSR     = 0x04,        /** R Transmit Page Start Address */
        P2_RCR      = 0x0C,        /** R Receive Configuration Register */
        P2_TCR      = 0x0D,        /** R Transmit Configuration Register */
        P2_DCR      = 0x0E,        /** R Data Configuration Register */
        P2_IMR      = 0x0F         /** R Interrupt Mask Register */
    };

    /**
         * Command Register as defined in DP8390D
         * P.19 https://datasheetspdf.com/pdf-file/549771/NationalSemiconductor/DP8390D/1
         * Accessed: 2024-03-29
         */
    enum CommandRegister : uint8_t {
        STP = 0x01, /** STOP */
        STA = 0x02, /** START */
        TXP = 0x04, /** Transmit Packet */
        RD0 = 0x08, /** Remote DMA Command 0 */
        RD1 = 0x10, /** Remote DMA Command 1 */
        RD2 = 0x20, /** Remote DMA Command 2*/
        PS0 = 0x40, /** Page Select PS0 */
        PS1 = 0x80, /** Page Select PS1 */
        /** Page Selection Commands */
        PAGE0 = 0x00,
        PAGE1 = PS0,
        PAGE2 = PS1,
        /** Remote DMA Commands */
        REMOTE_READ = RD0,
        REMOTE_WRITE = RD1,
        SEND_PACKET = RD0 | RD1,
        STOP_DMA = RD2,
        STOP = STP | STOP_DMA
    };

    /**
     * Interrupt Status Register as defined in DP8390D
     * P.20 https://datasheetspdf.com/pdf-file/549771/NationalSemiconductor/DP8390D/1
     * Accessed: 2024-03-29
     */
    enum InterruptStatusRegister : uint8_t {
        ISR_PRX = 0x01, /** Packet Received */
        ISR_PTX = 0x02, /** Packet Transmitted */
        ISR_RXE = 0x04, /** Receive Error */
        ISR_TXE = 0x08, /** Transmit Error */
        ISR_OVW = 0x10, /** Overwrite Error */
        ISR_CNT = 0x20, /** Counter Overflow */
        ISR_RDC = 0x40, /** Remote DMA Complete
                         * Is set after every Remote DMA Operation and needs to be cleared after each
                         * http://www.bitsavers.org/components/national/_dataBooks/1988_National_Data_Communications_Local_Area_Networks_UARTs_Handbook.pdf#page=159
                         */
        ISR_RST = 0x80  /** Reset Status */
    };

    /**
     * Interrupt Mask Register as defined in DP8390D
     * P.21 https://datasheetspdf.com/pdf-file/549771/NationalSemiconductor/DP8390D/1
     * Accessed: 2024-03-29
     */
    enum InterruptMaskRegister : uint8_t {
        IMR_PRXE = 0x01, /** Packet Received Interrupt Enable */
        IMR_PTXE = 0x02, /** Packet Transmit Interrupt Enable */
        IMR_RXEE = 0x04, /** Receive Error Interrupt Enable */
        IMR_TXEE = 0x08, /** Transmit Error Interrupt Enable */
        IMR_OVWE = 0x10, /** Overwrite Warning Interrupt Enable */
        IMR_CNTE = 0x20, /** Counter Overflow Interrupt Enable */
        IMR_RDCE = 0x40  /** DMA Complete Interrupt Enable */
        /** 0x80 reserved */
    };

    /**
     * Data Configuration Register as defined in DP8390D
     * P.22 https://datasheetspdf.com/pdf-file/549771/NationalSemiconductor/DP8390D/1
     * Accessed: 2024-03-29
     */
    enum DataConfigurationRegister : uint8_t {
        DCR_WTS = 0x01, /** Word Transfer Select */
        DCR_BOS = 0x02, /** Byte Order Select */
        DCR_LAS = 0x04, /** Long Address Select */
        DCR_LS  = 0x08, /** Loop-back Select */
        DCR_AR  = 0x10, /** Auto-Initialize Remote */
        DCR_FT0 = 0x20, /** FIFO Threshold Select 0 */
        DCR_FT1 = 0x40, /** FIFO Threshold Select 1 */
    };

    /**
     * Transmit Configuration Register as defined in DP8390D
     * P.23 https://datasheetspdf.com/pdf-file/549771/NationalSemiconductor/DP8390D/1
     * Accessed: 2024-03-29
     */
    enum TransmitConfigurationRegister : uint8_t {
        TCR_CRC  = 0x01, /** Inhibit CRC */
        TCR_LB0  = 0x02, /** Encoded Loop-back Control */
        TCR_LB1  = 0x04, /** Encoded Loop-back Control */
        TCR_ATD  = 0x08, /** Auto Transmit Disable */
        TCR_OFST = 0x10, /** Collision Offset Enable */
    };

    /**
     * Transmit Status Register as defined in DP8390D
     * P. 24https://datasheetspdf.com/pdf-file/549771/NationalSemiconductor/DP8390D/1
     * Accessed: 2024-03-29
     */
    enum TransmitStatusRegister : uint8_t {
        TSR_PTX = 0x01, /** Packet Transmit */
        TSR_COL = 0x02, /** Transmit Collided */
        TSR_ABT = 0x04, /** Transmit Aborted */
        TSR_CRS = 0x08, /** Carrier Sense Lost */
        TSR_FU  = 0x10, /** FIFO Under-run */
        TSR_CDH = 0x20, /** CD Heartbeat */
        TSR_OWC = 0x40  /** Out of Window Collision */
    };

    /**
     * Receive Configuration Register as defined in DP8390D
     * P.25 https://datasheetspdf.com/pdf-file/549771/NationalSemiconductor/DP8390D/1
     * Accessed: 2024-03-29
     */
    enum ReceiveConfigurationRegister : uint8_t {
        RCR_SEP = 0x01, /** Save Error Packets */
        RCR_AR  = 0x02, /** Accept Runt Packets */
        RCR_AB  = 0x04, /** Accept Broadcast */
        RCR_AM  = 0x08, /** Accept Multicast */
        RCR_PRO = 0x10, /** Promiscuous Physical */
        RCR_MON = 0x20, /** Monitor Mode */
    };

    /**
     * Receive Status Register as defined in DP8390D
     * P.26 https://datasheetspdf.com/pdf-file/549771/NationalSemiconductor/DP8390D/1
     * Accessed: 2024-03-29
     */
    enum ReceiveStatusRegister : uint8_t {
        RSR_PRX = 0x01, /** Packet Received Intact */
        RSR_CRC = 0x02, /** CRC Error */
        RSR_FAE = 0x04, /** Frame Alignment Error */
        RSR_FO  = 0x08, /** FIFO Overrun */
        RSR_MPA = 0x10, /** Missed Packet*/
        RSR_PHY = 0x20, /** Physical/Multicast Address */
        RSR_DIS = 0x40, /** Receiver Disabled */
        RSR_DFR = 0x80,  /** Deferring */
    };

    /**
     * Packet Header
     * P.11 Storage Format
     * https://datasheetspdf.com/pdf-file/549771/NationalSemiconductor/DP8390D/1
     * Accessed: 2024-03-29
     */
    struct PacketHeader {
        uint8_t rsr;        /** Receive Status Register Content */
        uint8_t next;       /** Next Ringbuffer Address */
        uint16_t length;    /** Packet Length */
    };

    /**
     * Check if Command register has bits for start and no DMA set
     * There's no blocking operation -> NIC is ready to transmit
     */
    bool readyToTransmit();

    /**
     * Necessary to ensure reliable NIC operations
     * Not doing this can result in data corruption and other issues
     * Source: https://datasheetspdf.com/pdf-file/549771/NationalSemiconductor/DP8390D/1
     * Page 13: Chapter 9; Setting PRQ Using the Remote Read
     * Accessed: 2024-03-29
     */
    void dummyReadBeforeWrite();

    /**
     * Handle OVW Interrupt according to the flow chart "Overflow Routine Flow Chart"
     * in Chapter 7.0. Packet Reception Page 9
     * https://datasheetspdf.com/pdf-file/549771/NationalSemiconductor/DP8390D/1
     * Accessed: 2024-03-29
     */
    void handleOverwriteWarning();

    /**
     * Read packets from the NIC buffer ring and hand them over to the NetworkDevice software
     * Linux, torokernel and the NatSem manual were helpful
     * https://github.com/torvalds/linux/blob/master/drivers/net/ethernet/8390/lib8390.c#L659
     * https://github.com/torokernel/torokernel/blob/7d6df4c40fa4cc85febd5fd5799404592ffdff53/rtl/drivers/Ne2000.pas
     * https://datasheetspdf.com/pdf-file/549771/NationalSemiconductor/DP8390D/1 Page 11
     * Accessed: 2024-03-30
     */
    void processReceivedPackets();

    PciDevice pciDevice;
    IoPort baseRegister = IoPort(0x00);

    uint8_t currentNextPagePointer;

    /**
     * Defined here: https://github.com/hisilicon/qemu/blob/master/include/hw/pci/pci_ids.h#L197
     * Accessed: 2024-03-29
     */
    static const constexpr uint16_t VENDOR_ID = 0x10ec;

    /**
     * Defined here: https://github.com/hisilicon/qemu/blob/master/include/hw/pci/pci.h#L48
     * Accessed: 2024-03-29
     */
    static const constexpr uint16_t DEVICE_ID = 0x8029;

    /** Input/Output Port Address */
    static const constexpr uint8_t ioPort = 0x10;

    /**
     * Buffer Page where the transmitted packets are placed:
     * P.6 TRANSMITBUFFER http://www.osdever.net/documents/WritingDriversForTheDP8390.pdf
     * Accessed: 2024-03-29
     */
    static const constexpr uint8_t TRANSMIT_START_PAGE = 0x40;

    /**
     * Reception Buffer Ring Start Page
     * http://www.osdever.net/documents/WritingDriversForTheDP8390.pdf
     * Page 4 PSTART
     */
    static const constexpr uint8_t RECEIVE_START_PAGE = 0x46;

    /**
     * Reception Buffer Ring End
     * P.4 PSTOP http://www.osdever.net/documents/WritingDriversForTheDP8390.pdf
     * Accessed: 2024-03-29
     */
    static const constexpr uint8_t RECEIVE_STOP_PAGE = 0x80;

    /**
     * Address to reset the NIC according to https://wiki.osdev.org/Ne2000
     * Accessed: 2024-03-29
     */
    static const constexpr uint8_t RESET = 0x1F;
};

}

#endif