#include "KernelKbdDriver.h"
#include "KernelUsbDriver.h"
#include "../../system/System.h"
#include "../../service/MemoryService.h"
#include "../../../device/usb/events/listeners/hid/KeyBoardListener.h"
#include "../../../device/usb/events/listeners/EventListener.h"
#include "../../../lib/util/base/String.h"
#include <cstdint>

#include "../../../device/usb/driver/UsbDriver.h"    

Kernel::Usb::Driver::KernelUsbDriver::KernelUsbDriver(Util::String name) : driver_name(name){
}

char* Kernel::Usb::Driver::KernelUsbDriver::getName(){
    return (char*)driver_name;
}