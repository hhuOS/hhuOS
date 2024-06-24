#ifndef KERNEL_USB_MASS_STORAGE_DRIVER__INCLUDE
#define KERNEL_USB_MASS_STORAGE_DRIVER__INCLUDE

#include "KernelUsbDriver.h"

extern "C"{
#include "../../../device/usb/driver/storage/MassStorageDriver.h"    
}

namespace Kernel::Usb::Driver{

class KernelMassStorageDriver : public KernelUsbDriver{

public:

    explicit KernelMassStorageDriver(Util::String name);

    int initialize() override;

    int submit(uint8_t minor) override;

    void create_usb_dev_node() override;

    bool control(uint32_t request, const Util::Array<uint32_t>& parameters, uint8_t minor);

    uint64_t readData(uint8_t *targetBuffer, uint64_t start_lba, uint64_t msd_data, uint8_t minor);

    uint64_t writeData(const uint8_t *sourceBuffer, uint64_t start_lba, uint64_t msd_data, uint8_t minor);

private:
    // additional msd params are in command interface 
    bool get_requests(const Util::Array<uint32_t> &parameters,
        MassStorageDev* msd_dev, uint32_t (*get_call)(MassStorageDriver* msd_driver, 
        MassStorageDev* msd_dev, uint8_t volume));
    MassStorageDriver* driver;
};

};

#endif