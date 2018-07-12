#ifndef HHUOS_PCIDEVICEDRIVER_H
#define HHUOS_PCIDEVICEDRIVER_H

#define PCI_DEVICE_DRIVER_IMPLEMENT_CREATE_INSTANCE(TYPE) \
        PciDeviceDriver *createInstance() const override { return new TYPE(); }

#include <cstdint>
#include "Pci.h"

class PciDeviceDriver {

public:

    enum SetupMethod {
        BY_ID,
        BY_CLASS
    };

    PciDeviceDriver() = default;

    PciDeviceDriver(const PciDeviceDriver &copy) = default;

    virtual ~PciDeviceDriver() = default;

    virtual PciDeviceDriver *createInstance() const  = 0;

    virtual uint16_t getVendorId() const {
        return 0;
    };

    virtual uint16_t getDeviceId() const {
        return 0;
    };

    virtual uint8_t getBaseClass() const {
        return 0;
    };

    virtual uint8_t getSubClass() const {
        return 0;
    };

    virtual SetupMethod getSetupMethod() const = 0;

    virtual void setup(const Pci::Device &device) = 0;
};

#endif
