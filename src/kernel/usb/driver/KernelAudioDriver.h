#ifndef KERNEL_USB_AUDIO_DRIVER__INCLUDE
#define KERNEL_USB_AUDIO_DRIVER__INCLUDE

#include "KernelUsbDriver.h"

extern "C"{
#include "../../../device/usb/driver/audio/AudioDeviceDriver.h"
}

namespace Kernel::Usb::Driver{

class KernelAudioDriver : public KernelUsbDriver{

public:

    explicit KernelAudioDriver(Util::String name);

    int initialize() override;

    int submit(uint8_t minor) override;

    void create_usb_dev_node() override;

private:

    AudioDriver* driver;

};

};

#endif