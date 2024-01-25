// abstraction layer, to call 

#include "../../device/usb/driver/UsbDriver.h"
#include "../../device/usb/dev/data/UsbDev_Data.h"
#include "../../device/usb/events/listeners/EventListener.h"
#include "../service/UsbService.h"
#include "../system/System.h"
#include <cstdint>

// implement abstractions (usb)
int interface_register_driver(UsbDriver* driver){
    Kernel::UsbService& service = Kernel::System::getService<Kernel::UsbService>();
    return service.add_driver(driver);
}

int interface_deregister_driver(UsbDriver* driver){
    Kernel::UsbService& service = Kernel::System::getService<Kernel::UsbService>();
    return service.remove_driver(driver);
}

void interface_submit_bulk_transfer(Interface* itf, unsigned int pipe, uint8_t prio, void* data, 
                                    unsigned int len, callback_function callback){
    Kernel::UsbService& service = Kernel::System::getService<Kernel::UsbService>();
    service.submit_bulk_transfer(itf, pipe, prio, data, len, callback);
}

void interface_submit_interrupt_transfer(Interface* itf, unsigned int pipe, uint8_t prio, uint16_t interval, void* data, unsigned int len, callback_function callback){
    Kernel::UsbService& service = Kernel::System::getService<Kernel::UsbService>();
    service.submit_interrupt_transfer(itf, pipe, prio, interval, data, len, callback);
}

void interface_submit_control_transfer(Interface* itf, unsigned int pipe, uint8_t prio, void* data, uint8_t* setup, callback_function callback){
    Kernel::UsbService& service = Kernel::System::getService<Kernel::UsbService>();
    service.submit_control_transfer(itf, pipe, prio, data, setup, callback);
}

/*void interface_submit_iso_transfer(){
    Kernel::UsbService& service = Kernel::System::getService<Kernel::UsbService>();
}*/

int interface_register_callback(uint16_t register_type, event_callback event_c){
    Kernel::UsbService& service = Kernel::System::getService<Kernel::UsbService>();
    return service.register_callback(register_type, event_c);
}

int interface_deregister_callback(uint16_t register_type, event_callback event_c){
    Kernel::UsbService& service = Kernel::System::getService<Kernel::UsbService>();
    return service.deregister_callback(register_type, event_c);
}

int interface_register_listener(EventListener* listener){
    Kernel::UsbService& service = Kernel::System::getService<Kernel::UsbService>();
    return service.register_listener(listener);
}

int interface_deregister_listener(int id){
    Kernel::UsbService& service = Kernel::System::getService<Kernel::UsbService>();
    return service.deregister_listener(id);
}


