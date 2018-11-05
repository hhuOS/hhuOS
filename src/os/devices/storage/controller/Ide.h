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

#ifndef __Ide_include__
#define __Ide_include__

#include <cstdint>
#include <devices/pci/Pci.h>
#include <devices/pci/PciDeviceDriver.h>

#define IDE_MAX_DEVICES 4

/**
 * Integrated Drive Electronics interface driver.
 *
 * @author Filip Krakowski
 */
class Ide : public PciDeviceDriver {

    private:

        static Logger &log;

        struct IdeChannelRegisters {
            uint16_t    base;    // I/O Base.
            uint16_t    ctrl;    // Control Base
            uint16_t    bmide;   // Bus Master IDE
            uint8_t     ni;      // No Interrupt
        } channels[2];

        struct ide_device {
            uint8_t     reserved;    // 0 (Empty) or 1 (This Drive really exists).
            uint8_t     channel;     // 0 (Primary Channel) or 1 (Secondary Channel).
            uint8_t     drive;       // 0 (Master Drive) or 1 (Slave Drive).
            uint16_t    type;        // 0: ATA, 1:ATAPI.
            uint8_t     serial[20];  // Serial Number
            uint16_t    signature;   // Drive Signature
            uint16_t    capabilities;// Features.
            uint32_t    commandSets; // Command Sets Supported.
            uint32_t    size;        // Size in Sectors.
            uint8_t     model[41];   // Model in string.
         } ideDevices[IDE_MAX_DEVICES];

        /* Status */
        static const uint8_t ATA_STS_BSY                = 0x80;    // Busy
        static const uint8_t ATA_STS_DRDY               = 0x40;    // Drive ready
        static const uint8_t ATA_STS_DF                 = 0x20;    // Drive write fault
        static const uint8_t ATA_STS_DSC                = 0x10;    // Drive seek complete
        static const uint8_t ATA_STS_DRQ                = 0x08;    // Data request ready
        static const uint8_t ATA_STS_CORR               = 0x04;    // Corrected data
        static const uint8_t ATA_STS_IDX                = 0x02;    // Inlex
        static const uint8_t ATA_STS_ERR                = 0x01;    // Error

        /* Error */
        static const uint8_t ATA_ERR_BBK                = 0x80;    // Bad sector
        static const uint8_t ATA_ERR_UNC                = 0x40;    // Uncorrectable data
        static const uint8_t ATA_ERR_MC                 = 0x20;    // No media
        static const uint8_t ATA_ERR_IDNF               = 0x10;    // ID mark not found
        static const uint8_t ATA_ERR_MCR                = 0x08;    // No media
        static const uint8_t ATA_ERR_ABRT               = 0x04;    // Command aborted
        static const uint8_t ATA_ERR_TK0NF              = 0x02;    // Track 0 not found
        static const uint8_t ATA_ERR_AMNF               = 0x01;    // No address mark

        /* Command */
        static const uint8_t ATA_CMD_READ_PIO           = 0x20;
        static const uint8_t ATA_CMD_READ_PIO_EXT       = 0x24;
        static const uint8_t ATA_CMD_READ_DMA           = 0xC8;
        static const uint8_t ATA_CMD_READ_DMA_EXT       = 0x25;
        static const uint8_t ATA_CMD_WRITE_PIO          = 0x30;
        static const uint8_t ATA_CMD_WRITE_PIO_EXT      = 0x34;
        static const uint8_t ATA_CMD_WRITE_DMA          = 0xCA;
        static const uint8_t ATA_CMD_WRITE_DMA_EXT      = 0x35;
        static const uint8_t ATA_CMD_CACHE_FLUSH        = 0xE7;
        static const uint8_t ATA_CMD_CACHE_FLUSH_EXT    = 0xEA;
        static const uint8_t ATA_CMD_PACKET             = 0xA0;
        static const uint8_t ATA_CMD_IDENTIFY_PACKET    = 0xA1;
        static const uint8_t ATA_CMD_IDENTIFY           = 0xEC;

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

        static const uint8_t ATA_REG_DATA               = 0x00;
        static const uint8_t ATA_REG_ERROR              = 0x01;
        static const uint8_t ATA_REG_FEATURES           = 0x01;
        static const uint8_t ATA_REG_SECCOUNT0          = 0x02;
        static const uint8_t ATA_REG_LBA0               = 0x03;
        static const uint8_t ATA_REG_LBA1               = 0x04;
        static const uint8_t ATA_REG_LBA2               = 0x05;
        static const uint8_t ATA_REG_HDDEVSEL           = 0x06;
        static const uint8_t ATA_REG_COMMAND            = 0x07;
        static const uint8_t ATA_REG_STATUS             = 0x07;
        static const uint8_t ATA_REG_SECCOUNT1          = 0x08;
        static const uint8_t ATA_REG_LBA3               = 0x09;
        static const uint8_t ATA_REG_LBA4               = 0x0A;
        static const uint8_t ATA_REG_LBA5               = 0x0B;
        static const uint8_t ATA_REG_CONTROL            = 0x0C;
        static const uint8_t ATA_REG_ALTSTATUS          = 0x0C;
        static const uint8_t ATA_REG_DEVADDRESS         = 0x0D;

        /* Channels */
        static const uint8_t ATA_PRIMARY                = 0x00;
        static const uint8_t ATA_SECONDARY              = 0x01;

        /* Directions */
        static const uint8_t ATA_READ                   = 0x00;
        static const uint8_t ATA_WRITE                  = 0x01;

        /* Device Types */
        static const uint8_t IDE_ATA                    = 0x00;
        static const uint8_t IDE_ATAPI                  = 0x01;

        static const uint8_t ATA_MASTER                 = 0x00;
        static const uint8_t ATA_SLAVE                  = 0x01;

        static const uint8_t ATA_DRV_MASTER             = 0xA0;
        static const uint8_t ATA_DRV_SLAVE              = 0xB0;

        static const uint8_t ATA_TIMEOUT                = 5;

        uint8_t readByte(uint8_t channel, uint8_t reg);
        void    writeByte(uint8_t channel, uint8_t reg, uint8_t value);

        void readBuffer(uint8_t channel, uint8_t reg, uint16_t *buf, uint32_t quads);

        uint8_t poll(uint8_t channel, bool advancedCheck);

        void selectDrive(uint8_t channel, uint8_t drive);

        void identifyDrive(uint8_t channel, uint8_t drive);

        void resetDrive(uint8_t channel);

        void waitBsy(uint8_t channel);

        void detect();

        void delay(uint32_t ns);

    public:

        Ide();

    PCI_DEVICE_DRIVER_IMPLEMENT_CREATE_INSTANCE(Ide);

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
};

#endif