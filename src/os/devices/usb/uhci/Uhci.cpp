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

#include "Uhci.h"
#include <kernel/interrupts/IntDispatcher.h>
#include <kernel/interrupts/Pic.h>
#include <kernel/Kernel.h>
#include <kernel/log/Logger.h>

Logger &Uhci::log = Logger::get("UHCI");

Uhci::Uhci() {}

void Uhci::setup(const Pci::Device &dev) {

    pciDevice = dev;

    timeService = Kernel::getService<TimeService>();

    uint32_t base = Pci::readDoubleWord(dev.bus, dev.device,
         dev.function, Pci::PCI_HEADER_BAR4) & 0xFFFFFFF0;

    io.command              = new IOport(base + IO_OFFSET_USBCMD);
    io.status               = new IOport(base + IO_OFFSET_USBSTS);
    io.interrupt            = new IOport(base + IO_OFFSET_USBINTR);
    io.frameNumber          = new IOport(base + IO_OFFSET_FRNUM);
    io.frameBaseAddress     = new IOport(base + IO_OFFSET_FRBASEADD);
    io.sofModify            = new IOport(base + IO_OFFSET_SOFMOD);
    io.port1                = new IOport(base + IO_OFFSET_PORTSC1);
    io.port2                = new IOport(base + IO_OFFSET_PORTSC2);

    Pci::enableBusMaster(dev.bus, dev.device, dev.function);
    Pci::enableIoSpace(dev.bus, dev.device, dev.function);

    disableLegacySupport();

    stop();

    disableInterrupts();

    // plugin();

    //globalReset();

    reset();

    configure();

    start();

    io.command->outw(io.command->inw() | REG_USBCMD_CF);

    enablePorts();

    printSummary();
}

void Uhci::printSummary() {
    log.trace("|-----------------------------------------|");
    log.trace("|               UHCI                      |");
    log.trace("|-----------------------------------------|");
    log.trace("   BASE:                         %x", io.command->getAddress());
    log.trace("   LEGACY SUPPORT:               %x", Pci::readWord(pciDevice.bus, pciDevice.device, pciDevice.function, CAP_LEGSUP));
    log.trace("   STATUS:                       %x", io.status->inw());
    log.trace("   COMMAND:                      %x", io.command->inw());
    log.trace("   INTERRUPT:                    %x", io.interrupt->inw());
    log.trace("   FRNUM:                        %x", io.frameNumber->inw());
    log.trace("   FRBASEADDR:                   %x", io.frameBaseAddress->indw());
    log.trace("   SOFMOD:                       %x", io.sofModify->inb());
    log.trace("   PORT1 (%x)                    %x", io.port1->getAddress(), io.port1->inw());
    log.trace("   PORT2 (%x)                    %x", io.port2->getAddress(), io.port2->inw());
    log.trace("   INTERRUPT LINE:               %d", pciDevice.intr);
    log.trace("|-----------------------------------------|");
}

void Uhci::acknowledgeInterrupts() {
    io.status->outw(REG_USBSTS_USBINT | 
                    REG_USBSTS_USBEINT | 
                    REG_USBSTS_RD |
                    REG_USBSTS_HSE |
                    REG_USBSTS_HCPE);
}

void Uhci::enableInterrupts() {
    io.interrupt->outw(REG_USBINTR_TIE |
                       REG_USBINTR_RIE |
                       REG_USBINTR_IOC |
                       REG_USBINTR_SPIE);
}

void Uhci::disableInterrupts() {
    io.interrupt->outw(0x0);
}

void Uhci::globalReset() {
    uint16_t cmd = io.command->inw();
    io.command->outw(cmd |  REG_USBCMD_GRESET);
    timeService->msleep(50);
    io.command->outw(cmd & ~REG_USBCMD_GRESET);
}

void Uhci::configure() {
    
    disableInterrupts();
    
    io.command->outw(0x0);

    io.port1->outw(0x0);
    io.port2->outw(0x0);

    FrameList *frameList = (FrameList*) aligned_alloc(4096, sizeof(FrameList));

    for (uint32_t i = 0; i < 1024; i++) {
        frameList->entries[i] = 0x1;
    }

    io.sofModify->outb(REG_SOFMOD_1MS);

    io.frameNumber->outw(0x0);

    io.frameBaseAddress->outdw((uint32_t) frameList);

    io.command->outw(REG_USBCMD_MAXP);
}

void Uhci::enablePorts() {
    resetPort(io.port1);
    resetPort(io.port2);
}

void Uhci::resetPort(IOport *port) {
    log.trace("Resetting port %x", port->getAddress());

    port->outw(port->inw() | REG_PORTSC_CSC | REG_PORTSC_PEC);

    port->outw(port->inw() | REG_PORTSC_PR);
    timeService->msleep(100);
    port->outw( port->inw() & ~REG_PORTSC_PR);

    timeService->msleep(10);

    if ( port->inw() & REG_PORTSC_CCS ) {
        log.trace(" -> Device detected");
    }
}

void Uhci::plugin() {
    log.trace("Assigning interrupt %d", pciDevice.intr);

    IntDispatcher::getInstance().assign((uint8_t ) pciDevice.intr + 32, *this);
    Pic::getInstance()->allow(pciDevice.intr);
}

void Uhci::trigger() {
    log.trace("INTERRUPT");
}

void Uhci::reset() {
    log.trace("Resetting Host Controller");
    uint16_t cmd = io.command->inw();
    io.command->outw(cmd |  REG_USBCMD_HCRESET);

    while ( io.command->inw() & REG_USBCMD_HCRESET );
    log.trace("Host Controller reset");

    disableInterrupts();
    stop();
}

void Uhci::start() {
    log.trace("Starting Host Controller");
    uint16_t cmd = io.command->inw();
    io.command->outw(cmd | REG_USBCMD_RS);

    while ( (io.status->inw() & REG_USBSTS_HCH) );
    log.trace("Host Controller started");
}

void Uhci::stop() {
    log.trace("Stopping Host Controller");
    uint16_t cmd = io.command->inw();
    io.command->outw(cmd &  ~REG_USBCMD_RS);

    while ( !(io.status->inw() & REG_USBSTS_HCH) );
    log.trace("Host Controller stopped");
}

void Uhci::disableLegacySupport() {
    Pci::writeWord(pciDevice.bus, pciDevice.device, pciDevice.function, CAP_LEGSUP, 0x8F00);
    log.trace("Disabled Legacy Support");
}
