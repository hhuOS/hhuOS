#include <kernel/Kernel.h>
#include "devices/block/Ahci.h"

#include "../../kernel/memory/SystemManagement.h"

extern "C" {
    #include "lib/libc/string.h"
}

Ahci::Ahci() {

}

void Ahci::setup(const Pci::Device &dev) {

    pciDevice = dev;
    Pci::enableBusMaster(pciDevice.bus, pciDevice.device, pciDevice.function);
    Pci::enableIoSpace(pciDevice.bus, pciDevice.device, pciDevice.function);
    Pci::enableMemorySpace(pciDevice.bus, pciDevice.device, pciDevice.function);

    uint32_t tmpAhciBase = Pci::readDoubleWord(pciDevice.bus, pciDevice.device, pciDevice.function, Pci::PCI_HEADER_BAR5);

    IOMemInfo memInfo = SystemManagement::getInstance()->mapIO(tmpAhciBase, 4096);
    uint32_t ahciBase = memInfo.virtStartAddress;
    abar = (HbaMem*) (ahciBase & ~0xF);

    // Enable AHCI mode (if supported)
    enableAhci();

    // Request Bios Handoff (if supported)
    biosHandoff();

    // Read HBA configuration
    readConfig();

    // Scan all ports for devices
    scan();

    // Reset HBA
    reset();

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

    // Wake up the device
    uint16_t buf[256];
    ahci_rw(sataDevices[0], 0, 0, 1, buf, ATA_CMD_READ_DMA_EX, 0);
    ahci_rw(sataDevices[0], 0, 0, 1, buf, ATA_CMD_READ_DMA_EX, 0);
    ahci_rw(sataDevices[0], 0, 0, 1, buf, ATA_CMD_READ_DMA_EX, 0);

    //identifyDevice(sataDevices[0]);

    abar->is = abar->is; // @suppress("Assignment to itself")
}

void Ahci::readConfig() {
    numPorts    = (abar->cap & ((1 << 5) - 1)) + 1;
    numCmdSlots = ((abar->cap >> 8) & ((1 << 5) - 1)) + 1;
    pi          = abar->pi;

    AHCI_TRACE("numPorts=%d  numCmdSlots=%d  pi=%x\n", numPorts, numCmdSlots, pi);

    AHCI_TRACE("HBA Capabilities = %x\n", abar->cap);
}

void Ahci::reset() {
    AHCI_TRACE("Resetting Host-Bus-Adapter\n");

    abar->ghc |= HBA_GHC_HR;

    while (abar->ghc & HBA_GHC_HR);

    // unsigned int spin = 0;
    // for (uint32_t i = 0; i < numPorts; i++) {
    //     if ( isPortImplemented(i) ) {
    //         AHCI_TRACE("Waiting for Phy communication\n");

    //         spin = 0;
    //         while( (abar->ports[i].ssts & HBA_PxSSTS_DET) != HBA_PxSSTS_DET_PHY && spin < 1000000) {
    //             spin++;
    //         }

    //         if (spin == 1000000) {
    //             AHCI_TRACE("Couldn't establish Phy connection on port %d\n", i);
    //         } else {
    //             AHCI_TRACE("Established Phy connection on port %d\n", i);
    //         }
    //     }
    // }
}

void Ahci::enableAhci() {
    if ( !(abar->cap & HBA_CAP_SAM) ) {
        AHCI_TRACE("Setting HBA to AHCI mode\n");
        abar->ghc |= HBA_GHC_AE;
    }
}

void Ahci::scan() {

    for (uint32_t i = 0; i < numPorts; i++) {
        if ( !isPortImplemented(i) ) {
            continue;
        }

        AHCI_TRACE("Checking port %d\n", i);

        switch ( checkType(&abar->ports[i]) ) {
            case AHCI_DEV_SATA:

                if (numDevices >= MAX_DEVICES) {
                    continue;
                }

                AHCI_TRACE("SATA found at port %d\n", i);

                sataDevices[numDevices] = &abar->ports[i];

                AHCI_TRACE("Stored device at %x\n", &sataDevices[numDevices], sataDevices[numDevices]->cmd);

                numDevices++;

                break;
            case AHCI_DEV_SATAPI:
                AHCI_TRACE("SATAPI found at port %d\n", i);
                break;
            case AHCI_DEV_SEMB:
                AHCI_TRACE("SEMB found at port %d\n", i);
                break;
            case AHCI_DEV_PM:
                AHCI_TRACE("SEMB found at port %d\n", i);
                break;
        }
    }

    AHCI_TRACE("Finished scanning all implemented ports\n");
}

bool Ahci::isActive(HbaPort* port) {
    return (port->cmd & (HBA_PxCMD_ST | HBA_PxCMD_CR | HBA_PxCMD_FRE | HBA_PxCMD_FR)) ==
        (HBA_PxCMD_ST | HBA_PxCMD_CR | HBA_PxCMD_FRE | HBA_PxCMD_FR);
}

bool Ahci::isPortImplemented(uint16_t portNumber) {
    return (pi & (1 << portNumber));
}

void Ahci::resetPort(HbaPort *port) {

    AHCI_TRACE("Resetting port...\n");

    port->cmd |= HBA_PxCMD_FRE;

    port->sctl = (port->sctl & ~HBA_PxSCTL_IPM) | HBA_PxSCTL_IPM_NO_DEVSLEEP |
        HBA_PxSCTL_IPM_NO_SLUMBER | HBA_PxSCTL_IPM_NO_PARTIAL;

    port->sctl = (port->sctl & ~HBA_PxSCTL_DET) | HBA_PxSCTL_DET_ESTABLISH_COM;

    for (int i = 0; i < 10000000; i++);

    port->sctl = (port->sctl & ~HBA_PxSCTL_DET) | HBA_PxSCTL_DET_NO_DETECTION;

    port->cmd &= ~HBA_PxCMD_FRE;

    port->cmd |= HBA_PxCMD_POD;

    port->cmd |= HBA_PxCMD_SUD;

    if (port->cmd & HBA_PxCMD_CPD) {
        AHCI_TRACE("Cold Presence Detection is supported\n");
        AHCI_TRACE("Powering up device...\n");

        // TODO(krakowski):
        //  Perform power-up
        
    }

    if (abar->cap & HBA_CAP_SSS) {
        AHCI_TRACE("Staggered Spin-up is supported\n");

        // TODO(krakowski):
        //  Perform staggered spin-up
        
    }

    AHCI_TRACE("Requesting transition to ACTIVE state\n");

    port->cmd = (port->cmd & ~HBA_PxCMD_ICC) | HBA_PxCMD_ICC_ACTIVE;

    // AHCI_TRACE("Waiting for Phy communication\n");

    // while( (port->ssts & HBA_PxSSTS_DET) != HBA_PxSSTS_DET_PHY );

    // AHCI_TRACE("Phy communication established\n");

    port->serr  = port->serr; // @suppress("Assignment to itself")
    port->is    = port->is; // @suppress("Assignment to itself")
}

void Ahci::resetAll() {
    for (uint32_t i = 0; i < numPorts; i++) {
        if ( isPortImplemented(i) ) {
            resetPort(&abar->ports[i]);
        }
    }
}

void Ahci::rebaseAll() {
    for (uint32_t i = 0; i < numPorts; i++) {
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
    }

    return AHCI_DEV_NULL;
}


uint8_t Ahci::getNumDevices() {
    return numDevices;
}

void Ahci::biosHandoff() {

    if ( !(abar->cap2 && HBA_CAP2_BOH) ) {
        AHCI_TRACE("AHCI BIOS Handoff is not supported\n");
        return;
    }

    AHCI_TRACE("Performing AHCI BIOS Handoff\n");

    abar->bohc = (abar->bohc & ~HBA_BOHC_OOC) | HBA_BOHC_OOS;

    unsigned int spin = 0;
    while( (abar->bohc & (HBA_BOHC_BOS | HBA_BOHC_OOS)) != HBA_BOHC_OOS && spin < 1000000) {
        spin++;
    }

    if (spin == 1000000) {
        AHCI_TRACE("BIOS Handoff timed out");
    } else {
        AHCI_TRACE("AHCI BIOS Handoff succeeded\n");
    }
}


bool Ahci::read(uint8_t device, uint32_t startl, uint32_t starth,
    uint32_t count, uint16_t *buf) {

    if (device + 1 > numDevices) {
        return false;
    }

    //AHCI_TRACE("Reading from device %d at %x\n", device, &sataDevices[device]);

    return ahci_rw(sataDevices[device], startl, starth, count, buf, ATA_CMD_READ_DMA_EX, device);
}


bool Ahci::write(uint8_t device, uint32_t startl, uint32_t starth,
    uint32_t count, uint16_t *buf) {

    if (device + 1 > numDevices) {
        return false;
    }

    return ahci_rw(sataDevices[device], startl, starth, count, buf, ATA_CMD_WRITE_DMA_EX, device);
}

void Ahci::identifyDevice(HbaPort *port) {

    uint8_t *buffer = new uint8_t[512];

    if ( !isActive(port) ) {
        AHCI_TRACE("Error: Port is not inizialized\n");
        return;
    }

    port->is = port->is; // @suppress("Assignment to itself")

    int spin = 0;
    int slot = findCmdSlot(port);

    if (slot == -1) {
        return;
    }

    HbaCmdHeader *cmdheader = (HbaCmdHeader*) port->clb;
    cmdheader += slot;

    cmdheader->cfl = sizeof(FisRegH2D) / sizeof(uint32_t);
    cmdheader->prdtl = 1;
    cmdheader->w = 0;
    
    HbaCmdTbl *cmdtbl = (HbaCmdTbl*) (cmdheader->ctba);

    memset(cmdtbl, 0, 256);

    cmdtbl->prdt_entry[0].dba = (uint32_t) buffer;
    cmdtbl->prdt_entry[0].dbau = (uint32_t) 0;
    cmdtbl->prdt_entry[0].dbc = 511;
    cmdtbl->prdt_entry[0].i = 1;
    

    FisRegH2D *cmdfis = (FisRegH2D*) (&cmdtbl->cfis);
    
    cmdfis->fis_type = FIS_TYPE_REG_H2D;
    cmdfis->c = 1;
    cmdfis->command = ATA_CMD_IDENTIFY;

    while ( (port->tfd & (HBA_PxTFD_STS_BSY | HBA_PxTFD_STS_DRQ)) && spin < 1000000 ) {
        spin++;
    }

    if (spin == 1000000) {
        AHCI_TRACE("Error: Device is not responding\n");
        return;
    }

    AHCI_TRACE("Issueing command at slot %d\n", slot);

    port->ci = 1 << slot;
    
    spin = 0;
    while (spin < 1000000) {

        if ((port->ci & (1 << slot)) == 0) {
            break;
        }

        if (port->is & HBA_PxIS_TFES) {
            AHCI_TRACE("Error: Task File Error!\n");
            return;
        }

        spin++;
    }

#if AHCI_DEBUG
    char name[41];

    for(unsigned int k = 0; k < 40; k += 2) {
        name[k] = buffer[54 + k + 1];
        name[k + 1] = buffer[54 + k];
    }
    
    name[40] = 0;

    unsigned int commandsets = *((unsigned int *)(buffer + ATA_IDENT_COMMANDSETS));
    
    unsigned int size;
    if (commandsets & (1 << 26))
        // Device uses 48-Bit Addressing:
        size   = *((unsigned int *)(buffer + ATA_IDENT_MAX_LBA_EXT));
    else
        // Device uses CHS or 28-bit Addressing:
        size   = *((unsigned int *)(buffer + ATA_IDENT_MAX_LBA));

    AHCI_TRACE("Model Name: %s\n", name);
    AHCI_TRACE("Size: %dMB", size / 1024 / 2);
#endif

}

bool Ahci::ahci_rw(HbaPort *port, uint32_t startl, uint32_t starth, uint16_t count, uint16_t *buf, uint8_t command,
                   uint8_t portIndex) {

    if (count == 0) {
        AHCI_TRACE("Error: Count has to be greater than 0\n");
        return false;
    }

    if ( !isActive(port) ) {
        AHCI_TRACE("Error: Port is not inizialized\n");
        return false;
    }

    uint32_t totalCount = count;

    IOMemInfo ioMemInfo = SystemManagement::getInstance()->mapIO(count * 512 * 2);

    if(command == ATA_CMD_WRITE_DMA_EX) {
        memcpy((void *) ioMemInfo.virtStartAddress, buf, count * 512);
    }

    HbaPort_Virt *virtPort = sataDevices_Virt[portIndex];

    port->is = port->is; // @suppress("Assignment to itself")

    int spin = 0;
    int slot = findCmdSlot(port);

    if (slot == -1) {
        return false;
    }

    HbaCmdHeader *cmdheader = (HbaCmdHeader*) virtPort->clb;
    cmdheader += slot;
    cmdheader->cfl = sizeof(FisRegH2D) / sizeof(uint32_t);
    cmdheader->w = command == ATA_CMD_READ_DMA_EX ? 0 : 1;

    cmdheader->prdtl = count % 8 == 0 ? count / 8 : count / 8 + 1;

    HbaCmdTbl *cmdtbl = (HbaCmdTbl*)(virtCtbas[portIndex][slot]);
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
    cmdtbl->prdt_entry[i].dbc = (count << 9) - 1;
    cmdtbl->prdt_entry[i].i = 0;

    FisRegH2D *cmdfis = (FisRegH2D*) (&cmdtbl->cfis);

    cmdfis->fis_type = FIS_TYPE_REG_H2D;
    cmdfis->c = 1;
    cmdfis->command = command;

    cmdfis->lba0 = (uint8_t) (startl);
    cmdfis->lba1 = (uint8_t) (startl >> 8);
    cmdfis->lba2 = (uint8_t) (startl >> 16);

    cmdfis->device = 1 << 6;

    cmdfis->lba3 = (uint8_t) (startl >> 24);
    cmdfis->lba4 = (uint8_t) (starth);
    cmdfis->lba5 = (uint8_t) (starth >> 8);

    cmdfis->countl = (uint8_t) totalCount & 0xFF;
    cmdfis->counth = (uint8_t) (totalCount >> 8) & 0xFF;

    cmdfis->featurel = 1;

    while ( (port->tfd & (HBA_PxTFD_STS_BSY | HBA_PxTFD_STS_DRQ)) && spin < 1000000 ) {
        spin++;
    }

    if (spin == 1000000) {
        AHCI_TRACE("Error: Device is not responding\n");
        SystemManagement::getInstance()->freeIO(ioMemInfo);
        return false;
    }

    //AHCI_TRACE("Issueing command for slot %d\n", slot);

    port->ci = 1 << slot;

    spin = 0;
    while (spin < 1000000) {

        if ((port->ci & (1 << slot)) == 0) {
            break;
        }

        if (port->is & HBA_PxIS_TFES) {
            AHCI_TRACE("Error: Task File Error!\n");
            SystemManagement::getInstance()->freeIO(ioMemInfo);
            return false;
        }

        spin++;
    }

    if (spin == 1000000) {
        AHCI_TRACE("Error: Device is hung\n");
        SystemManagement::getInstance()->freeIO(ioMemInfo);
        return false;
    }

    if (port->is & HBA_PxIS_TFES || port->tfd & HBA_PxTFD_ERR) {
        AHCI_TRACE("Task File Error!\n");
        SystemManagement::getInstance()->freeIO(ioMemInfo);
        return false;
    }

    memcpy((void*)buf, (void*)(ioMemInfo.virtStartAddress), totalCount * 512);
    SystemManagement::getInstance()->freeIO(ioMemInfo);

    return true;
}

int Ahci::findCmdSlot(HbaPort *port) {
    uint32_t slots = (port->sact | port->ci);

    for (int i = 0; i < numCmdSlots; i++)	{
        if ((slots & (1 << i)) == 0) {
            return i;
        }
    }

    return -1;
}

void Ahci::rebasePort(HbaPort *port, int portno) {

    AHCI_TRACE("Rebasing port %d\n", portno);

    IOMemInfo tmp = SystemManagement::getInstance()->mapIO(4096);

    uint32_t clb = (uint32_t) tmp.virtStartAddress;
    memset((void*) clb, 0, 4096);


    HbaPort_Virt* portVirt = new HbaPort_Virt;
    sataDevices_Virt[portno] = portVirt;
    portVirt->clb = tmp.virtStartAddress;

	port->clb = tmp.physAddresses[0];
	port->clbu = 0;
	
    AHCI_TRACE("Rebased Command List to %x\n", port->clb);

    uint32_t fb = port->clb + 1024;
    portVirt->fb = tmp.virtStartAddress + 1024;
	port->fb = fb;
	port->fbu = 0;
	
    AHCI_TRACE("Rebased FIS area to %x\n", port->fb);

    tmp = SystemManagement::getInstance()->mapIO(8192);
    memset((void*) tmp.virtStartAddress, 0, 8192);

	HbaCmdHeader *cmdheader = (HbaCmdHeader*) (portVirt->clb);

	for (uint32_t i = 0; i < MAX_CMD_SLOTS; i++) {
        virtCtbas[portno][i] = (uint32_t)tmp.virtStartAddress + i * 256;

        uint32_t physAddr = (i < 16) ? tmp.physAddresses[0] : tmp.physAddresses[1];
        uint32_t ctba = physAddr + i * 256;

        cmdheader[i].ctba = ctba;
		cmdheader[i].ctbau = 0;
		cmdheader[i].prdtl = 16;
    }
    
    AHCI_TRACE("Rebased %d Command Headers ( %x - %x )\n",
        MAX_CMD_SLOTS,
        cmdheader[0].ctba,
        cmdheader[MAX_CMD_SLOTS - 1].ctba);    

    port->serr = port->serr; // @suppress("Assignment to itself")
    port->is = port->is; // @suppress("Assignment to itself")
}

void Ahci::startAll() {
    for (uint32_t i = 0; i <= numPorts; i++) {
        if ( isPortImplemented(i) ) {
            AHCI_TRACE("Starting port %d\n", i);
            startCommand(&abar->ports[i]);
        }
    }
}

void Ahci::startCommand(HbaPort *port) {
	while (port->cmd & (HBA_PxCMD_CR | HBA_PxCMD_FR));

    port->cmd |= HBA_PxCMD_ST | HBA_PxCMD_FRE;
}

void Ahci::stopAll() {
    for (uint32_t i = 0; i < numPorts; i++) {
        if ( isPortImplemented(i) ) {
            AHCI_TRACE("Stopping port %d\n", i);
            stopCommand(&abar->ports[i]);
        }
    }
}


void Ahci::stopCommand(HbaPort *port) {
	port->cmd &= ~(HBA_PxCMD_ST | HBA_PxCMD_FRE);

	while(port->cmd & (HBA_PxCMD_FR | HBA_PxCMD_CR));
}

void Ahci::plugin() {

}

void Ahci::trigger() {
    // TODO(krakowski):
    //  Wake up waiting threads once i/o-manager is implemented
}
