#include <cstdint>

extern "C"{
#include "../../device/usb/driver/UsbDriver.h"
#include "../../device/usb/events/listeners/EventListener.h"
#include "../../device/usb/dev/data/UsbDev_Data.h"
}

// define abstraction layer to call specific service routine (usb)!
int interface_register_callback(uint16_t register_type, event_callback event_c, void* buffer);
int interface_deregister_callback(uint16_t register_type, event_callback event_c);

// :for kernel
int interface_register_driver(UsbDriver* driver);
int interface_deregister_driver(UsbDriver* driver);

int interface_register_listener(EventListener* listener);
int interface_deregister_listener(int id);

void interface_submit_bulk_transfer(Interface* interface, unsigned int pipe, uint8_t prio, void* data, unsigned int len, callback_function callback);
void interface_submit_interrupt_transfer(Interface* interface, unsigned int pipe, uint8_t prio, uint16_t interval, void* data, unsigned int len, callback_function callback);
void interface_submit_control_transfer(Interface* interface, unsigned int pipe, uint8_t prio, void* data, uint8_t* setup, callback_function callback);
//void interface_submit_iso_transfer(Interface* interface, unsigned int pipe, uint8_t prio);