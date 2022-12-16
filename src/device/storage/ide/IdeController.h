/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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
 */

#ifndef HHUOS_IDECONTROLLER_H
#define HHUOS_IDECONTROLLER_H

#include <cstdint>

#include "kernel/interrupt/InterruptHandler.h"
#include "device/cpu/IoPort.h"
#include "lib/util/async/Spinlock.h"

namespace Device {
class PciDevice;
}  // namespace Device
namespace Kernel {
class Logger;
struct InterruptFrame;
}  // namespace Kernel

namespace Device::Storage {

class IdeController : public Kernel::InterruptHandler {

public:
    /**
     * Constructor.
     */
    explicit IdeController(const PciDevice &pciDevice);

    /**
     * Copy Constructor.
     */
    IdeController(const IdeController &other) = delete;

    /**
     * Assignment operator.
     */
    IdeController &operator=(const IdeController &other) = delete;

    /**
     * Destructor.
     */
    ~IdeController() override = default;

    static void initializeAvailableControllers();

    void plugin() override;

    void trigger(const Kernel::InterruptFrame &frame) override;

private:

    static const constexpr uint8_t PCI_SUBCLASS_IDE = 0x01;
    static const constexpr uint8_t CHANNELS_PER_CONTROLLER = 0x02;
    static const constexpr uint8_t DEVICES_PER_CHANNEL = 0x02;
    static const constexpr uint8_t ATAPI_CYLINDER_LOW_V1 = 0x14;
    static const constexpr uint8_t ATAPI_CYLINDER_HIGH_V1 = 0xeb;
    static const constexpr uint8_t ATAPI_CYLINDER_LOW_V2 = 0x69;
    static const constexpr uint8_t ATAPI_CYLINDER_HIGH_V2 = 0x96;
    static const constexpr uint16_t ATAPI_SECTOR_SIZE = 2048;
    static const constexpr uint16_t DEFAULT_BASE_ADDRESSES[DEVICES_PER_CHANNEL] = {0x01f0, 0x0170};
    static const constexpr uint16_t DEFAULT_CONTROL_BASE_ADDRESSES[DEVICES_PER_CHANNEL] = {0x03f4, 0x0374};
    static const constexpr uint32_t COMMAND_SET_WORD_COUNT = 6;
    static const constexpr uint32_t BUS_MASTER_CHANNEL_OFFSET = 0x08;
    static const constexpr uint32_t MAX_WAIT_ON_STATUS_RETRIES = 4095;
    static const constexpr uint32_t DMA_TIMEOUT = 30000;
    static const constexpr uint32_t PRD_END_OF_TRANSMISSION = 1 << 31;

    enum AddressType : uint8_t {
        CHS = 0x00,
        LBA28 = 0x01,
        LBA48 = 0x02,
    };

    enum DriveType : uint8_t {
        ATA = 0x00,
        ATAPI = 0x01,
        OTHER = 0x02,
    };

    enum AtapiType {
        SBC_3 = 0x00,       // Magnetic disk
        SBC = 0x01,         // Non CD-ROM optical disk
        MMC = 0x02,         // CD-ROM/DVD
        RBC = 0x04,         // Reduced block command device
        SCC_2 = 0x08,       // Storage array device (RAID)
        UNDEFINED = 0x10    // Undefined (error)
    };

    enum Command : uint8_t {
        NO_OPERATION = 0x00,
        DEVICE_RESET = 0x08,
        READ_PIO_LBA28 = 0x20,
        READ_PIO_LBA48 = 0x24,
        READ_DMA_LBA28 = 0xC8,
        READ_DMA_LBA48 = 0x25,
        WRITE_PIO_LBA28 = 0x30,
        WRITE_PIO_LBA48 = 0x34,
        WRITE_DMA_LBA28 = 0xCA,
        WRITE_DMA_LBA48 = 0x35,
        EXECUTE_DRIVE_DIAGNOSE = 0x90,
        FLUSH_CACHE = 0xE7,
        IDENTIFY_ATA_DRIVE = 0xEC,
        IDENTIFY_ATAPI_DRIVE = 0xA1,
        EJECT_MEDIA = 0xED,
        SET_FEATURES = 0xEF,
        ATAPI_READ = 0xA8,
        ATA_SEND_PACKET = 0xA0,
    };

    enum Status : uint8_t {
        NONE = 0x00,
        ERROR = 0x01,
        DATA_REQUEST = 0x08,
        DRIVE_READY = 0x40,
        BUSY = 0x80
    };

    enum DmaStatus : uint8_t {
        BUS_MASTER_ACTIVE = 0x01,
        DMA_ERROR = 0x02,
        INTERRUPT = 0x04,
        DMA_SUPPORTED_DRIVE_0 = 0x20,
        DMA_SUPPORTED_DRIVE_1 = 0x40,
        SIMPLEX = 0x80
    };

    enum DmaCommand : uint8_t {
        ENABLE = 0x01,
        DIRECTION = 0x08
    };

    enum IdentifyFieldOffset : uint8_t {
        DEVICE_TYPE = 0,
        CYLINDERS = 1,
        HEADS = 3,
        SECTORS = 6,
        SERIAL = 10,
        FIRMWARE = 23,
        MODEL = 27,
        CAPABILITIES = 49,
        VALID = 53,
        MAX_LBA = 60,
        DMA_MULTI = 63,
        MAJOR_VERSION = 80,
        MINOR_VERSION = 81,
        COMMAND_SETS = 82,
        UDMA_MODES = 88,
        MAX_LBA_EXT = 100
    };

    struct AtapiValues {
        uint8_t type;
        uint8_t packetLength;
    };

public:

    enum TransferMode : uint8_t{
        READ = 0x0,
        WRITE = 0x1,
    };

    struct DeviceInfo {
        uint8_t channel;                              // 0 (Primary Channel) or 1 (Secondary Channel)
        uint8_t drive;                                // 0 (Master Drive) or 1 (Slave Drive)
        DriveType type;                               // 0 (ATA) or 1 (ATAPI)
        uint16_t cylinders;                           // Number of logical cylinders of the drive
        uint16_t heads;                               // Number of logical heads of the drive
        uint16_t sectorsPerTrack;                     // Number of sectors per track of the drive
        uint16_t signature;                           // Drive Signature
        uint16_t capabilities;                        // Features
        uint8_t multiwordDma;                         // Supported versions of mutltiword dma
        uint8_t ultraDma;                             // Supported versions of ultra dma
        uint16_t commandSets[COMMAND_SET_WORD_COUNT]; // Supported command sets
        uint32_t maxSectorsLba48;                     // Size in Sectors LBA48
        uint32_t maxSectorsLba28;                     // Size in Sectors LBA28 / CHS
        char model[40];                               // Model as string
        char serial[10];                              // Serial number as string
        char firmware[4];                             // Firmware revision as string
        uint16_t majorVersion;                        // Major ATA Version supported
        uint16_t minorVersion;                        // Minor ATA Version supported
        AddressType addressing;                       // CHS (0), LBA28 (1), LBA48 (2)
        uint16_t sectorSize;                          // Sector size
        AtapiValues atapi;                            // In case of ATAPI, more information about the drive

        [[nodiscard]] bool supportsDma() const;
    };

    uint16_t performIO(const DeviceInfo &info, TransferMode mode, uint8_t *buffer, uint64_t startSector, uint32_t sectorCount);

private:

    struct CommandRegisters {
        explicit CommandRegisters(uint16_t baseAddress);
        
        Device::IoPort data;            // base + 0x00 (read/write)
        Device::IoPort error;           // base + 0x01 (read)
        Device::IoPort features;        // base + 0x01 (write)
        Device::IoPort sectorCount;     // base + 0x02 (read/write)
        Device::IoPort sectorNumber;    // base + 0x03 (read/write)
        Device::IoPort lbaLow;          // base + 0x03 (read/write)
        Device::IoPort cylinderLow;     // base + 0x04 (read/write)
        Device::IoPort lbaMid;          // base + 0x04 (read/write)
        Device::IoPort cylinderHigh;    // base + 0x05 (read/write)
        Device::IoPort lbaHigh;         // base + 0x05 (read/write)
        Device::IoPort driveHead;       // base + 0x06 (read/write)
        Device::IoPort status;          // base + 0x07 (read)
        Device::IoPort command;         // base + 0x07 (write)

    private:
        CommandRegisters();
    };
    
    struct ControlRegisters {
        ControlRegisters();
        explicit ControlRegisters(uint16_t baseAddress);
        
        Device::IoPort alternateStatus;   // base + 0x02 (read)
        Device::IoPort deviceControl;       // base + 0x02 (write);
        Device::IoPort deviceAddress;       // base + 0x03 (read)
    };
    
    struct DmaRegisters {
        DmaRegisters();
        explicit DmaRegisters(uint16_t baseAddress);
        
        Device::IoPort command;     // base + 0x00 (read/write)
        Device::IoPort reserved0;   // base + 0x01 (device specific)
        Device::IoPort status;      // base + 0x02 (read)
        Device::IoPort reserved1;   // base + 0x03 (device specific)
        Device::IoPort address;     // base + 0x04 (read/write)
    };
    
    struct ChannelRegisters {
        ChannelRegisters();
        ChannelRegisters(uint16_t commandBaseAddress, uint16_t controlBaseAddress, uint16_t dmaBaseAddress);

        bool receivedInterrupt{};             // Currently received interrupt
        uint8_t lastDeviceControl{};          // Saves current state of deviceControlRegister
        bool interruptsDisabled{};            // nIEN (No Interrupt);
        DriveType driveType[2]{};             // Initially found drive types;
        CommandRegisters command;             // Command Register Set IoPorts
        ControlRegisters control;             // Control Register Set IoPorts
        DmaRegisters dma;                     // DMA Bus Master Register Set IoPorts
    };

    void initializeDrives();

    bool resetDrive(uint8_t channel, uint8_t drive);

    bool identifyDrive(uint8_t channel, uint8_t drive);

    bool readAtaIdentity(uint8_t channel, uint16_t *buffer);

    bool readAtapiIdentity(uint8_t channel, uint16_t *buffer);

    static uint8_t getAtapiType(uint16_t signature);

    bool selectDrive(uint8_t channel, uint8_t drive, bool prepareLbaAccess = false, uint8_t lbaHead = 0);

    uint16_t determineSectorSize(const DeviceInfo &info);

    bool checkBounds(const DeviceInfo &info, uint64_t startSector, uint32_t sectorCount);

    void prepareIO(const DeviceInfo &info, uint64_t startSector, uint16_t sectorCount);

    uint16_t performProgrammedIO(const DeviceInfo &info, TransferMode mode, uint16_t *buffer, uint64_t startSector, uint16_t sectorCount);

    uint16_t performDmaIO(const DeviceInfo &info, TransferMode mode, uint16_t *buffer, uint64_t startSector, uint16_t sectorCount);

    static bool waitStatus(const IoPort &port, Status status, uint16_t retries = MAX_WAIT_ON_STATUS_RETRIES, bool logError = true);

    static bool waitBusy(const IoPort &port, uint16_t retries = MAX_WAIT_ON_STATUS_RETRIES, bool logError = true);

    static void copyByteSwappedString(const char *source, char *target, uint32_t length);

    ChannelRegisters channels[CHANNELS_PER_CONTROLLER]{};
    Util::Async::Spinlock ioLock;
    bool supportsDma = false;

    static Kernel::Logger log;
};

}


#endif
