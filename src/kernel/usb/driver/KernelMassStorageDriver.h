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

    int submit() override;

    bool control(uint32_t request, const Util::Array<uint32_t>& parameters);

    uint64_t readData(uint8_t *targetBuffer, uint64_t start_lba, uint32_t blocks, uint8_t volume);

    uint64_t writeData(const uint8_t *sourceBuffer, uint64_t start_lba, uint32_t blocks, uint8_t volume);

private:
    enum MSD_Params : uint8_t{
        GET_SIZE,
        GET_BLOCK_NUM,
        GET_BLOCK_LEN,
        GET_VOLUMES,
        GET_CAPACITIES_FOUND,
        GET_INQUIRY,
        GET_CAPACITY,
        GET_SENSE, // not implemented -> too lazy :)
        GET_READ_CAPACITY
    };

    // additional msd params are in command interface 

    MassStorageDriver* driver;
};

};

#endif