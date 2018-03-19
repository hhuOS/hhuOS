#ifndef __Partition_include__
#define __Partition_include__

#include "StorageDevice.h"
#include "lib/String.h"

#include <cstdint>

/**
 * Implementation of StorageDevice for a partition, that is located on a StorageDevice itself.
 */
class Partition : public StorageDevice {

private:
    StorageDevice *parentDevice;
    uint32_t startSector;
    uint32_t sectorCount;
    uint8_t systemId;

public:
    /**
     * Constructor.
     *
     * @param parentDevice Pointer to the device, that the partition is located on
     * @param startSector The partitions first sector on the parent device
     * @param sectorCount The amount of sectors, that the partition consists of
     * @param systemId The system id
     * @param name The name (e.g. 'hdd0p1', 'usb1p3', ...)
     */
    Partition(StorageDevice *parentDevice, uint32_t startSector, uint32_t sectorCount, uint8_t systemId, String &name);

    /**
     * Copy-constructor.
     */
    Partition(Partition &copy) = delete;

    /**
     * Desctructor.
     */
    ~Partition() override = default;

    /**
     * Overriding function from StorageDevice.
     */
    Util::ArrayList<StorageDevice::PartitionInfo>& readPartitionTable() override;

    /**
     * Overriding function from StorageDevice.
     */
    uint32_t writePartition(uint8_t partNumber, bool active, uint8_t systemId, uint32_t startSector, uint32_t sectorCount) override;

    /**
     * Overriding function from StorageDevice.
     */
    uint32_t deletePartition(uint8_t partNumber) override;

    /**
     * Overriding function from StorageDevice.
     */
    uint32_t createPartitionTable() override;

    /**
     * Overriding function from StorageDevice.
     */
    uint8_t getSystemId() override;

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