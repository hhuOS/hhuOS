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
 * The AHCI driver is based on a bachelor's thesis, written by Manuel Demetrio Angelescu.
 * The original source code can be found here: https://github.com/m8nu/hhuOS
 */

#ifndef HHUOS_AHCICONTROLLER_H
#define HHUOS_AHCICONTROLLER_H

#include <stdint.h>

#include "device/bus/pci/PciDevice.h"
#include "kernel/interrupt/InterruptHandler.h"

namespace Kernel {
enum InterruptVector : uint8_t;
struct InterruptFrame;
}  // namespace Kernel
namespace Util {
namespace Async {
class Spinlock;
}  // namespace Async
}  // namespace Util

namespace Device::Storage {

class AhciController : Kernel::InterruptHandler {

public:

    enum TransferMode : uint8_t {
        READ = 0x0,
        WRITE = 0x1,
    };

    /**
     * https://forum.osdev.org/viewtopic.php?f=1&t=30118
     */
    struct DeviceInfo {
        uint16_t config;                /* lots of obsolete bit flags */
        uint16_t cyls;                  /* obsolete */
        uint16_t reserved2;             /* special config */
        uint16_t heads;                 /* "physical" heads */
        uint16_t track_bytes;           /* unformatted bytes per track */
        uint16_t bytesPerSector;        /* unformatted bytes per sector */
        uint16_t sectors;               /* "physical" sectors per track */
        uint16_t vendor0;               /* vendor unique */
        uint16_t vendor1;               /* vendor unique */
        uint16_t vendor2;               /* vendor unique */
        uint8_t serialNumber[20];       /* 0 = not specified */
        uint16_t buf_type;
        uint16_t buf_size;              /* 512 byte increments; 0 = not specified */
        uint16_t ecc_bytes;             /* for r/w long cmds; 0 = not specified */
        uint8_t firmwareRevision[8];    /* 0 = not specified */
        uint8_t model[40];              /* 0 = not specified */
        uint16_t multi_count;           /* Multiple Count */
        uint16_t dword_io;              /* 0=not_implemented; 1=implemented */
        uint16_t capability1;           /* vendor unique */
        uint16_t capability2;           /* bits 0:DMA 1:LBA 2:IORDYsw 3:IORDYsup word: 50 */
        uint8_t vendor5;                /* vendor unique */
        uint8_t tPIO;                   /* 0 = slow, 1 = medium, 2 = fast */
        uint8_t vendor6;                /* vendor unique */
        uint8_t tDMA;                   /* 0 = slow, 1 = medium, 2 = fast */
        uint16_t field_valid;           /* bits 0:cur_ok 1:eide_ok */
        uint16_t cur_cyls;              /* logical cylinders */
        uint16_t cur_heads;             /* logical heads word 55 */
        uint16_t cur_sectors;           /* logical sectors per track */
        uint16_t cur_capacity0;         /* logical total sectors on drive */
        uint16_t cur_capacity1;         /* (2 words, misaligned int)     */
        uint8_t multsect;               /* current multiple sector count */
        uint8_t multsect_valid;         /* when (bit0==1) multsect is ok */
        uint32_t lbaCapacity;           /* total number of sectors */
        uint16_t dma_1word;             /* single-word dma info */
        uint16_t dma_mword;             /* multiple-word dma info */
        uint16_t eide_pio_modes;        /* bits 0:mode3 1:mode4 */
        uint16_t eide_dma_min;          /* min mword dma cycle time (ns) */
        uint16_t eide_dma_time;         /* recommended mword dma cycle time (ns) */
        uint16_t eide_pio;              /* min cycle time (ns), no IORDY */
        uint16_t eide_pio_iordy;        /* min cycle time (ns), with IORDY */
        uint16_t words69_70[2];         /* reserved words 69-70 */
        uint16_t words71_74[4];         /* reserved words 71-74 */
        uint16_t queue_depth;
        uint16_t sata_capability;       /* SATA Capabilities word 76 */
        uint16_t sata_additional;       /* Additional Capabilities */
        uint16_t sata_supported;        /* SATA Features supported */
        uint16_t features_enabled;      /* SATA features enabled */
        uint16_t major_rev_num;         /* Major rev number word 80 */
        uint16_t minor_rev_num;         /* Minor revision number */
        uint16_t command_set_1;         /* bits 0: Smart, 1: Security, 2: Removable, 3: PM */
        uint16_t command_set_2;         /* bits 14:Smart Enabled 13:0 zero */
        uint16_t cfsse;                 /* command set-feature supported extensions */
        uint16_t cfs_enable_1;          /* command set-feature enabled */
        uint16_t cfs_enable_2;          /* command set-feature enabled */
        uint16_t csf_default;           /* command set-feature default */
        uint16_t dma_ultra;
        uint16_t word89;                /* reserved (word 89) */
        uint16_t word90;                /* reserved (word 90) */
        uint16_t CurAPMvalues;          /* current APM values */
        uint16_t word92;                /* reserved (word 92) */
        uint16_t comreset;              /* should be cleared to 0 */
        uint16_t accoustic;             /*  accoustic management */
        uint16_t min_req_sz;            /* Stream minimum required size */
        uint16_t transfer_time_dma;     /* Streaming Transfer Time-DMA */
        uint16_t access_latency;        /* Streaming access latency-DMA & PIO WORD 97*/
        uint32_t perf_granularity;      /* Streaming performance granularity */
        uint32_t total_usr_sectors[2];  /* Total number of user addressable sectors */
        uint16_t transfer_time_pio;     /* Streaming Transfer time PIO */
        uint16_t reserved105;           /* Word 105 */
        uint16_t sector_sz;             /* Physical Sector size / Logical sector size */
        uint16_t inter_seek_delay;      /* In microseconds */
        uint16_t words108_116[9];       /* Reserved */
        uint32_t words_per_sector;      /* words per logical sectors */
        uint16_t supported_settings;    /* continued from words 82-84 */
        uint16_t command_set_3;         /* continued from words 85-87 */
        uint16_t words121_126[6];       /* reserved words 121-126 */
        uint16_t word127;               /* reserved (word 127) */
        uint16_t security_status;       /* device lock function
                                         * 15:9   reserved
                                         * 8   security level 1:max 0:high
                                         * 7:6   reserved
                                         * 5   enhanced erase
                                         * 4   expire
                                         * 3   frozen
                                         * 2   locked
                                         * 1   en/disabled
                                         * 0   capability */
        uint16_t  csfo;                 /* current set features options
                                         * 15:4   reserved
                                         * 3   auto reassign
                                         * 2   reverting
                                         * 1   read-look-ahead
                                         * 0   write cache */
        uint16_t words130_155[26];      /* reserved vendor words 130-155 */
        uint16_t word156;
        uint16_t words157_159[3];       /* reserved vendor words 157-159 */
        uint16_t cfa;                   /* CFA Power mode 1 */
        uint16_t words161_175[15];      /* Reserved */
        uint8_t media_serial[60];       /* words 176-205 Current Media serial number */
        uint16_t sct_cmd_transport;     /* SCT Command Transport */
        uint16_t words207_208[2];       /* reserved */
        uint16_t block_align;           /* Alignement of logical blocks in larger physical blocks */
        uint32_t WRV_sec_count;         /* Write-Read-Verify sector count mode 3 only */
        uint32_t verf_sec_count;        /* Verify Sector count mode 2 only */
        uint16_t nv_cache_capability;   /* NV Cache capabilities */
        uint16_t nv_cache_sz;           /* NV Cache size in logical blocks */
        uint16_t nv_cache_sz2;          /* NV Cache size in logical blocks */
        uint16_t rotation_rate;         /* Nominal media rotation rate */
        uint16_t word218;               /* Reserved  */
        uint16_t nv_cache_options;      /* NV Cache options */
        uint16_t words220_221[2];       /* reserved */
        uint16_t transport_major_rev;
        uint16_t transport_minor_rev;
        uint16_t words224_233[10];      /* Reserved */
        uint16_t min_dwnload_blocks;    /* Minimum number of 512 byte units per DOWNLOAD MICROCODE command for mode 03h */
        uint16_t max_dwnload_blocks;    /* Maximum number of 512 byte units per DOWNLOAD MICROCODE command for mode 03h */
        uint16_t words236_254[19];      /* Reserved */
        uint16_t integrity;             /* Cheksum, Signature */
    } __attribute__((packed));

    /**
     * Constructor.
     */
    explicit AhciController(const PciDevice &pciDevice);

    /**
     * Copy Constructor.
     */
    AhciController(const AhciController &other) = delete;

    /**
     * Assignment operator.
     */
    AhciController &operator=(const AhciController &other) = delete;

    /**
     * Destructor.
     */
    ~AhciController() override;

    static void initializeAvailableControllers();

    uint16_t performAtaIO(uint32_t portNumber, const DeviceInfo &deviceInfo, TransferMode mode, uint8_t *buffer, uint64_t startSector, uint32_t sectorCount);

    void plugin() override;

    void trigger(const Kernel::InterruptFrame &frame, Kernel::InterruptVector slot) override;

private:

    enum DeviceSignature : uint32_t {
        NONE = 0x00000000,
        ATA = 0x00000101,
        ATAPI = 0xeb140101,
        ENCLOSURE_POWER_MANAGEMENT_BRIDGE = 0xc33c0101,
        PORT_MULTIPLIER = 0x96690101
    };

    enum DeviceDetection : uint8_t {
        NO_DEVICE = 0x00,
        NO_PHYSICAL_COMMUNICATION = 0x01,
        PRESENT = 0x03,
        OFFLINE = 0x04
    };

    enum InterfacePowerManagement : uint8_t {
        NOT_PRESENT = 0x00,
        ACTIVE = 0x01,
        PARTIAL_POWER_MANAGEMENT = 0x02,
        SLUMBER_POWER_MANAGEMENT = 0x06,
        SLEEP_POWER_MANAGEMENT = 0x08
    };

    enum BiosHandoffFlags {
        BIOS_OWNED_SEMAPHORE = 1 << 0,
        OS_OWNED_SEMAPHORE = 1 << 1,
        SMI_ON_OWNERSHIP_CHANGE = 1 << 2,
        OS_OWNERSHIP_CHANGE = 1 << 3,
        BIOS_BUSY = 1 << 4
    };

    enum PortCommand {
        START = 1 << 0,
        FIS_RECEIVE_ENABLE = 1 << 4,
        FIS_RECEIVE_RUNNING = 1 << 14,
        COMMAND_LIST_RUNNING = 1 << 15
    };

    enum HostControl {
        HBA_RESET = 1 << 0,
        INTERRUPT_ENABLE = 1 << 1,
        MSI_REVERT_TO_SINGLE_MESSAGE = 1 << 2,
        AHCI_ENABLE = 1 << 31
    };

    enum TaskFileStatus {
        ERROR = 1 << 0,
        DATA_TRANSFER_REQUESTED = 1 << 3,
        BUSY = 1 << 7
    };

    enum PortInterruptStatus {
        TASK_FILE_ERROR = 1 << 30
    };

    enum FisType : uint8_t {
        REGISTER_HOST_TO_DEVICE = 0x27,
        REGISTER_DEVICE_TO_HOST = 0x34,
        DMA_ACTIVATE = 0x39,
        DMA_SETUP = 0x41,
        DMA_DATA = 0x46,
        BIST_ACTIVATE = 0x58,
        PIO_SETUP = 0x5f,
        DEVICE_BITS = 0xa1
    };

    enum Command : uint8_t {
        ATA_IDENTIFY = 0xec,
        ATAPI_IDENTIFY = 0xa1,
        ATA_PACKET = 0xa0,
        READ_DMA = 0xc8,
        READ_DMA_EX = 0x25,
        WRITE_DMA = 0xca,
        WRITE_DMA_EX = 0x35
    };

    struct FisRegisterHostToDevice {
        // DWORD 0
        FisType type;

        uint8_t portMultiplierPort: 4;
        uint8_t reserved1: 3;
        uint8_t commandControl: 1;

        uint8_t command;
        uint8_t featureLow;

        // DWORD 1
        uint8_t lba0;
        uint8_t lba1;
        uint8_t lba2;
        uint8_t device;

        // DWORD 2
        uint8_t lba3;
        uint8_t lba4;
        uint8_t lba5;
        uint8_t featureHigh;

        // DWORD 3
        uint8_t countLow;
        uint8_t countHigh;
        uint8_t isochronousCommandCompletion;
        uint8_t control;

        // DWORD 4
        uint32_t reserved2;
    } __attribute__((packed));

    struct HbaPort {
        uint32_t commandListBaseAddress;
        uint32_t commandListBaseAddressUpper;
        uint32_t fisBaseAddress;
        uint32_t fisBaseAddressUpper;
        uint32_t interruptStatus;
        uint32_t interruptEnable;
        uint32_t command;
        uint32_t reserved1;
        uint32_t taskFileData;
        DeviceSignature signature;
        uint32_t sataStatus;
        uint32_t sataControl;
        uint32_t sataError;
        uint32_t sataActive;
        uint32_t commandIssue;
        uint32_t sataNotification;
        uint32_t fisBasedSwitchControl;
        uint32_t deviceSleep;
        uint32_t reserved2[10];
        uint32_t vendorSpecific[4];

        void startCommandEngine();

        void stopCommandEngine();

        [[nodiscard]] bool isActive() const;

        [[nodiscard]] DeviceSignature checkType() const;
    } __attribute__((packed));

    struct HbaRegisters {
        uint32_t hostCapabilities;
        uint32_t globalHostControl;
        uint32_t interruptStatus;
        uint32_t portsImplemented;
        uint32_t version;
        uint32_t commandCompletionCoalescingControl;
        uint32_t commandCompletionCoalescingPorts;
        uint32_t enclosureManagementLocation;
        uint32_t enclosureManagementControl;
        uint32_t extendedHostCapabilities;
        uint32_t biosHandoffControl;
        uint8_t reserved[116];
        uint8_t vendorSpecific[96];
        HbaPort ports[];
    } __attribute__((packed));

    struct HbaPhysicalRegionDescriptorTableEntry {
        uint32_t dataBaseAddress;
        uint32_t dataBaseAddressUpper;
        uint32_t reserved1;

        uint32_t dataByteCount: 22;
        uint32_t reserved2: 9;
        uint32_t interruptOnCompletion: 1;
    } __attribute__((packed));

    struct HbaCommandHeader {
        // DWORD 0
        uint8_t commandFisLength: 5;
        uint8_t atapi: 1;
        uint8_t write: 1;
        uint8_t prefetchable: 1;

        uint8_t reset: 1;
        uint8_t bist: 1;
        uint8_t clearBusyOnOK: 1;
        uint8_t reserved1: 1;
        uint8_t portMultiplierPort: 4;

        uint16_t physicalRegionDescriptorTableLength;

        // DWORD 1
        uint32_t physicalRegionDescriptorByteCount;

        // DWORD 2-3
        uint32_t commandTableDescriptorBaseAddress;
        uint32_t commandTableDescriptorBaseAddressUpper;

        // DWORD 4-7
        uint32_t reserved[4];

        void clear();
    } __attribute__((packed));

    struct HbaCommandTable {
        uint8_t commandFis[64];
        uint8_t atapiCommand[16];
        uint8_t reserved[48];
        HbaPhysicalRegionDescriptorTableEntry physicalRegionDescriptorTable[];

        static HbaCommandTable* createCommandTable(uint16_t descriptorCount);
    } __attribute__((packed));

    bool biosHandoff();

    bool enableAhci();

    void rebasePort(uint32_t portNumber);

    DeviceInfo* identifyDevice(uint32_t portNumber);

    uint32_t findCommandSlot(uint32_t portNumber);

    static void byteSwapString(char *string, uint32_t length);

    PciDevice pciDevice;
    HbaRegisters *registers = nullptr;
    HbaCommandHeader **virtualCommandLists = nullptr;
    Util::Async::Spinlock *portLocks = nullptr;
    uint32_t portCount = 0;

    static const constexpr uint8_t PCI_SUBCLASS_AHCI = 0x06;
    static const constexpr uint32_t AHCI_ENABLE_TIMEOUT_MS = 1000;
    static const constexpr uint32_t COMMAND_TIMEOUT = 10000;
    static const constexpr uint32_t SECTORS_PER_DESCRIPTOR_ENTRY = 8;
};

}

#endif
