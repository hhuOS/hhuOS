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

#include <devices/Pci.h>
#include <kernel/services/DebugService.h>
#include <kernel/interrupts/IntDispatcher.h>
#include <kernel/services/InputService.h>
#include <kernel/Kernel.h>
#include <kernel/log/Logger.h>
#include "Ehci.h"

void waitOnEnter() {
    Keyboard *kb = Kernel::getService<InputService>()->getKeyboard();

    while (!kb->isKeyPressed(28));
}

extern "C" {
#include "lib/libc/stdlib.h"
}

Logger &Ehci::log = Logger::get("EHCI");

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
    log.trace("|--------------------------------------------------------------|");
    log.trace("|                         EHCI                                 |");
    log.trace("|--------------------------------------------------------------|");
    log.trace("   COMMAND (%x):                 %x", &op->command, op->command);
    log.trace("   STATUS (%x):                  %x", &op->status, op->status);
    log.trace("   INTERRUPT (%x):               %x", &op->interrupt, op->interrupt);
    log.trace("   FRINDEX (%x):                 %x", &op->frameIndex, op->frameIndex);
    log.trace("   CTRLDSSEGMENT (%x):           %x", &op->ctrlDsSegment, op->ctrlDsSegment);
    log.trace("   PERIODICLISTBASE (%x):        %x", &op->periodicListBase, op->periodicListBase);
    log.trace("   ASYNCLISTADDR (%x):           %x", &op->asyncListAddress, op->asyncListAddress);
    log.trace("   CONFIGFLAG (%x):              %x", &op->configFlag, op->configFlag);
    log.trace("|--------------------------------------------------------------|");
    log.trace("|                         PORTS                                |");
    log.trace("|--------------------------------------------------------------|");
    for (uint8_t port = 0; port < numPorts; port++) {
        log.trace("   PORT%d (%x):              %x", port + 1, &op->ports[port], op->ports[port]);
    }
    log.trace("|--------------------------------------------------------------|");
}


Ehci::EhciStatus Ehci::stop() {

    log.trace("Stopping Host Controller");

    op->command &= ~USBCMD_RS;
    op->command &= ~USBCMD_PSE;
    op->command &= ~USBCMD_ASE;

    uint8_t timeout = 20;
    while ( !(op->status & USBSTS_HCH) ) {
        timeService->msleep(10);
        timeout--;

        if ( timeout == 0) {
            log.trace("ERROR: HC reset timed out");
            return TIMEOUT;
        }
    }

    log.trace("Successfully stopped Host Controller");

    return OK;
}


Ehci::EhciStatus Ehci::reset() {

    if (stop() != OK) {
        log.trace("WARNING: Couldn't stop Host Controller");
    }

    log.trace("Resetting Host Controller");

    op->command |= USBCMD_HCRESET;

    uint8_t timeout = 20;
    while ( op->command & USBCMD_HCRESET ) {
        timeService->msleep(10);
        timeout--;

        if ( timeout == 0) {
            log.trace("Error: HC reset timed out");
            return TIMEOUT;
        }
    }

    log.trace("Successfully reset Host Controller");

    return OK;
}

void Ehci::setupPeriodicSchedule() {
    frameList = (PeriodicFrameList*) aligned_alloc(4096, 4 * frameListEntries);

    log.trace("Setting up Periodic Frame List with %d entries", frameListEntries);

    for (uint32_t i = 0; i < frameListEntries; i++) {
        frameList->entries[i] = ENABLED;
    }

    op->command = (op->command & ~(bitMask(2) << 2)) | ((frameListSize & bitMask(2)) << 2);
    op->periodicListBase = (uint32_t) frameList;

    log.trace("Periodic Frame List base at %x", op->periodicListBase);
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
            log.trace("Error: Async Schedule couldn't be enabled");
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
            log.trace("Error: Periodic Schedule couldn't be enabled");
        }
    }
}


void Ehci::handoff() {

    uint8_t eecp = (uint8_t) ((cap->hccParams & HCCPARAMS_EECP) >> 8);
    uint8_t capId = 0;

    log.trace("Extended Capabilities Pointer at %x", eecp);

    if ( eecp >= 0x40) {

        while ( eecp != 0x0 ) {
            capId = Pci::readByte(pciDevice.bus, pciDevice.device, pciDevice.function, eecp);

            if ( capId == 0x1 ) {
                break;
            }

            eecp = Pci::readByte(pciDevice.bus, pciDevice.device, pciDevice.function, eecp + 1);
        }

        if ( capId != 0x1 ) {
            log.trace("EHCI does not support BIOS handoff");
        }

        uint8_t bos   = eecp + 2;
        uint8_t oos   = eecp + 3;
        uint8_t legctlsts = eecp + 4;

        log.trace("Performing EHCI BIOS handoff");

        Pci::writeByte(pciDevice.bus, pciDevice.device, pciDevice.function, oos, ENABLED);

        timeService->msleep(50);

        uint8_t timeout = 250;
        while ( Pci::readByte(pciDevice.bus, pciDevice.device, pciDevice.function, bos) & ENABLED) {
            timeService->msleep(10);
            timeout--;

            if ( timeout == 0 ) {
                log.trace("Error: EHCI handoff timed out");
                break;
            }
        }

        Pci::writeDoubleWord(pciDevice.bus, pciDevice.device, pciDevice.function, legctlsts, 0x0);

        timeService->msleep(50);

        if (Pci::readByte(pciDevice.bus, pciDevice.device, pciDevice.function, bos) & ENABLED) {
            log.trace("WARNING: BIOS still owns semaphore");
        } else {
            log.trace("EHCI BIOS handoff succeeded");
        }

    } else {
        log.trace("EHCI does not support BIOS handoff");
    }
}


void Ehci::start() {

    op->ctrlDsSegment = 0;

    acknowledgeAll();

    setupPeriodicSchedule();
    setupAsyncSchedule();

    op->command = (op->command & ~USBCMD_ITC) | (0x08 << 16);

    if ( op->status & USBSTS_HCH ) {
        log.trace("Starting HC");
        op->command |= USBCMD_RS;
    }

    uint8_t timeout = 20;
    while ( op->status & USBSTS_HCH ) {
        timeService->msleep(10);
        timeout--;

        if ( timeout == 0) {
            log.trace("Error: HC start timed out");
            break;
        }
    }

    op->configFlag = 0x1;
}


void Ehci::startPorts() {

    log.trace("Starting ports");

#if ALLOW_USB_EXCHANGE
    printf("");
    printf("          You can now safely replace the USB thumb drive. Please press ENTER to continue");
    waitOnEnter();
#endif

    uint8_t lineStatus;
    for (uint8_t i = 0; i < numPorts; i++) {

        resetPort(i);

        lineStatus = (uint8_t) (((op->ports[i] & PORTSC_LS) >> 10) & 0x2);
        if (lineStatus != PORTSC_LS_SE0) {
            log.trace("Skipping non-highspeed device on port %d - LS=%02b", i + 1, lineStatus);
            continue;
        }

        if (op->ports[i] & PORTSC_CCS) {
            setupUsbDevice(i);
        }
    }
}


void Ehci::resetPort(uint8_t portNumber) {

    if (op->status & USBSTS_HCH) {
        log.trace("Error: HC is halted");
        return;
    }

    op->ports[portNumber] |= PORTSC_PP;
    op->ports[portNumber] &= ~PORTSC_PE;

    op->status |= USBSTS_PCD;

    log.trace("Resetting Port %d", portNumber + 1);

    op->ports[portNumber] |= PORTSC_PR;
    timeService->msleep(100);
    op->ports[portNumber] &= ~PORTSC_PR;

    uint8_t timeout = 20;
    while ( op->ports[portNumber] & PORTSC_PR ) {
        timeService->msleep(10);
        timeout--;

        if ( timeout == 0) {
            log.trace("WARNING: Port Reset timed out");
            break;
        }
    }

    for (timeout = 0; timeout < 10; timeout++) {
        timeService->msleep(10);

        if ( op->ports[portNumber] & PORTSC_CCS || op->ports[portNumber] & PORTSC_PE ) {
            log.trace(" -> Device detected or Port enabled");
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

    log.trace("Enabling Interrupts");

    acknowledgeAll();

    op->interrupt |= (USBINTR_FLR | USBINTR_HSE | USBINTR_IAA | USBINTR_PCD | USBINTR_USBEINT | USBINTR_USBINT);
}


void Ehci::disableInterrupts() {

    log.trace("Disabling Interrupts");

    op->interrupt &= ~(USBINTR_FLR | USBINTR_HSE | USBINTR_IAA | USBINTR_PCD | USBINTR_USBEINT | USBINTR_USBINT);
}

/**
 * Acknowledges all interrupts.
 */
void Ehci::acknowledgeAll() {
    op->status |= (USBSTS_USBINT | USBSTS_USBEINT | USBSTS_PCD | USBSTS_PCD | USBSTS_FLR | USBSTS_IAA);
}


void Ehci::setupUsbDevice(uint8_t portNumber) {
    log.trace("Setting up USB Mass Storage Device");

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
    log.trace("Assigning interrupt %d", pciDevice.intr);

    IntDispatcher::getInstance().assign((uint8_t) pciDevice.intr + 32, *this);
    Pic::getInstance()->allow(pciDevice.intr);
}

void Ehci::trigger(InterruptFrame &frame) {

    if (op->status & USBSTS_PCD) {
        eventBuffer.push(UsbEvent(UsbEvent::SUBTYPE_PORT_CHANGE));
        eventBus->publish(eventBuffer.pop());
    }

    acknowledgeAll();
}

void Ehci::printPciStatus() {

    log.trace("  PCI STATUS: %x", Pci::readDoubleWord(pciDevice.bus, pciDevice.device, pciDevice.function, Pci::PCI_HEADER_COMMAND));

}

void Ehci::printQueueHead(AsyncListQueue::QueueHead *queueHead) {
    log.trace("|-------------------------------------------------------------|");
    log.trace("|                   QUEUEHEAD(%08x)                     |", queueHead);
    log.trace("|-------------------------------------------------------------|");
    log.trace("| NEXT                        %08x                      |", queueHead->link & ~0x1F);
    log.trace("| ENDPOINT STATE 0            %08x                      |", queueHead->endpointState[0]);
    log.trace("| ENDPOINT STATE 1            %08x                      |", queueHead->endpointState[1]);
    log.trace("|-------------------------------------------------------------|");
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
