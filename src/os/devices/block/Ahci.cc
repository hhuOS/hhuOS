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

#include <kernel/Kernel.h>
#include <kernel/log/Logger.h>
#include <devices/block/storage/AhciDevice.h>
#include "devices/block/Ahci.h"

#include "../../kernel/memory/SystemManagement.h"

Logger &Ahci::log = Logger::get("AHCI");
uint32_t Ahci::deviceCount = 0;

Ahci::Ahci() {
    timeService = Kernel::getService<TimeService>();
}

void Ahci::setup(const Pci::Device &dev) {

    pciDevice = dev;
    Pci::enableBusMaster(pciDevice.bus, pciDevice.device, pciDevice.function);
    Pci::enableIoSpace(pciDevice.bus, pciDevice.device, pciDevice.function);
    Pci::enableMemorySpace(pciDevice.bus, pciDevice.device, pciDevice.function);

    uint32_t tmpAhciBase = Pci::readDoubleWord(pciDevice.bus, pciDevice.device, pciDevice.function, Pci::PCI_HEADER_BAR5);

    IOMemInfo memInfo = SystemManagement::getInstance()->mapIO(tmpAhciBase & ~0xFu, 4096);
    uint32_t ahciBase = memInfo.virtStartAddress;
    abar = (HbaMem*) (ahciBase);

    bool ret;

    // Enable AHCI mode (if supported)
    enableAhci();

    // Request Bios Handoff (if supported)
    biosHandoff();

    // Read HBA configuration
    readConfig();

    if(numPorts == 0) {
        log.trace("No ports found on HBA");
        log.trace("Aborting AHCI-setup");

        return;
    }

    if(pi == 0) {
        log.trace("No ports are implemented on HBA");
        log.trace("Aborting AHCI-setup");

        return;
    }

    // Scan all ports for devices
    scan();

    // Reset HBA
    ret = reset();

    if(!ret) {
        log.trace("Failed to reset HBA");
        log.trace("Aborting AHCI-setup");

        numDevices = 0;

        return;
    }

    // Enable AHCI mode (if supported)
    enableAhci();

    // Stop all implemented ports
    stopAll();

    // Reset all implemented ports
    //resetAll();

    // Rebase all implemented ports
    rebaseAll();

    // Start all implemented ports
    startAll();

    abar->is = abar->is; // @suppress("Assignment to itself")

    auto *storageService = Kernel::getService<StorageService>();

    for (uint8_t i = 0; i < numDevices; i++) {

        StorageDevice *storageDevice = new AhciDevice(*this, i, "hdd" + String::valueOf(deviceCount, 10));

        storageService->registerDevice(storageDevice);

        deviceCount++;
    }
}

void Ahci::readConfig() {
    numPorts    = static_cast<uint8_t>((abar->cap & ((1u << 5u) - 1)) + 1);
    numCmdSlots = static_cast<uint16_t>(((abar->cap >> 8u) & ((1u << 5u) - 1)) + 1);
    pi          = abar->pi;

    log.trace("numPorts=%d  numCmdSlots=%d  pi=%x ", numPorts, numCmdSlots, pi);

    log.trace("HBA Capabilities = %x ", abar->cap);
}

bool Ahci::reset() {
    log.trace("Resetting Host-Bus-Adapter ");

    abar->ghc |= HBA_GHC_HR;

    uint32_t timeout = 0;
    while ((abar->ghc & HBA_GHC_HR) && timeout < AHCI_TIMEOUT) {
        timeService->msleep(10);
        timeout += 10;
    }

    if(timeout == AHCI_TIMEOUT) {
        log.trace("Error: Timeout while resetting HBA");

        return false;
    }

    return true;

    // uint32_t spin = 0;
    // for (uint32_t i = 0; i < numPorts; i++) {
    //     if ( isPortImplemented(i) ) {
    //         log.trace("Waiting for Phy communication ");

    //         spin = 0;
    //         while( (abar->ports[i].ssts & HBA_PxSSTS_DET) != HBA_PxSSTS_DET_PHY && spin < 1000000) {
    //             spin++;
    //         }

    //         if (spin == 1000000) {
    //             log.trace("Couldn't establish Phy connection on port %d ", i);
    //         } else {
    //             log.trace("Established Phy connection on port %d ", i);
    //         }
    //     }
    // }
}

void Ahci::enableAhci() {
    if ( !(abar->cap & HBA_CAP_SAM) ) {
        log.trace("Setting HBA to AHCI mode ");
        abar->ghc |= HBA_GHC_AE;
    }
}

void Ahci::scan() {

    for (uint8_t i = 0; i < numPorts; i++) {
        if ( !isPortImplemented(i) ) {
            continue;
        }

        log.trace("Checking port %d ", i);

        switch ( checkType(&abar->ports[i]) ) {
            case AHCI_DEV_SATA:

                if (numDevices >= MAX_DEVICES) {
                    continue;
                }

                log.trace("SATA found at port %d ", i);

                sataDevices[numDevices] = &abar->ports[i];

                log.trace("Stored device at %x ", &sataDevices[numDevices], sataDevices[numDevices]->cmd);

                numDevices++;

                break;
            case AHCI_DEV_SATAPI:
               log.trace("SATAPI found at port %d ", i);
                break;
            case AHCI_DEV_SEMB:
               log.trace("SEMB found at port %d ", i);
                break;
            case AHCI_DEV_PM:
               log.trace("SEMB found at port %d ", i);
                break;
            default:
                break;
        }
    }

    log.trace("Finished scanning all implemented ports ");
}

bool Ahci::isActive(HbaPort* port) {
    return (port->cmd & (HBA_PxCMD_ST | HBA_PxCMD_CR | HBA_PxCMD_FRE | HBA_PxCMD_FR)) ==
        (HBA_PxCMD_ST | HBA_PxCMD_CR | HBA_PxCMD_FRE | HBA_PxCMD_FR);
}

bool Ahci::isPortImplemented(uint16_t portNumber) {
    uint16_t port = static_cast<uint16_t>(1) << portNumber;

    return (pi & port) == port;
}

void Ahci::resetPort(HbaPort *port) {

    log.trace("Resetting port... ");

    port->cmd |= HBA_PxCMD_FRE;

    port->sctl = (port->sctl & ~HBA_PxSCTL_IPM) | HBA_PxSCTL_IPM_NO_DEVSLEEP |
        HBA_PxSCTL_IPM_NO_SLUMBER | HBA_PxSCTL_IPM_NO_PARTIAL;

    port->sctl = (port->sctl & ~HBA_PxSCTL_DET) | HBA_PxSCTL_DET_ESTABLISH_COM;
    
    timeService->msleep(10);

    port->sctl = (port->sctl & ~HBA_PxSCTL_DET) | HBA_PxSCTL_DET_NO_DETECTION;

    port->cmd &= ~HBA_PxCMD_FRE;

    port->cmd |= HBA_PxCMD_POD;

    port->cmd |= HBA_PxCMD_SUD;

    if (port->cmd & HBA_PxCMD_CPD) {
        log.trace("Cold Presence Detection is supported ");
        log.trace("Powering up device... ");

        // TODO(krakowski):
        //  Perform power-up
        
    }

    if (abar->cap & HBA_CAP_SSS) {
        log.trace("Staggered Spin-up is supported ");

        // TODO(krakowski):
        //  Perform staggered spin-up
        
    }

    log.trace("Requesting transition to ACTIVE state ");

    port->cmd = (port->cmd & ~HBA_PxCMD_ICC) | HBA_PxCMD_ICC_ACTIVE;

    /*log.trace("Waiting for Phy communication ");

    while( (port->ssts & HBA_PxSSTS_DET) != HBA_PxSSTS_DET_PHY );

    log.trace("Phy communication established ");*/

    port->serr  = port->serr; // @suppress("Assignment to itself")
    port->is    = port->is;   // @suppress("Assignment to itself")
}

void Ahci::resetAll() {
    for (uint8_t i = 0; i < numPorts; i++) {
        if ( isPortImplemented(i) ) {
            resetPort(&abar->ports[i]);
        }
    }
}

void Ahci::rebaseAll() {
    for (uint8_t i = 0; i < numPorts; i++) {
        if ( isPortImplemented(i) ) {
            rebasePort(&abar->ports[i], i);
        }
    }
}

uint8_t Ahci::checkType(HbaPort *port) {

    switch (port->sig) {
        case SATA_SIG_ATAPI:
            return AHCI_DEV_SATAPI;
        case SATA_SIG_SEMB:
            return AHCI_DEV_SEMB;
        case SATA_SIG_PM:
            return AHCI_DEV_PM;
        case SATA_SIG_ATA:
            return AHCI_DEV_SATA;
        default:
            return AHCI_DEV_NULL;
    }
}


uint8_t Ahci::getNumDevices() {
    return numDevices;
}

void Ahci::biosHandoff() {

    if ( !(abar->cap2 & HBA_CAP2_BOH) ) {
       log.trace("AHCI BIOS Handoff is not supported ");
        return;
    }

    log.trace("Performing AHCI BIOS Handoff ");

    abar->bohc = (abar->bohc & ~HBA_BOHC_OOC) | HBA_BOHC_OOS;

    uint32_t timeout = 0;
    while( (abar->bohc & (HBA_BOHC_BOS | HBA_BOHC_OOS)) != HBA_BOHC_OOS && timeout < AHCI_TIMEOUT) {
        timeService->msleep(10);
        timeout += 10;
    }

    if (timeout == AHCI_TIMEOUT) {
       log.trace("BIOS Handoff timed out");
    } else {
       log.trace("AHCI BIOS Handoff succeeded ");
    }
}

bool Ahci::read(uint8_t device, uint32_t startl, uint32_t starth,
                uint16_t count, uint16_t *buf) {

    if (device + 1 > numDevices) {
        return false;
    }

    return ahci_rw(sataDevices[device], startl, starth, count, buf, ATA_CMD_READ_DMA_EX, device);
}


bool Ahci::write(uint8_t device, uint32_t startl, uint32_t starth, uint16_t count, uint16_t *buf) {

    if (device + 1 > numDevices) {
        return false;
    }

    return ahci_rw(sataDevices[device], startl, starth, count, buf, ATA_CMD_WRITE_DMA_EX, device);
}

Ahci::AhciDeviceInfo Ahci::getDeviceInfo(uint16_t deviceNumber) {

    AhciDeviceInfo ret{0};

    HbaPort *port = sataDevices[deviceNumber];

    HbaPort_Virt *virtPort = sataDevices_Virt[deviceNumber];

    if (!isActive(port)) {
       log.trace("Unable to get device info from port %u: Port is not inizialized", port);
        return ret;
    }

    port->is = port->is; // @suppress("Assignment to itself")

    int tmp = findCmdSlot(port);

    if (tmp == -1) {
        return ret;
    }

    auto slot = static_cast<uint32_t>(tmp);

    IOMemInfo ioMemInfo = SystemManagement::getInstance()->mapIO(512);

    auto *cmdheader = (HbaCmdHeader*) virtPort->clb;
    cmdheader += slot;

    cmdheader->cfl = sizeof(FisRegH2D) / sizeof(uint32_t);
    cmdheader->prdtl = 1;
    cmdheader->w = 0;

    auto *cmdtbl = (HbaCmdTbl*)(virtCtbas[deviceNumber][slot]);
    memset(cmdtbl, 0, 256);

    cmdtbl->prdt_entry[0].dba = (uint32_t) ioMemInfo.physAddresses[0];
    cmdtbl->prdt_entry[0].dbau = (uint32_t) 0;
    cmdtbl->prdt_entry[0].dbc = 511;
    cmdtbl->prdt_entry[0].i = 1;

    auto *cmdfis = (FisRegH2D*) (&cmdtbl->cfis);
    
    cmdfis->fis_type = FIS_TYPE_REG_H2D;
    cmdfis->c = 1;
    cmdfis->command = ATA_CMD_IDENTIFY;

    uint32_t timeout = 0;
    while ( (port->tfd & (HBA_PxTFD_STS_BSY | HBA_PxTFD_STS_DRQ)) && timeout < AHCI_TIMEOUT ) {
        timeout += 10;
    }

    if (timeout == AHCI_TIMEOUT) {
        log.trace("Error: Device is not responding ");
        SystemManagement::getInstance()->freeIO(ioMemInfo);
        return ret;
    }

    port->ci = static_cast<uint32_t>(1u << slot);

    timeout = 0;
    while (timeout < AHCI_TIMEOUT) {

        if ((port->ci & (1u << slot)) == 0) {
            break;
        }

        if (port->is & HBA_PxIS_TFES) {
            log.trace("Error: Task File Error! ");
            SystemManagement::getInstance()->freeIO(ioMemInfo);
            return ret;
        }

        timeService->msleep(10);
        timeout += 10;
    }

    auto *buffer = (uint8_t*) ioMemInfo.virtStartAddress;

    for(uint32_t k = 0; k < 40; k += 2) {
        ret.name[k] = buffer[54 + k + 1];
        ret.name[k + 1] = buffer[54 + k];
    }

    ret.name[40] = 0;

    uint32_t commandsets = *((uint32_t *)(buffer + ATA_IDENT_COMMANDSETS));

    if (commandsets & (1u << 26u))
        // Device uses 48-Bit Addressing:
        ret.sectorCount = *((uint32_t *)(buffer + ATA_IDENT_MAX_LBA_EXT));
    else
        // Device uses CHS or 28-bit Addressing:
        ret.sectorCount = *((uint32_t *)(buffer + ATA_IDENT_MAX_LBA));

    SystemManagement::getInstance()->freeIO(ioMemInfo);

    return ret;
}

bool Ahci::ahci_rw(HbaPort *port, uint32_t startl, uint32_t starth, uint16_t count, uint16_t *buf, uint8_t command,
                   uint8_t portIndex) {

    if (count == 0) {
        log.trace("Error: Count has to be greater than 0 ");
        return false;
    }

    if ( !isActive(port) ) {
        log.trace("Error: Port is not inizialized ");
        return false;
    }

    uint32_t totalCount = count;

    HbaPort_Virt *virtPort = sataDevices_Virt[portIndex];

    port->is = port->is; // @suppress("Assignment to itself")

    int tmp = findCmdSlot(port);

    if (tmp == -1) {
        return false;
    }

    auto slot = static_cast<uint32_t>(tmp);

    IOMemInfo ioMemInfo = SystemManagement::getInstance()->mapIO(static_cast<uint32_t>(count * 512 * 2));

    if(command == ATA_CMD_WRITE_DMA_EX) {
        memcpy((void *) ioMemInfo.virtStartAddress, buf, static_cast<size_t>(count * 512));
    }

    auto *cmdheader = (HbaCmdHeader*) virtPort->clb;
    cmdheader += slot;
    cmdheader->cfl = sizeof(FisRegH2D) / sizeof(uint32_t);
    cmdheader->w = static_cast<uint8_t>(command == ATA_CMD_READ_DMA_EX ? 0 : 1);

    cmdheader->prdtl = static_cast<uint16_t>(count % 8 == 0 ? count / 8 : count / 8 + 1);

    auto *cmdtbl = (HbaCmdTbl*)(virtCtbas[portIndex][slot]);
    memset(cmdtbl, 0, 256);

    int i = 0;
    for (; i < cmdheader->prdtl - 1 ; i++) {
        uint32_t physBuf = ioMemInfo.physAddresses[i];
        cmdtbl->prdt_entry[i].dba = (uint32_t) physBuf;
        cmdtbl->prdt_entry[i].dbc = (4 * 1024) - 1;
        cmdtbl->prdt_entry[i].i = 0;
        count -= 8;
    }

    uint32_t physBuf = ioMemInfo.physAddresses[i];
    cmdtbl->prdt_entry[i].dba = (uint32_t) physBuf;
    cmdtbl->prdt_entry[i].dbc = static_cast<uint32_t>((count << 9u) - 1);
    cmdtbl->prdt_entry[i].i = 0;

    auto *cmdfis = (FisRegH2D*) (&cmdtbl->cfis);

    cmdfis->fis_type = FIS_TYPE_REG_H2D;
    cmdfis->c = 1;
    cmdfis->command = command;

    cmdfis->lba0 = (uint8_t) (startl);
    cmdfis->lba1 = (uint8_t) (startl >> 8u);
    cmdfis->lba2 = (uint8_t) (startl >> 16u);

    cmdfis->device = 1 << 6;

    cmdfis->lba3 = (uint8_t) (startl >> 24u);
    cmdfis->lba4 = (uint8_t) (starth);
    cmdfis->lba5 = (uint8_t) (starth >> 8u);

    cmdfis->countl = static_cast<uint8_t>(totalCount & 0xFFu);
    cmdfis->counth = static_cast<uint8_t>((totalCount >> 8u) & 0xFFu);

    cmdfis->featurel = 1;

    uint32_t timeout = 0;
    while ( (port->tfd & (HBA_PxTFD_STS_BSY | HBA_PxTFD_STS_DRQ)) && timeout < AHCI_TIMEOUT ) {
        timeService->msleep(10);
        timeout += 10;
    }

    if (timeout == AHCI_TIMEOUT) {
       log.trace("Error: Device is not responding ");
        SystemManagement::getInstance()->freeIO(ioMemInfo);
        return false;
    }

    port->ci = 1u << slot;

    timeout = 0;
    while (timeout < AHCI_TIMEOUT) {

        if ((port->ci & (1u << slot)) == 0) {
            break;
        }

        if (port->is & HBA_PxIS_TFES) {
            log.trace("Error: Task File Error! ");
            SystemManagement::getInstance()->freeIO(ioMemInfo);
            return false;
        }

        timeService->msleep(10);
        timeout += 10;
    }

    if (timeout == AHCI_TIMEOUT) {
        log.trace("Error: Device is hung ");
        SystemManagement::getInstance()->freeIO(ioMemInfo);
        return false;
    }

    if (port->is & HBA_PxIS_TFES || port->tfd & HBA_PxTFD_ERR) {
        log.trace("Task File Error! ");
        SystemManagement::getInstance()->freeIO(ioMemInfo);
        return false;
    }

    memcpy((void*)buf, (void*)(ioMemInfo.virtStartAddress), totalCount * 512);
    SystemManagement::getInstance()->freeIO(ioMemInfo);

    return true;
}

int Ahci::findCmdSlot(HbaPort *port) {
    uint32_t slots = (port->sact | port->ci);

    for (uint16_t i = 0; i < numCmdSlots; i++)	{
        if ((slots & (1u << i)) == 0) {
            return i;
        }
    }

    return -1;
}

void Ahci::rebasePort(HbaPort *port, int portno) {

    log.trace("Rebasing port %d ", portno);

    IOMemInfo tmp = SystemManagement::getInstance()->mapIO(4096);

    uint32_t clb = tmp.virtStartAddress;
    memset((void*) clb, 0, 4096);

    HbaPort_Virt* portVirt = new HbaPort_Virt;
    sataDevices_Virt[portno] = portVirt;
    portVirt->clb = tmp.virtStartAddress;

	port->clb = tmp.physAddresses[0];
	port->clbu = 0;
	
    log.trace("Rebased Command List to %x ", port->clb);

    uint32_t fb = port->clb + 1024;
    portVirt->fb = tmp.virtStartAddress + 1024;
	port->fb = fb;
	port->fbu = 0;
	
    log.trace("Rebased FIS area to %x ", port->fb);

    tmp = SystemManagement::getInstance()->mapIO(8192);
    memset((void*) tmp.virtStartAddress, 0, 8192);

	auto *cmdheader = (HbaCmdHeader*) (portVirt->clb);

	for (uint32_t i = 0; i < MAX_CMD_SLOTS; i++) {
        virtCtbas[portno][i] = (uint32_t)tmp.virtStartAddress + i * 256;

        uint32_t physAddr = (i < 16) ? tmp.physAddresses[0] : tmp.physAddresses[1];
        uint32_t ctba = physAddr + i * 256;

        cmdheader[i].ctba = ctba;
		cmdheader[i].ctbau = 0;
		cmdheader[i].prdtl = 16;
    }
    
    log.trace("Rebased %d Command Headers ( %x - %x ) ",
        MAX_CMD_SLOTS,
        cmdheader[0].ctba,
        cmdheader[MAX_CMD_SLOTS - 1].ctba);    

    port->serr = port->serr; // @suppress("Assignment to itself")
    port->is = port->is;     // @suppress("Assignment to itself")
}

void Ahci::startAll() {
    for (uint8_t i = 0; i <= numPorts; i++) {
        if ( isPortImplemented(i) ) {
            log.trace("Starting port %d ", i);
            startCommand(&abar->ports[i]);
        }
    }
}

void Ahci::startCommand(HbaPort *port) {
    uint32_t timeout = 0;
    while (port->cmd & (HBA_PxCMD_CR | HBA_PxCMD_FR) && timeout < AHCI_TIMEOUT) {
        timeService->msleep(10);
        timeout += 10;
    }

    if (timeout == AHCI_TIMEOUT) {
        log.trace("Error: Timeout while sending start command");
    } else {
        port->cmd |= HBA_PxCMD_ST | HBA_PxCMD_FRE;
    }
}

void Ahci::stopAll() {
    for (uint8_t i = 0; i < numPorts; i++) {
        if ( isPortImplemented(i) ) {
            log.trace("Stopping port %d ", i);
            stopCommand(&abar->ports[i]);
        }
    }
}


void Ahci::stopCommand(HbaPort *port) {
	port->cmd &= ~(HBA_PxCMD_ST | HBA_PxCMD_FRE);

    uint32_t timeout = 0;
    while (port->cmd & (HBA_PxCMD_FR | HBA_PxCMD_CR) && timeout < AHCI_TIMEOUT) {
        timeService->msleep(10);
        timeout += 10;
    }

    if (timeout == AHCI_TIMEOUT) {
        log.trace("Error: Timeout while sending stop command");
    }
}

void Ahci::plugin() {

}

void Ahci::trigger() {
    // TODO(krakowski):
    //  Wake up waiting threads once i/o-manager is implemented
}
