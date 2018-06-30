#ifndef __AhciDevice_include__
#define __AhciDevice_include__

#include "StorageDevice.h"
#include "lib/String.h"

#include <cstdint>
#include <devices/block/Ahci.h>

/**
 * Implementation of StorageDevice for a Device, that is controlled by an AhciController (see devices/block/Ahci.h).
 */
class AhciDevice : public StorageDevice {

private:
    Ahci &controller;
    uint8_t ahciDiskNumber;

    Ahci::AhciDeviceInfo deviceInfo;

public:
    /**
     * Constructor.
     *
     * @param controller A reference to the controller, that controls this device.
     * @param ahciDiskNumber The slot, that the device takes in the controller's device-array.
     * @param name The name
     */
    AhciDevice(Ahci &controller, uint8_t ahciDiskNumber, String name);

    /**
     * Copy-constructor.
     */
    AhciDevice(AhciDevice &copy) = delete;

    /**
     * Destructor.
     */
    ~AhciDevice() override = default;

    /**
     * Overriding function from StorageDevice.
     */
    String getDeviceName() override;

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