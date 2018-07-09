#ifndef __VirtualDiskDrive_include__
#define __VirtualDiskDrive_include__

#include "StorageDevice.h"
#include "lib/String.h"

#include <cstdint>

/**
 * Implementation of StorageDevice for a virtual device, that exists only in RAM.
 *
 * @author Fabian Ruhland
 * @date 2018
 */
class VirtualDiskDrive : public StorageDevice {

private:
    static uint32_t nameCounter;

    uint32_t sectorSize;
    uint32_t sectorCount;
    String name;

    uint8_t *buffer = nullptr;

public:
    /**
     * Constructor.
     *
     * @param sectorSize The virtual size of a sector on the virtual disk
     * @param sectorCount The amount of sectors, that the virtual disk shall consist of
     * @param name The name
     */
    VirtualDiskDrive(uint32_t sectorSize, uint32_t sectorCount);

    /**
     * Copy-constructor.
     */
    VirtualDiskDrive(VirtualDiskDrive &copy) = delete;

    /**
     * Destructor.
     */
    ~VirtualDiskDrive() override;

    /**
     * Overriding function from StorageDevice.
     */
    String getHardwareName() override;

    /**
     * Overriding function from StorageDevice.
     */
    uint32_t getSectorSize() override;

    /**
     * Overriding function from StorageDevice.
     */
    uint64_t getSectorCount() override;

    /**
     * Overriding function from StorageDevice.
     */
    bool read(uint8_t *buff, uint32_t sector, uint32_t count) override;

    /**
     * Overriding function from StorageDevice.
     */
    bool write(const uint8_t *buff, uint32_t sector, uint32_t count) override;
};

#endif