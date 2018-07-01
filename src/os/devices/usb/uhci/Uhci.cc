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

#include "Uhci.h"
#include <kernel/interrupts/IntDispatcher.h>
#include <kernel/interrupts/Pic.h>
#include <kernel/Kernel.h>
#include <kernel/log/Logger.h>

const String Uhci::LOG_NAME = String("UHCI");

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
    Logger::trace(LOG_NAME, "|-----------------------------------------|");
    Logger::trace(LOG_NAME, "|               UHCI                      |");
    Logger::trace(LOG_NAME, "|-----------------------------------------|");
    Logger::trace(LOG_NAME, "   BASE:                         %x", io.command->getAddress());
    Logger::trace(LOG_NAME, "   LEGACY SUPPORT:               %x", Pci::readWord(pciDevice.bus, pciDevice.device, pciDevice.function, CAP_LEGSUP));
    Logger::trace(LOG_NAME, "   STATUS:                       %x", io.status->inw());
    Logger::trace(LOG_NAME, "   COMMAND:                      %x", io.command->inw());
    Logger::trace(LOG_NAME, "   INTERRUPT:                    %x", io.interrupt->inw());
    Logger::trace(LOG_NAME, "   FRNUM:                        %x", io.frameNumber->inw());
    Logger::trace(LOG_NAME, "   FRBASEADDR:                   %x", io.frameBaseAddress->indw());
    Logger::trace(LOG_NAME, "   SOFMOD:                       %x", io.sofModify->inb());
    Logger::trace(LOG_NAME, "   PORT1 (%x)                    %x", io.port1->getAddress(), io.port1->inw());
    Logger::trace(LOG_NAME, "   PORT2 (%x)                    %x", io.port2->getAddress(), io.port2->inw());
    Logger::trace(LOG_NAME, "   INTERRUPT LINE:               %d", pciDevice.intr);
    Logger::trace(LOG_NAME, "|-----------------------------------------|");
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
    Logger::trace(LOG_NAME, "Resetting port %x", port->getAddress());

    port->outw(port->inw() | REG_PORTSC_CSC | REG_PORTSC_PEC);

    port->outw(port->inw() | REG_PORTSC_PR);
    timeService->msleep(100);
    port->outw( port->inw() & ~REG_PORTSC_PR);

    timeService->msleep(10);

    if ( port->inw() & REG_PORTSC_CCS ) {
        Logger::trace(LOG_NAME, " -> Device detected");
    }
}

void Uhci::plugin() {
    Logger::trace(LOG_NAME, "Assigning interrupt %d", pciDevice.intr);

    IntDispatcher::getInstance().assign((uint8_t ) pciDevice.intr + 32, *this);
    Pic::getInstance()->allow(pciDevice.intr);
}

void Uhci::trigger() {
    Logger::trace(LOG_NAME, "INTERRUPT");
}

void Uhci::reset() {
    Logger::trace(LOG_NAME, "Resetting Host Controller");
    uint16_t cmd = io.command->inw();
    io.command->outw(cmd |  REG_USBCMD_HCRESET);

    while ( io.command->inw() & REG_USBCMD_HCRESET );
    Logger::trace(LOG_NAME, "Host Controller reset");

    disableInterrupts();
    stop();
}

void Uhci::start() {
    Logger::trace(LOG_NAME, "Starting Host Controller");
    uint16_t cmd = io.command->inw();
    io.command->outw(cmd | REG_USBCMD_RS);

    while ( (io.status->inw() & REG_USBSTS_HCH) );
    Logger::trace(LOG_NAME, "Host Controller started");
}

void Uhci::stop() {
    Logger::trace(LOG_NAME, "Stopping Host Controller");
    uint16_t cmd = io.command->inw();
    io.command->outw(cmd &  ~REG_USBCMD_RS);

    while ( !(io.status->inw() & REG_USBSTS_HCH) );
    Logger::trace(LOG_NAME, "Host Controller stopped");
}

void Uhci::disableLegacySupport() {
    Pci::writeWord(pciDevice.bus, pciDevice.device, pciDevice.function, CAP_LEGSUP, 0x8F00);
    Logger::trace(LOG_NAME, "Disabled Legacy Support");
}
