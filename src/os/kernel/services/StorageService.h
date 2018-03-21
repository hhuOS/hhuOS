#ifndef __StorageService_include__
#define __StorageService_include__

#include "kernel/KernelService.h"
#include "kernel/KernelSymbols.h"
#include "devices/block/storage/StorageDevice.h"

#define STORAGE_SERVICE_DEBUG 0

#if STORAGE_SERVICE_DEBUG
#define     STORAGE_SERVICE_TRACE(...) printf("[STORAGE SERVICE] " __VA_ARGS__)
#else
#define     STORAGE_SERVICE_TRACE(...)
#endif

/**
 * Manages all storage devices.
 * When a devices is added or removed, an event is published via the event bus.
 */
class StorageService : public KernelService {

private:
    Util::HashMap<String, StorageDevice*> devices;

public:
    /**
     * Constructor.
     */
    StorageService() = default;

    /**
     * Copy-constructor.
     */
    StorageService(const StorageService &other) = delete;

    /**
     * Destructor.
     */
    ~StorageService() = default;

    /**
     * Get a storage device for its name.
     *
     * CAUTION: If the name does not match a registered device, the return value is nullptr.
     *
     * @param name The device's name
     * @return The device
     */
    StorageDevice *getDevice(const String &name);

    /**
     * Register a storage device.
     *
     * @param device The device
     */
    void registerDevice(StorageDevice *device);

    /**
     * Remove a device.
     *
     * @param name The device's name
     */
    void removeDevice(const String &name);

    /**
     * Search all devices for a device with system id 0x58.
     * This system id stands for FAT-formatted device, which should be mounted as root.
     *
     * CAUTION: If no device with system id 0x58 is found, the return value is nullptr.
     *
     * @return The device
     */
    StorageDevice *findRootDevice();

    static constexpr const char* SERVICE_NAME = "StorageService";
};

#endif