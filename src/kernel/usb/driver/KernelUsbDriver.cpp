#include "KernelUsbDriver.h"

#include "../../../device/usb/driver/UsbDriver.h"    

Kernel::Usb::Driver::KernelUsbDriver::KernelUsbDriver(Util::String name) : driver_name(name){
}

char* Kernel::Usb::Driver::KernelUsbDriver::getName(){
    return (char*)driver_name;
}