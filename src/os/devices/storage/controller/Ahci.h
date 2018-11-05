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


#ifndef __Ahci_include__
#define __Ahci_include__

#include "devices/pci/Pci.h"
#include "kernel/interrupts/InterruptHandler.h"

#include <cstdint>
#include <kernel/services/DebugService.h>
#include <devices/storage/devices/StorageDevice.h>
#include <lib/libc/printf.h>
#include <devices/pci/PciDeviceDriver.h>
#include "kernel/log/Logger.h"

#define MAX_DEVICES 8

/**
 * Advanced Host Controller Interface driver.
 *
 * @author Filip Krakowski
 */
class Ahci : public InterruptHandler, public PciDeviceDriver {

private:

    static Logger &log;

    static uint32_t deviceCount;

    typedef enum {
        FIS_TYPE_REG_H2D	= 0x27,	// Register FIS - host to device
        FIS_TYPE_REG_D2H	= 0x34,	// Register FIS - device to host
        FIS_TYPE_DMA_ACT	= 0x39,	// DMA activate FIS - device to host
        FIS_TYPE_DMA_SETUP	= 0x41,	// DMA setup FIS - bidirectional
        FIS_TYPE_DATA		= 0x46,	// Data FIS - bidirectional
        FIS_TYPE_BIST		= 0x58,	// BIST activate FIS - bidirectional
        FIS_TYPE_PIO_SETUP	= 0x5F,	// PIO setup FIS - device to host
        FIS_TYPE_DEV_BITS	= 0xA1,	// Set device bits FIS - device to host
    } FisType;

    typedef struct tagFIS_REG_H2D {
        uint8_t	fis_type;	// FIS_TYPE_REG_H2D

        uint8_t	pmport:4;	// Port multiplier
        uint8_t	rsv0:3;		// Reserved
        uint8_t	c:1;		// 1: Command, 0: Control

        uint8_t	command;	// Command register
        uint8_t	featurel;	// Feature register, 7:0

        uint8_t	lba0;		// LBA low register, 7:0
        uint8_t	lba1;		// LBA mid register, 15:8
        uint8_t	lba2;		// LBA high register, 23:16
        uint8_t	device;		// Device register

        uint8_t	lba3;		// LBA register, 31:24
        uint8_t	lba4;		// LBA register, 39:32
        uint8_t	lba5;		// LBA register, 47:40
        uint8_t	featureh;	// Feature register, 15:8

        uint8_t	countl;		// Count register, 7:0
        uint8_t	counth;		// Count register, 15:8
        uint8_t	icc;		// Isochronous command completion
        uint8_t	control;	// Control register

        uint8_t	rsv1[4];	// Reserved
    } FisRegH2D;

    typedef struct tagFIS_REG_D2H {
        uint8_t	fis_type;    // FIS_TYPE_REG_D2H

        uint8_t	pmport:4;    // Port multiplier
        uint8_t	rsv0:2;      // Reserved
        uint8_t	i:1;         // Interrupt bit
        uint8_t	rsv1:1;      // Reserved

        uint8_t	status;      // Status register
        uint8_t	error;       // Error register

        uint8_t	lba0;        // LBA low register, 7:0
        uint8_t	lba1;        // LBA mid register, 15:8
        uint8_t	lba2;        // LBA high register, 23:16
        uint8_t	device;      // Device register

        uint8_t	lba3;        // LBA register, 31:24
        uint8_t	lba4;        // LBA register, 39:32
        uint8_t	lba5;        // LBA register, 47:40
        uint8_t	rsv2;        // Reserved

        uint8_t	countl;      // Count register, 7:0
        uint8_t	counth;      // Count register, 15:8
        uint8_t	rsv3[2];     // Reserved

        uint8_t	rsv4[4];     // Reserved
    } FisRegD2H;

    typedef struct tagFIS_DATA {
        uint8_t	fis_type;	// FIS_TYPE_DATA

        uint8_t	pmport:4;	// Port multiplier
        uint8_t	rsv0:4;		// Reserved

        uint8_t	rsv1[2];	// Reserved

        uint32_t	data[1];	// Payload
    } FisData;

    typedef struct tagFIS_PIO_SETUP {
        uint8_t	fis_type;	// FIS_TYPE_PIO_SETUP

        uint8_t	pmport:4;	// Port multiplier
        uint8_t	rsv0:1;		// Reserved
        uint8_t	d:1;		// Data transfer direction, 1 - device to host
        uint8_t	i:1;		// Interrupt bit
        uint8_t	rsv1:1;

        uint8_t	status;		// Status register
        uint8_t	error;		// Error register

        uint8_t	lba0;		// LBA low register, 7:0
        uint8_t	lba1;		// LBA mid register, 15:8
        uint8_t	lba2;		// LBA high register, 23:16
        uint8_t	device;		// Device register

        uint8_t	lba3;		// LBA register, 31:24
        uint8_t	lba4;		// LBA register, 39:32
        uint8_t	lba5;		// LBA register, 47:40
        uint8_t	rsv2;		// Reserved

        uint8_t	countl;		// Count register, 7:0
        uint8_t	counth;		// Count register, 15:8
        uint8_t	rsv3;		// Reserved
        uint8_t	e_status;	// New value of status register

        uint16_t	tc;		// Transfer count
        uint8_t	rsv4[2];	// Reserved
    } FisPioSetup;

    typedef struct tagFIS_DMA_SETUP {
        uint8_t	fis_type;	// FIS_TYPE_DMA_SETUP

        uint8_t	pmport:4;	// Port multiplier
        uint8_t	rsv0:1;		// Reserved
        uint8_t	d:1;		// Data transfer direction, 1 - device to host
        uint8_t	i:1;		// Interrupt bit
        uint8_t	a:1;            // Auto-activate. Specifies if DMA Activate FIS is needed

        uint8_t    rsved[2];       // Reserved

        unsigned long long   DMAbufferID;    // DMA Buffer Identifier. Used to Identify DMA buffer in host memory. SATA Spec says host specific and not in Spec. Trying AHCI spec might work.

        uint32_t   rsvd;           //More reserved

        uint32_t   DMAbufOffset;   //Byte offset into buffer. First 2 bits must be 0

        uint32_t   TransferCount;  //Number of bytes to transfer. Bit 0 must be 0

        uint32_t   resvd;          //Reserved

    } FisDmaSetup;

    typedef volatile struct tagHBA_PORT {
        uint32_t	clb;		// 0x00, command list base address, 1K-byte aligned
        uint32_t	clbu;		// 0x04, command list base address upper 32 bits
        uint32_t	fb;		    // 0x08, FIS base address, 256-byte aligned
        uint32_t	fbu;		// 0x0C, FIS base address upper 32 bits
        uint32_t	is;		    // 0x10, interrupt status
        uint32_t	ie;		    // 0x14, interrupt enable
        uint32_t	cmd;		// 0x18, command and status
        uint32_t	rsv0;		// 0x1C, Reserved
        uint32_t	tfd;		// 0x20, task file data
        uint32_t	sig;		// 0x24, signature
        uint32_t	ssts;		// 0x28, SATA status (SCR0:SStatus)
        uint32_t	sctl;		// 0x2C, SATA control (SCR2:SControl)
        uint32_t	serr;		// 0x30, SATA error (SCR1:SError)
        uint32_t	sact;		// 0x34, SATA active (SCR3:SActive)
        uint32_t	ci;		    // 0x38, command issue
        uint32_t	sntf;		// 0x3C, SATA notification (SCR4:SNotification)
        uint32_t	fbs;		// 0x40, FIS-based switch control
        uint32_t	devslp;		// 0x44, Device Sleep
        uint32_t	rsv1[10];	// 0x48 ~ 0x6F, Reserved
        uint32_t	vendor[4];	// 0x70 ~ 0x7F, vendor specific
    } HbaPort;

    typedef volatile struct tagHBA_PORT_VIRT {
        uint32_t clb;
        uint32_t fb;

    } HbaPort_Virt;

    typedef volatile struct tagHBA_MEM {
        // 0x00 - 0x2B, Generic Host Control
        uint32_t	cap;		// 0x00, Host capability
        uint32_t	ghc;		// 0x04, Global host control
        uint32_t	is;		    // 0x08, Interrupt status
        uint32_t	pi;		    // 0x0C, Port implemented
        uint32_t	vs;		    // 0x10, Version
        uint32_t	ccc_ctl;	// 0x14, Command completion coalescing control
        uint32_t	ccc_pts;	// 0x18, Command completion coalescing ports
        uint32_t	em_loc;		// 0x1C, Enclosure management location
        uint32_t	em_ctl;		// 0x20, Enclosure management control
        uint32_t	cap2;		// 0x24, Host capabilities extended
        uint32_t	bohc;		// 0x28, BIOS/OS handoff control and status

        // 0x2C - 0x9F, Reserved
        uint8_t	rsv[0xA0-0x2C];

        // 0xA0 - 0xFF, Vendor specific registers
        uint8_t	vendor[0x100-0xA0];

        // 0x100 - 0x10FF, Port control registers
        HbaPort	ports[1];	// 1 ~ 32
    } HbaMem;

    typedef volatile struct tagHBA_FIS {
        // 0x00
        FisDmaSetup	dsfis;		// DMA Setup FIS
        uint8_t		pad0[4];

        // 0x20
        FisPioSetup	psfis;		// PIO Setup FIS
        uint8_t		pad1[12];

        // 0x40
        FisRegD2H	rfis;		// Register – Device to Host FIS
        uint8_t		pad2[4];

        // 0x58
        //FIS_DEV_BITS	sdbfis;		// Set Device Bit FIS
        uint8_t       sdbfis[2];

        // 0x60
        uint8_t		ufis[64];

        // 0xA0
        uint8_t		rsv[0x100-0xA0];
    } HbaFis;

    typedef struct tagHBA_CMD_HEADER {
        // DW0
        uint8_t	cfl:5;		// Command FIS length in DWORDS, 2 ~ 16
        uint8_t	a:1;		// ATAPI
        uint8_t	w:1;		// Write, 1: H2D, 0: D2H
        uint8_t	p:1;		// Prefetchable

        uint8_t	r:1;		// Reset
        uint8_t	b:1;		// BIST
        uint8_t	c:1;		// Clear busy upon R_OK
        uint8_t	rsv0:1;		// Reserved
        uint8_t	pmp:4;		// Port multiplier port

        uint16_t	prdtl;		// Physical region descriptor table length in entries

        // DW1
        volatile
        uint32_t	prdbc;		// Physical region descriptor byte count transferred

        // DW2, 3
        uint32_t	ctba;		// Command table descriptor base address
        uint32_t	ctbau;		// Command table descriptor base address upper 32 bits

        // DW4 - 7
        uint32_t	rsv1[4];	// Reserved
    } HbaCmdHeader;

    struct HbaCmdHeaderVirt {
        uint32_t ctba;
    };

    typedef struct tagHBA_PRDT_ENTRY {
        uint32_t	dba;		// Data base address
        uint32_t	dbau;		// Data base address upper 32 bits
        uint32_t	rsv0;		// Reserved

        // DW3
        uint32_t	dbc:22;		// Byte count, 4M max
        uint32_t	rsv1:9;		// Reserved
        uint32_t	i:1;		// Interrupt on completion
    } HbaPrdtEntry;

    typedef struct tagHBA_CMD_TBL {
        // 0x00
        uint8_t	cfis[64];	// Command FIS

        // 0x40
        uint8_t	acmd[16];	// ATAPI command, 12 or 16 bytes

        // 0x50
        uint8_t	rsv[48];	// Reserved

        // 0x80
        HbaPrdtEntry	prdt_entry[1];	// Physical region descriptor table entries, 0 ~ 65535
    } HbaCmdTbl;

  private:

	static const uint32_t SATA_SIG_ATA              = 0x00000101;       // SATA drive
    static const uint32_t SATA_SIG_ATAPI            = 0xEB140101;       // SATAPI drive
    static const uint32_t SATA_SIG_SEMB             = 0xC33C0101;	    // Enclosure management bridge
    static const uint32_t SATA_SIG_PM               = 0x96690101;       // Port multiplier

    static const uint8_t AHCI_DEV_NULL            = 0x00;
    static const uint8_t AHCI_DEV_SATA            = 0x01;
    static const uint8_t AHCI_DEV_SATAPI          = 0x02;
    static const uint8_t AHCI_DEV_SEMB            = 0x03;
    static const uint8_t AHCI_DEV_PM              = 0x04;

    static const uint8_t ATA_DEV_BUSY             = 0x80;
    static const uint8_t ATA_DEV_DRQ              = 0x08;
    static const uint8_t ATA_CMD_READ_DMA_EX      = 0x25;
    static const uint8_t ATA_CMD_WRITE_DMA_EX     = 0x35;
    static const uint8_t ATA_CMD_IDENTIFY         = 0xEC;

    /* Identify Offsets */
    static const uint8_t ATA_IDENT_DEVICETYPE       = 0;
    static const uint8_t ATA_IDENT_CYLINDERS        = 2;
    static const uint8_t ATA_IDENT_HEADS            = 6;
    static const uint8_t ATA_IDENT_SECTORS          = 12;
    static const uint8_t ATA_IDENT_SERIAL           = 20;
    static const uint8_t ATA_IDENT_MODEL            = 54;
    static const uint8_t ATA_IDENT_CAPABILITIES     = 98;
    static const uint8_t ATA_IDENT_FIELDVALID       = 106;
    static const uint8_t ATA_IDENT_MAX_LBA          = 120;
    static const uint8_t ATA_IDENT_COMMANDSETS      = 164;
    static const uint8_t ATA_IDENT_MAX_LBA_EXT      = 200;

    // -- Port x Interrupt Status -- //

    /* Task File Error Status */
    static const uint32_t HBA_PxIS_TFES     = (1u << 30u);

    // -- Port x Command and Status -- //

    /* Start */
    static const uint32_t HBA_PxCMD_ST      = (1u << 0u);

    /* Spin-Up Device */
    static const uint32_t HBA_PxCMD_SUD     = (1u << 1u);

    /* Power On Device */
    static const uint32_t HBA_PxCMD_POD     = (1u << 2u);

    /* FIS Receive Enable */
    static const uint32_t HBA_PxCMD_FRE     = (1u << 4u);

    /* FIS Receive Running */
    static const uint32_t HBA_PxCMD_FR      = (1u << 14u);

    /* Command List Running */
    static const uint32_t HBA_PxCMD_CR      = (1u << 15u);

    /* Cold Presence Detection */
    static const uint32_t HBA_PxCMD_CPD     = (1u << 20u);

    /* Interface Communication Control */
    static const uint32_t HBA_PxCMD_ICC     = (0xFu << 28u);

    static const uint32_t HBA_PxCMD_ICC_IDLE        = (0u << 28u);
    static const uint32_t HBA_PxCMD_ICC_ACTIVE      = (1u << 28u);
    static const uint32_t HBA_PxCMD_ICC_PARTIAL     = (2u << 28u);
    static const uint32_t HBA_PxCMD_ICC_SLUMBER     = (6u << 28u);
    static const uint32_t HBA_PxCMD_ICC_DEVSLEEP    = (8u << 28u);

    // -- BIOS/OS Handoff Control and Status -- //

    /* BIOS Owned Semaphore */
    static const uint32_t HBA_BOHC_BOS      = (1u << 0u);

    /* OS Owned Semaphore */
    static const uint32_t HBA_BOHC_OOS      = (1u << 1u);

    /* SMI on OS Ownership Change Enable */
    static const uint32_t HBA_BOHC_SOOE     = (1u << 2u);

    /* OS Ownership Change */
    static const uint32_t HBA_BOHC_OOC      = (1u << 3u);

    /* BIOS Busy */
    static const uint32_t HBA_BOHC_BB       = (1u << 4u);

    // -- Port x Serial ATA Status -- //

    /* Interface Power Management */
    static const uint8_t HBA_PxSSTS_IPM                = 0xF0;

    static const uint8_t HBA_PxSSTS_IPM_NOT_PRESENT   = 0x00;
    static const uint8_t HBA_PxSSTS_IPM_ACTIVE        = 0x01;
    static const uint8_t HBA_PxSSTS_IPM_PARTIAL       = 0x02;
    static const uint8_t HBA_PxSSTS_IPM_SLUMBER       = 0x06;
    static const uint8_t HBA_PxSSTS_IPM_SLEEP         = 0x08;

    /* Device Detection */
    static const uint8_t HBA_PxSSTS_DET               = 0x0F;

    static const uint8_t HBA_PxSSTS_DET_NO_DEVICE     = 0x00;
    static const uint8_t HBA_PxSSTS_DET_NO_PHY        = 0x01;
    static const uint8_t HBA_PxSSTS_DET_PHY           = 0x03;
    static const uint8_t HBA_PxSSTS_DET_OFFLINE       = 0x04;

    // -- HBA Capabilities -- //

    /* Number of ports */
    static const uint8_t HBA_CAP_NP                   = 0x0F;

    /* Number of Command Slots */
    static const uint32_t HBA_CAP_NCS                 = (0xFu << 8u);

    /* Supports AHCI mode only */
    static const uint32_t HBA_CAP_SAM                 = (1u << 18u);

    /* Supports Staggered Spin-up */
    static const uint32_t HBA_CAP_SSS                 = (1u << 27u);

    // -- HBA Capabilities Extended -- //

    /* BIOS/OS Handoff */
    static const uint8_t HBA_CAP2_BOH   = (1u << 0u);

    /* NVMHCI Present */
    static const uint8_t HBA_CAP2_NVMP  = (1u << 1u);

    /* Automatic Partial to Slumber Transitions */
    static const uint8_t HBA_CAP2_APST  = (1u << 2u);

    /* Supports Device Sleep */
    static const uint8_t HBA_CAP2_SDS   = (1u << 3u);

    /* Supports Aggressive Device Sleep */
    static const uint8_t HBA_CAP2_SADM  = (1u << 4u);

    /* DevSleep Entrance from Slumber Only */
    static const uint8_t HBA_CAP2_DESO  = (1u << 5u);

    // -- Port x Serial ATA Control -- //

    /* Device Detection Initialization */
    static const uint8_t HBA_PxSCTL_DET                = 0x0F;

    static const uint8_t HBA_PxSCTL_DET_NO_DETECTION   = 0x00;
    static const uint8_t HBA_PxSCTL_DET_ESTABLISH_COM  = 0x01;
    static const uint8_t HBA_PxSCTL_DET_DISABLE        = 0x04;

    /* Speed Allowed */
    static const uint8_t HBA_PxSCTL_SPD     = 0xF0;

    /* Interface Power Management Transitions Allowed */
    static const uint16_t HBA_PxSCTL_IPM    =  0xF00;

    static const uint16_t HBA_PxSCTL_IPM_NO_RESTRICTIONS  = (0u << 8u);
    static const uint16_t HBA_PxSCTL_IPM_NO_PARTIAL       = (1u << 8u);
    static const uint16_t HBA_PxSCTL_IPM_NO_SLUMBER       = (2u << 8u);
    static const uint16_t HBA_PxSCTL_IPM_NO_DEVSLEEP      = (4u << 8u);

    // -- Port x Task File Data -- //

    static const uint8_t HBA_PxTFD_STS         = 0xFF;

    static const uint8_t HBA_PxTFD_STS_ERR     = (1u << 0u);
    static const uint8_t HBA_PxTFD_STS_DRQ     = (1u << 3u);
    static const uint8_t HBA_PxTFD_STS_BSY     = (1u << 3u);

    static const uint16_t HBA_PxTFD_ERR    = 0xFF00;

    // -- Global HBA Control -- //

    static const uint32_t HBA_GHC_HR        = (1u << 0u);

    static const uint32_t HBA_GHC_AE        = (1u << 31u);

    // -- Constants -- //

    static const uint32_t MAX_CMD_SLOTS     = 32;


    /**
     *Scans all implemented ports for devices.
     */
    void scan();

    /**
     * Determines a ports type.
     *
     * @param port the port to check.
     * @return the ports type.
     */
    uint8_t checkType(HbaPort* port);

    /**
     * Finds an empty command slot for a specified port.
     *
     * @param port   the port.
     * @return the next free command slots index. -1 if no slot is found.
     */
    int findCmdSlot(HbaPort *port);

    /**
     * Rebases a specified port.
     *
     * @param port   the port to rebase.
     * @param portno   the ports number.
     */
    void rebasePort(HbaPort *port, int portNumber);

    /**
     * Stops a specific port.
     *
     * @param port the port to stop.
     */
    void stopCommand(HbaPort *port);

    /**
     * Starts a specific port.
     *
     * @param port the port to stop.
     */
    void startCommand(HbaPort *port);

    /**
     * Stops all implemented ports.
     */
    void stopAll();

    /**
     * Starts all implemented ports.
     */
    void startAll();

    /**
     * Resets all implemented ports.
     */
    void resetAll();

    /**
     * Rebases all implemented ports.
     */
    void rebaseAll();

    /**
     * Resets a single HBA port.
     *
     * @param port the port to reset.
     */
    void resetPort(HbaPort *port);

    /**
     * Checks if a port is currently running.
     *
     * @param port the HBA port to check.
     * @return true if the port is running, else false.
     */
    bool isActive(HbaPort *port);

    /**
     * Checks if a port at a specified location is implemented.
     *
     * @param portNumber the ports location.
     * @return true if the port is implemented, else false.
     */
    bool isPortImplemented(uint16_t portNumber);

    /**
     * Reads the HBAs configuration.
     */
    void readConfig();

    /**
     * Performs a BIOS handoff if the HBA supports it.
     */
    void biosHandoff();

    /**
     * Sets the HBA into AHCI mode.
     */
    void enableAhci();

    /**
     * Resets the HBA.
     */
    bool reset();

    TimeService *timeService;
    static const uint32_t AHCI_TIMEOUT = 1000;

    HbaPort *sataDevices[MAX_DEVICES];
    HbaPort_Virt *sataDevices_Virt[MAX_DEVICES];
    uint32_t virtCtbas[MAX_DEVICES][MAX_CMD_SLOTS];
    uint8_t numDevices = 0;

    Pci::Device pciDevice;

    uint8_t   numPorts            = 0;
    uint16_t  numCmdSlots         = 0;
    uint32_t  pi                  = 0;

    uint32_t  signatures[32];

    bool ahci_rw(HbaPort *port, uint32_t startl, uint32_t starth, uint16_t count, uint16_t *buf, uint8_t command,
                     uint8_t portIndex);

  public:

    struct AhciDeviceInfo {
        char name[41];
        uint64_t sectorCount;
    };

    HbaMem* abar = nullptr;

    Ahci();

    PCI_DEVICE_DRIVER_IMPLEMENT_CREATE_INSTANCE(Ahci);

    TimeProvider &time;

    uint8_t getBaseClass() const override {
        return Pci::CLASS_MASS_STORAGE_DEVICE;
    }

    uint8_t getSubClass() const override {
        return Pci::SUBCLASS_SERIAL_ATA;
    }

    PciDeviceDriver::SetupMethod getSetupMethod() const override {
        return PciDeviceDriver::BY_CLASS;
    }

    void setup(const Pci::Device &dev) override;

    /**
    * Reads data from a specified device.
    *
    * @param device   the device number.
    * @param startl   the start sector (low).
    * @param starth   the start sector (high).
    * @param count    the number of sectors to read.
    * @param buf      the buffer data will be written to.
    * @return true if the operation succeeded, else false.
    */
    bool read(uint8_t device, uint32_t startl, uint32_t starth, uint16_t count, uint16_t *buf);

    /**
     * Writes data to a specified device.
     *
     * @param device   the device number.
     * @param startl   the start sector (low).
     * @param starth   the start sector (high).
     * @param count    the number of sectors to write.
     * @param buf      the data that should be written.
     * @return true if the operation succeeded, else false.
     */
    bool write(uint8_t device, uint32_t startl, uint32_t starth, uint16_t count, uint16_t *buf);

    /**
     * Returns the number of devices detected by this AHCI.
     *
     * @return The number of devices
     */
    uint8_t getNumDevices();

    /**
     * Get information about an AHCI device.
     *
     * @param deviceNumber The device's number
     */
    AhciDeviceInfo getDeviceInfo(uint16_t deviceNumber);

    void plugin();

    void trigger(InterruptFrame &frame) override;
};

#endif
