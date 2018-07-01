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

#ifndef __Uhci_include__
#define __Uhci_include__

#include "devices/Pci.h"
#include "kernel/interrupts/InterruptHandler.h"
#include "kernel/memory/MemLayout.h"
#include "kernel/services/TimeService.h"

#include <cstdint>
#include <devices/Pci.h>
#include <kernel/log/Logger.h>

/**
 * @author Filip Krakowski
 */
class Uhci : public InterruptHandler {

    struct FrameList {
        uint32_t  entries[1024];
    };

    struct QueueHead {
        uint32_t    qhlp;           // Queue Head Link Pointer
        uint32_t    qelp;           // Queue Element Link Pointer
    };

    struct UsbIo {
        /* 0x00 - USBCMD */
        IOport      *command;

        /* 0x02 - USBSTS */
        IOport      *status;

        /* 0x04 - USBINTR */
        IOport      *interrupt;

        /* 0x06 - FRNUM */
        IOport      *frameNumber;

        /* 0x08 - FRBASEADD */
        IOport      *frameBaseAddress;

        /* 0x0C - SOFMOD */
        IOport      *sofModify;

        /* 0x10 - PORTSC1 */
        IOport      *port1;

        /* 0x12 - PORTSC2 */
        IOport      *port2;

    };

private:

    static Logger &log;

    static const String LOG_NAME;

    Pci::Device pciDevice;

    UsbIo io;

    TimeService *timeService;

    uint8_t numPorts;

    void enableInterrupts();

    void disableInterrupts();

    void acknowledgeInterrupts();

    void printSummary();

    void globalReset();

    void reset();

    void stop();

    void start();

    void disableLegacySupport();

    void configure();

    void enablePorts();

    void resetPort(IOport *port);
    
public:
    
    Uhci();

    void plugin();

    void trigger();

    void setup(const Pci::Device &dev);

    /* IO Register Offsets */
    const static uint8_t    IO_OFFSET_USBCMD            = 0x00;
    const static uint8_t    IO_OFFSET_USBSTS            = 0x02;
    const static uint8_t    IO_OFFSET_USBINTR           = 0x04;
    const static uint8_t    IO_OFFSET_FRNUM             = 0x06;
    const static uint8_t    IO_OFFSET_FRBASEADD         = 0x08;
    const static uint8_t    IO_OFFSET_SOFMOD            = 0x0C;
    const static uint8_t    IO_OFFSET_PORTSC1           = 0x10;
    const static uint8_t    IO_OFFSET_PORTSC2           = 0x12;

    /* PCI Capabilities */
    const static uint8_t    CAP_LEGSUP                  = 0xC0;

    /* USB Commands */
    const static uint16_t   REG_USBCMD_RS               = (1 << 0);
    const static uint16_t   REG_USBCMD_HCRESET          = (1 << 1);
    const static uint16_t   REG_USBCMD_GRESET           = (1 << 2);
    const static uint16_t   REG_USBCMD_EGSM             = (1 << 3);
    const static uint16_t   REG_USBCMD_FGR              = (1 << 4);
    const static uint16_t   REG_USBCMD_SWDBG            = (1 << 5);
    const static uint16_t   REG_USBCMD_CF               = (1 << 6);
    const static uint16_t   REG_USBCMD_MAXP             = (1 << 7);

    /* USB Status */
    const static uint16_t   REG_USBSTS_USBINT           = (1 << 0);
    const static uint16_t   REG_USBSTS_USBEINT          = (1 << 1);
    const static uint16_t   REG_USBSTS_RD               = (1 << 2);
    const static uint16_t   REG_USBSTS_HSE              = (1 << 3);
    const static uint16_t   REG_USBSTS_HCPE             = (1 << 4);
    const static uint16_t   REG_USBSTS_HCH              = (1 << 5);

    /* USB Interrupts */
    const static uint16_t   REG_USBINTR_DISABLE         = (0 << 0);
    const static uint16_t   REG_USBINTR_TIE             = (1 << 0);
    const static uint16_t   REG_USBINTR_RIE             = (1 << 1);
    const static uint16_t   REG_USBINTR_IOC             = (1 << 2);
    const static uint16_t   REG_USBINTR_SPIE            = (1 << 3);

    /* Frame Number */
    const static uint16_t   REG_FRNUM_CURIDX            = 0x7FF;

    /* Start of Frame */
    const static uint8_t    REG_SOFMOD_1MS              = 0x40;

    /* Port Status/Control */
    const static uint16_t   REG_PORTSC_CCS              = (1 << 0);
    const static uint16_t   REG_PORTSC_CSC              = (1 << 1);
    const static uint16_t   REG_PORTSC_PE               = (1 << 2);
    const static uint16_t   REG_PORTSC_PEC              = (1 << 3);
    const static uint16_t   REG_PORTSC_LS               = (3 << 4);
    const static uint16_t   REG_PORTSC_RD               = (1 << 6);
    const static uint16_t   REG_PORTSC_LSDA             = (1 << 8);
    const static uint16_t   REG_PORTSC_PR               = (1 << 9);
    const static uint16_t   REG_PORTSC_SP               = (1 << 12);
};

#endif
