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

#include <devices/Pci.h>
#include <kernel/services/DebugService.h>
#include <kernel/interrupts/IntDispatcher.h>
#include <kernel/services/InputService.h>
#include <kernel/Kernel.h>
#include "Ehci.h"

void waitOnEnter() {
    Keyboard *kb = Kernel::getService<InputService>()->getKeyboard();

    while (!kb->isKeyPressed(28));
}

extern "C" {
#include "lib/libc/stdlib.h"
}

Ehci::Ehci() : eventBuffer(1024) {}


void Ehci::setup(const Pci::Device &dev) {

    eventBus = Kernel::getService<EventBus>();
    timeService = Kernel::getService<TimeService>();

    pciDevice = dev;

    Pci::enableBusMaster(pciDevice.bus, pciDevice.device, pciDevice.function);
    Pci::enableMemorySpace(pciDevice.bus, pciDevice.device, pciDevice.function);

    uint32_t base = Pci::readDoubleWord(pciDevice.bus, pciDevice.device,
                                        pciDevice.function, Pci::PCI_HEADER_BAR0) & 0xFFFFFF00;

    cap = (HostCap*) base;
    op = (HostOp*) (base + cap->length);

#if DEBUG_BIOS_QH

    DebugService *debugService = Kernel::getService<DebugService>();

    AsyncListQueue::QueueHead *queueHead = (AsyncListQueue::QueueHead *) op->asyncListAddress;

    while(1) {
        debugService->dumpMemory((uint32_t) queueHead, 2);
        debugService->dumpMemory(queueHead->currentQTD, 2);
        debugService->dumpMemory(((AsyncListQueue::TransferDescriptor*)queueHead->currentQTD)->buffer0, 2);
        debugService->dumpMemory(queueHead->overlay.nextQTD, 2);
        debugService->dumpMemory(((AsyncListQueue::TransferDescriptor*)queueHead->overlay.nextQTD)->buffer0, 2);
        waitOnEnter();
        queueHead = (AsyncListQueue::QueueHead*) (queueHead->link & ~0x1F);
    }

#endif

    handoff();

    readConfig();

    disableInterrupts();

    // plugin();

    reset();

    start();

    enablePeriodicSchedule();
    enableAsyncSchedule();

    startPorts();

    eventBus->subscribe(*this, UsbEvent::TYPE);

    enableInterrupts();
}


void Ehci::readConfig() {
    numPorts = (uint8_t) (cap->hcsParams & HCSPARAMS_NP);
    version = cap->version;
    frameListSize = (uint8_t) (((op->command & USBCMD_FLS) >> 2) & 0b11);
    frameListEntries = (uint16_t) (4096 / (1 << frameListSize));
}


void Ehci::printSummary() {
    EHCI_TRACE("|--------------------------------------------------------------|\n");
    EHCI_TRACE("|                         EHCI                                 |\n");
    EHCI_TRACE("|--------------------------------------------------------------|\n");
    EHCI_TRACE("   COMMAND (%x):                 %x\n", &op->command, op->command);
    EHCI_TRACE("   STATUS (%x):                  %x\n", &op->status, op->status);
    EHCI_TRACE("   INTERRUPT (%x):               %x\n", &op->interrupt, op->interrupt);
    EHCI_TRACE("   FRINDEX (%x):                 %x\n", &op->frameIndex, op->frameIndex);
    EHCI_TRACE("   CTRLDSSEGMENT (%x):           %x\n", &op->ctrlDsSegment, op->ctrlDsSegment);
    EHCI_TRACE("   PERIODICLISTBASE (%x):        %x\n", &op->periodicListBase, op->periodicListBase);
    EHCI_TRACE("   ASYNCLISTADDR (%x):           %x\n", &op->asyncListAddress, op->asyncListAddress);
    EHCI_TRACE("   CONFIGFLAG (%x):              %x\n", &op->configFlag, op->configFlag);
    EHCI_TRACE("|--------------------------------------------------------------|\n");
    EHCI_TRACE("|                         PORTS                                |\n");
    EHCI_TRACE("|--------------------------------------------------------------|\n");
    for (uint8_t port = 0; port < numPorts; port++) {
        EHCI_TRACE("   PORT%d (%x):              %x\n", port + 1, &op->ports[port], op->ports[port]);
    }
    EHCI_TRACE("|--------------------------------------------------------------|\n");
}


Ehci::EhciStatus Ehci::stop() {

    EHCI_TRACE("Stopping Host Controller\n");

    op->command &= ~USBCMD_RS;
    op->command &= ~USBCMD_PSE;
    op->command &= ~USBCMD_ASE;

    uint8_t timeout = 20;
    while ( !(op->status & USBSTS_HCH) ) {
        timeService->msleep(10);
        timeout--;

        if ( timeout == 0) {
            EHCI_TRACE("ERROR: HC reset timed out\n");
            return TIMEOUT;
        }
    }

    EHCI_TRACE("Successfully stopped Host Controller\n");

    return OK;
}


Ehci::EhciStatus Ehci::reset() {

    if (stop() != OK) {
        EHCI_TRACE("WARNING: Couldn't stop Host Controller\n");
    }

    EHCI_TRACE("Resetting Host Controller\n");

    op->command |= USBCMD_HCRESET;

    uint8_t timeout = 20;
    while ( op->command & USBCMD_HCRESET ) {
        timeService->msleep(10);
        timeout--;

        if ( timeout == 0) {
            EHCI_TRACE("Error: HC reset timed out\n");
            return TIMEOUT;
        }
    }

    EHCI_TRACE("Successfully reset Host Controller\n");

    return OK;
}

void Ehci::setupPeriodicSchedule() {
    frameList = (PeriodicFrameList*) aligned_alloc(4096, 4 * frameListEntries);

    EHCI_TRACE("Setting up Periodic Frame List with %d entries\n", frameListEntries);

    for (uint32_t i = 0; i < frameListEntries; i++) {
        frameList->entries[i] = ENABLED;
    }

    op->command = (op->command & ~(bitMask(2) << 2)) | ((frameListSize & bitMask(2)) << 2);
    op->periodicListBase = (uint32_t) frameList;

    EHCI_TRACE("Periodic Frame List base at %x\n", op->periodicListBase);
}


void Ehci::setupAsyncSchedule() {
    asyncListQueue = new AsyncListQueue();
    op->asyncListAddress = (uint32_t) asyncListQueue->getHead();
}


void Ehci::enableAsyncSchedule() {
    op->command |= USBCMD_ASE;

    uint8_t timeout = 20;
    while ( !(op->status & USBSTS_ASS) ) {
        timeService->msleep(10);
        timeout--;

        if ( timeout == 0) {
            EHCI_TRACE("Error: Async Schedule couldn't be enabled\n");
        }
    }
}


void Ehci::enablePeriodicSchedule() {
    op->command |= USBCMD_PSE;

    uint8_t timeout = 20;
    while ( !(op->status & USBSTS_PSS) ) {
        timeService->msleep(10);
        timeout--;

        if ( timeout == 0) {
            EHCI_TRACE("Error: Periodic Schedule couldn't be enabled\n");
        }
    }
}


void Ehci::handoff() {

    uint8_t eecp = (uint8_t) ((cap->hccParams & HCCPARAMS_EECP) >> 8);
    uint8_t capId = 0;

    EHCI_TRACE("Extended Capabilities Pointer at %x\n", eecp);

    if ( eecp >= 0x40) {

        while ( eecp != 0x0 ) {
            capId = Pci::readByte(pciDevice.bus, pciDevice.device, pciDevice.function, eecp);

            if ( capId == 0x1 ) {
                break;
            }

            eecp = Pci::readByte(pciDevice.bus, pciDevice.device, pciDevice.function, eecp + 1);
        }

        if ( capId != 0x1 ) {
            EHCI_TRACE("EHCI does not support BIOS handoff\n");
        }

        uint8_t bos   = eecp + 2;
        uint8_t oos   = eecp + 3;
        uint8_t legctlsts = eecp + 4;

        EHCI_TRACE("Performing EHCI BIOS handoff\n");

        Pci::writeByte(pciDevice.bus, pciDevice.device, pciDevice.function, oos, ENABLED);

        timeService->msleep(50);

        uint8_t timeout = 250;
        while ( Pci::readByte(pciDevice.bus, pciDevice.device, pciDevice.function, bos) & ENABLED) {
            timeService->msleep(10);
            timeout--;

            if ( timeout == 0 ) {
                EHCI_TRACE("Error: EHCI handoff timed out\n");
                break;
            }
        }

        Pci::writeDoubleWord(pciDevice.bus, pciDevice.device, pciDevice.function, legctlsts, 0x0);

        timeService->msleep(50);

        if (Pci::readByte(pciDevice.bus, pciDevice.device, pciDevice.function, bos) & ENABLED) {
            EHCI_TRACE("WARNING: BIOS still owns semaphore\n");
        } else {
            EHCI_TRACE("EHCI BIOS handoff succeeded\n");
        }

    } else {
        EHCI_TRACE("EHCI does not support BIOS handoff\n");
    }
}


void Ehci::start() {

    op->ctrlDsSegment = 0;

    acknowledgeAll();

    setupPeriodicSchedule();
    setupAsyncSchedule();

    op->command = (op->command & ~USBCMD_ITC) | (0x08 << 16);

    if ( op->status & USBSTS_HCH ) {
        EHCI_TRACE("Starting HC\n");
        op->command |= USBCMD_RS;
    }

    uint8_t timeout = 20;
    while ( op->status & USBSTS_HCH ) {
        timeService->msleep(10);
        timeout--;

        if ( timeout == 0) {
            EHCI_TRACE("Error: HC start timed out\n");
            break;
        }
    }

    op->configFlag = 0x1;
}


void Ehci::startPorts() {

    EHCI_TRACE("Starting ports\n");

#if ALLOW_USB_EXCHANGE
    printf("\n\n");
    printf("          You can now safely replace the USB thumb drive. Please press ENTER to continue");
    waitOnEnter();
#endif

    uint8_t lineStatus;
    for (uint8_t i = 0; i < numPorts; i++) {

        resetPort(i);

        lineStatus = (uint8_t) (((op->ports[i] & PORTSC_LS) >> 10) & 0x2);
        if (lineStatus != PORTSC_LS_SE0) {
            EHCI_TRACE("Skipping non-highspeed device on port %d - LS=%02b\n", i + 1, lineStatus);
            continue;
        }

        if (op->ports[i] & PORTSC_CCS) {
            setupUsbDevice(i);
        }
    }
}


void Ehci::resetPort(uint8_t portNumber) {

    if (op->status & USBSTS_HCH) {
        EHCI_TRACE("Error: HC is halted\n");
        return;
    }

    op->ports[portNumber] |= PORTSC_PP;
    op->ports[portNumber] &= ~PORTSC_PE;

    op->status |= USBSTS_PCD;

    EHCI_TRACE("Resetting Port %d\n", portNumber + 1);

    op->ports[portNumber] |= PORTSC_PR;
    timeService->msleep(100);
    op->ports[portNumber] &= ~PORTSC_PR;

    uint8_t timeout = 20;
    while ( op->ports[portNumber] & PORTSC_PR ) {
        timeService->msleep(10);
        timeout--;

        if ( timeout == 0) {
            EHCI_TRACE("WARNING: Port Reset timed out\n");
            break;
        }
    }

    for (timeout = 0; timeout < 10; timeout++) {
        timeService->msleep(10);

        if ( op->ports[portNumber] & PORTSC_CCS || op->ports[portNumber] & PORTSC_PE ) {
            EHCI_TRACE(" -> Device detected or Port enabled\n");
            break;
        }
    }

    if ( op->ports[portNumber] & PORTSC_CSC ) {
        op->ports[portNumber] |= PORTSC_CSC;
    }

    if ( op->ports[portNumber] & PORTSC_PEC ) {
        op->ports[portNumber] |= PORTSC_PEC;
    }
}


void Ehci::enableInterrupts() {

    EHCI_TRACE("Enabling Interrupts\n");

    acknowledgeAll();

    op->interrupt |= (USBINTR_FLR | USBINTR_HSE | USBINTR_IAA | USBINTR_PCD | USBINTR_USBEINT | USBINTR_USBINT);
}


void Ehci::disableInterrupts() {

    EHCI_TRACE("Disabling Interrupts\n");

    op->interrupt &= ~(USBINTR_FLR | USBINTR_HSE | USBINTR_IAA | USBINTR_PCD | USBINTR_USBEINT | USBINTR_USBINT);
}

/**
 * Acknowledges all interrupts.
 */
void Ehci::acknowledgeAll() {
    op->status |= (USBSTS_USBINT | USBSTS_USBEINT | USBSTS_PCD | USBSTS_PCD | USBSTS_FLR | USBSTS_IAA);
}


void Ehci::setupUsbDevice(uint8_t portNumber) {
    EHCI_TRACE("Setting up USB Mass Storage Device\n");

    AsyncListQueue::QueueHead *control = AsyncListQueue::createQueueHead(false, 0, 0, 64, 0x1, 0x2, true);

    asyncListQueue->insertQueueHead(control);

    UsbMassStorage *device = new UsbMassStorage(control, portNumber);

    massStorageDevices.add(device);
}


uint32_t Ehci::getNumDevices() {
    return massStorageDevices.length();
}


UsbMassStorage *Ehci::getDevice(uint32_t index) {
    return massStorageDevices.get(index);
}

void Ehci::plugin() {
    EHCI_TRACE("Assigning interrupt %d\n", pciDevice.intr);

    IntDispatcher::getInstance().assign((uint8_t) pciDevice.intr + 32, *this);
    Pic::getInstance()->allow(pciDevice.intr);
}

void Ehci::trigger() {

    if (op->status & USBSTS_PCD) {
        eventBuffer.push(UsbEvent(UsbEvent::SUBTYPE_PORT_CHANGE));
        eventBus->publish(eventBuffer.pop());
    }

    acknowledgeAll();
}

void Ehci::printPciStatus() {

    EHCI_TRACE("  PCI STATUS: %x\n", pci.readDoubleWord(pciDevice.bus, pciDevice.device, pciDevice.function, Pci::PCI_HEADER_COMMAND));

}

void Ehci::printQueueHead(AsyncListQueue::QueueHead *queueHead) {
    EHCI_TRACE("|-------------------------------------------------------------|\n");
    EHCI_TRACE("|                   QUEUEHEAD(%08x)                     |\n", queueHead);
    EHCI_TRACE("|-------------------------------------------------------------|\n");
    EHCI_TRACE("| NEXT                        %08x                      |\n", queueHead->link & ~0x1F);
    EHCI_TRACE("| ENDPOINT STATE 0            %08x                      |\n", queueHead->endpointState[0]);
    EHCI_TRACE("| ENDPOINT STATE 1            %08x                      |\n", queueHead->endpointState[1]);
    EHCI_TRACE("|-------------------------------------------------------------|\n");
}

void Ehci::onEvent(const Event &event) {

    if (event.getType() != UsbEvent::TYPE) {
        return;
    }

    UsbEvent &usbEvent = (UsbEvent&) event;

    switch (usbEvent.getSubtype()) {
        case UsbEvent::SUBTYPE_PORT_CHANGE:
            onPortChangeDetected();
            break;
    }

}

void Ehci::onPortChangeDetected() {
    for (uint8_t i = 0; i < numPorts; i++) {
        if (op->ports[i] & PORTSC_CSC) {
            if (op->ports[i] & PORTSC_CCS) {
                resetPort(i);
                setupUsbDevice(i);
            } else {
                // TODO(krakowski):
                //  Implement removing usb devices
            }

            op->ports[i] |= PORTSC_CSC;
        }
    }
}
