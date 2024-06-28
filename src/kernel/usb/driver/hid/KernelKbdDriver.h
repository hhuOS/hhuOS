#ifndef KERNEL_USB_KBD_DRIVER__INCLUDE
#define KERNEL_USB_KBD_DRIVER__INCLUDE

#include "../KernelUsbDriver.h"

extern "C"{
#include "../../../../device/usb/driver/hid/KeyBoardDriver.h"    
}

namespace Kernel::Usb::Driver{

class KernelKbdDriver : public KernelUsbDriver{

public:

    explicit KernelKbdDriver(Util::String name);

    int initialize() override;

    int submit(uint8_t minor) override;

    void create_usb_dev_node() override;

private:

KeyBoardDriver* driver;

};

};

#ifdef __cplusplus
extern "C" {
#endif

void key_board_event_callback(void* e, void* buffer);

#ifdef __cplusplus
}
#endif

#endif