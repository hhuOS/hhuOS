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
 * The ISO9660 driver is based on a bachelor's thesis, written by Moritz Riefer.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-morie103
 */

#ifndef HHUOS_ISODRIVER_H
#define HHUOS_ISODRIVER_H

#include <stdint.h>

#include "filesystem/PhysicalDriver.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/base/String.h"
#include "lib/util/io/file/File.h"
#include "lib/util/reflection/Prototype.h"

namespace Device {
namespace Storage {
class StorageDevice;
}  // namespace Storage
}  // namespace Device

namespace Filesystem::Iso {

class IsoDriver : public PhysicalDriver {

public:

    enum FileFlags {
        HIDDEN = 1 << 0,
        DIRECTORY = 1 << 1,
        ASSOCIATED = 1 << 2,
        EXTENDED_ATTRIBUTES = 1 << 3,
        PERMISSIONS_IN_EXTENDED_ATTRIBUTES = 1 << 4,
        NOT_FINAL_RECORD = 1 << 7
    };

    struct DateTime {
        uint8_t year; // Number of years since 1900
        uint8_t month;
        uint8_t dayOfMonth;
        uint8_t hour;
        uint8_t minute;
        uint8_t second;
        uint8_t gmtOffset; // In 15 minute intervals
    } __attribute__((packed));

    struct DirectoryRecord {
        uint8_t recordLength;
        uint8_t extendedAttributeRecordLength;
        uint32_t extentLbaLSB;
        uint32_t extentLbaMSB;
        uint32_t dataLengthLSB;
        uint32_t dataLengthMSB;
        DateTime recordDateTime;
        uint8_t flags;
        uint8_t fileUnitSize;
        uint8_t interleaveGap;
        uint16_t volumeSequenceNumberLSB;
        uint16_t volumeSequenceNumberMSB;
        uint8_t identifierLength;
        char identifier[];

        [[nodiscard]] bool isDirectory() const;

        [[nodiscard]] Util::String getName() const;

        [[nodiscard]] DirectoryRecord* createCopy() const;
    } __attribute__((packed));

    /**
     * Default Constructor.
     */
    IsoDriver() = default;

    /**
     * Copy Constructor.
     */
    IsoDriver(const IsoDriver &other) = delete;

    /**
     * Assignment operator.
     */
    IsoDriver &operator=(const IsoDriver &other) = delete;

    /**
     * Destructor.
     */
    ~IsoDriver() override = default;

    PROTOTYPE_IMPLEMENT_CLONE(IsoDriver);

    PROTOTYPE_IMPLEMENT_GET_CLASS_NAME("Filesystem::Iso::IsoDriver")

    /**
     * Overriding function from Driver.
     */
    bool mount(Device::Storage::StorageDevice &device) override;

    /**
     * Overriding function from Driver.
     */
    bool createFilesystem(Device::Storage::StorageDevice &device) override;

    /**
     * Overriding function from Driver.
     */
    Node* getNode(const Util::String &path) override;

    /**
     * Overriding function from Driver.
     */
    bool createNode(const Util::String &path, Util::Io::File::Type type) override;

    /**
     * Overriding function from Driver.
     */
    bool deleteNode(const Util::String &path) override;

private:

    enum VolumeDescriptorType : uint8_t {
        BOOT_RECORD = 0,
        PRIMARY_VOLUME_DESCRIPTOR = 1,
        SUPPLEMENTARY_VOLUME_DESCRIPTOR = 2,
        VOLUME_PARTITION_DESCRIPTOR = 3,
        VOLUME_DESCRIPTOR_SET_TERMINATOR = 255
    };

    enum DirectoryOffsets {
        DIRECTORY_RECORD_LENGTH = 0,
        EXTENDED_ATTRIBUTE_RECORD_LENGTH = 1,
        EXTENT_LBA = 2,
        SIZE_OF_EXTENT = 10,
        DATE_TIME = 18,
        FILE_FLAGS = 25,
        VOLUME_SEQUENCE_NUMBER = 28,
        FILE_IDENTIFIER_LENGTH = 32,
        FILE_IDENTIFIER = 33
    };

    struct VolumeDescriptorHeader {
        VolumeDescriptorType type;
        char identifier[5];
        uint8_t version;
    } __attribute__((packed));

    struct PrimaryVolumeDescriptor {
        VolumeDescriptorHeader header;          // Header prepends each volume descriptor
        uint8_t unused1;
        char systemIdentifier[32];              // Name of the system that can act upon sectors 0x00-0x0F for the volume
        char volumeIdentifier[32];              // Identification of this volume
        uint8_t unused2[8];
        uint32_t volumeSpaceSizeLSB;            // Number of logical blocks of this volume (LSB encoded)
        uint32_t volumeSpaceSizeMSB;            // Number of logical blocks of this volume (MSB encoded)
        uint8_t unused3[32];
        uint16_t volumeSetSizeLSB;              // Number of disks (LSB encoded)
        uint16_t volumeSetSizeMSB;              // Number of disks (MSB encoded)
        uint16_t volumeSequenceNumberLSB;       // Number of this disk in the volume set (LSB encoded)
        uint16_t volumeSequenceNumberMSB;       // Number of this disk in the volume set (MSB encoded)
        uint16_t logicalBlockSizeLSB;           // Size of a logical block in bytes (LSB encoded)
        uint16_t logicalBlockSizeMSB;           // Size of a logical block in bytes (MSB encoded)
        uint32_t pathTableSizeLSB;              // size of the path table in bytes (LSB encoded)
        uint32_t pathTableSizeMSB;              // size of the path table in bytes (MSB encoded)
        uint32_t pathTableLbaLSB;               // LBA location of the little endian encoded path table
        uint32_t optionalPathTableLbaLSB;       // LBA location of the little endian encoded optional path table
        uint32_t pathTableLbaMSB;               // LBA location of the big endian encoded path table
        uint32_t optionalPathTableLbaMSB;       // LBA location of the big endian encoded optional path table
        char rootDirectoryEntry[34];            // Directory record for root directory
        char volumeSetIdentifier[128];          // Identifier of the volume set of which this volume is a member
        char publisherIdentifier[128];          // Volume publisher(extended information if first byte=0x5F)
        char dataPreparerIdentifier[128];       // Identifier of the person(s) who prepared the data(extended information if first byte=0x5F)
        char applicationIdentifier[128];        // Identifies how the data are recorded(extended information if first byte=0x5F)
        char copyrightFileIdentifier[37];       // Filename of file in the root directory that contains copyright information
        char abstractFileIdentifier[37];        // Filename of a file in the root directory that contains abstract information
        char bibliographicFileIdentifier[37];   // Filename of a file in the root directory that contains bibliographic information
        char volumeCreationDateTime[17];        // Date and time of when the volume was created
        char volumeModificationDateTime[17];    // Date and time of when the volume was modified
        char volumeExpirationDateTime[17];      // Date and time of when this volume is considered obsolete(if not specified: never)
        char volumeEffectiveDateTime[17];       // Date and time of when this volume may be used(if not specified: immediately)
        uint8_t fileStructureVersion;           // Version of directory records and path table
        uint8_t unused4;
        uint8_t applicationUsed[512];
        uint8_t reserved[653];
    } __attribute__((packed));

    struct PathTableEntry {
        uint8_t directoryIdentifierLength;              // Length of the directory identifier
        uint8_t extendedAttributeRecordLength;          // Length of the extended attribute record
        uint32_t extentLba;                             // LBA address of Extent
        uint16_t parentDirectoryIndex;                  // Path Table entry index for parent directory
        char directoryIdentifier[];                     // Directory identifier

        [[nodiscard]] uint32_t getLength() const;

        [[nodiscard]] Util::String getName() const;

        [[nodiscard]] PathTableEntry* createCopy() const;

        bool operator!=(const PathTableEntry &other) const;
    } __attribute__((packed));

    bool initializePrimaryVolumeDescriptor();

    bool initializePathTable();

private:

    Device::Storage::StorageDevice *device = nullptr;
    PrimaryVolumeDescriptor primaryVolumeDescriptor{};
    Util::ArrayList<PathTableEntry*> pathTableEntryList = Util::ArrayList<PathTableEntry*>();

    static const constexpr uint16_t VOLUME_DESCRIPTORS_START_SECTOR = 16;
};

}

#endif
