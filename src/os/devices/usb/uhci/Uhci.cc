/*****************************************************************************
 *                                                                           *
 *                                 U H C I                                   *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Universal Host Controller Interface                      *
 *                                                                           *
 *                                                                           *
 * Autor:           Filip Krakowski, 12.11.2017                              *
 *****************************************************************************/

#include "Uhci.h"
#include <kernel/interrupts/IntDispatcher.h>
#include <kernel/interrupts/Pic.h>

extern "C" {
    #include "lib/libc/stdlib.h"
}

Uhci::Uhci() {}

void Uhci::setup(const Pci::Device &dev) {

    pciDevice = dev;

    timeService = (TimeService*) Kernel::getService(TimeService::SERVICE_NAME);

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

    plugin();

    //globalReset();

    reset();

    configure();

    start();

    io.command->outw(io.command->inw() | REG_USBCMD_CF);

    enablePorts();

    printSummary();
}

void Uhci::printSummary() {
    UHCI_TRACE("|-----------------------------------------|\n");
    UHCI_TRACE("|               UHCI                      |\n");
    UHCI_TRACE("|-----------------------------------------|\n");
    UHCI_TRACE("   BASE:                         %x\n", io.command->getAddress());
    UHCI_TRACE("   LEGACY SUPPORT:               %x\n", pci.readWord(pciDevice->bus, pciDevice->device, pciDevice->function, CAP_LEGSUP));
    UHCI_TRACE("   STATUS:                       %x\n", io.status->inw());
    UHCI_TRACE("   COMMAND:                      %x\n", io.command->inw());
    UHCI_TRACE("   INTERRUPT:                    %x\n", io.interrupt->inw());
    UHCI_TRACE("   FRNUM:                        %x\n", io.frameNumber->inw());
    UHCI_TRACE("   FRBASEADDR:                   %x\n", io.frameBaseAddress->indw());
    UHCI_TRACE("   SOFMOD:                       %x\n", io.sofModify->inb());
    UHCI_TRACE("   PORT1 (%x)                    %x\n", io.port1->getAddress(), io.port1->inw());
    UHCI_TRACE("   PORT2 (%x)                    %x\n", io.port2->getAddress(), io.port2->inw());
    UHCI_TRACE("   INTERRUPT LINE:               %d\n", pciDevice->intr);
    UHCI_TRACE("|-----------------------------------------|\n");
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
    UHCI_TRACE("Resetting port %x\n", port->getAddress());

    port->outw(port->inw() | REG_PORTSC_CSC | REG_PORTSC_PEC);

    port->outw(port->inw() | REG_PORTSC_PR);
    timeService->msleep(100);
    port->outw( port->inw() & ~REG_PORTSC_PR);

    timeService->msleep(10);

    if ( port->inw() & REG_PORTSC_CCS ) {
        UHCI_TRACE(" -> Device detected\n");
    }
}

void Uhci::plugin() {
    UHCI_TRACE("Assigning interrupt %d\n", pciDevice->intr);

    IntDispatcher::assign(pciDevice.intr + 32, *this);
    Pic::getInstance()->allow(pciDevice.intr);
}

void Uhci::trigger() {
    UHCI_TRACE("INTERRUPT");
}

void Uhci::reset() {
    UHCI_TRACE("Resetting Host Controller\n");
    uint16_t cmd = io.command->inw();
    io.command->outw(cmd |  REG_USBCMD_HCRESET);

    while ( io.command->inw() & REG_USBCMD_HCRESET );
    UHCI_TRACE("Host Controller reset\n");

    disableInterrupts();
    stop();
}

void Uhci::start() {
    UHCI_TRACE("Starting Host Controller\n");
    uint16_t cmd = io.command->inw();
    io.command->outw(cmd | REG_USBCMD_RS);

    while ( (io.status->inw() & REG_USBSTS_HCH) );
    UHCI_TRACE("Host Controller started\n");
}

void Uhci::stop() {
    UHCI_TRACE("Stopping Host Controller\n");
    uint16_t cmd = io.command->inw();
    io.command->outw(cmd &  ~REG_USBCMD_RS);

    while ( !(io.status->inw() & REG_USBSTS_HCH) );
    UHCI_TRACE("Host Controller stopped\n");
}

void Uhci::disableLegacySupport() {
    Pci::writeWord(pciDevice.bus, pciDevice.device, pciDevice.function, CAP_LEGSUP, 0x8F00);
    UHCI_TRACE("Disabled Legacy Support\n");
}
