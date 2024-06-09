#ifndef MouseDriver__include
#define MouseDriver__include

#include "../UsbDriver.h"
#include "../../events/event/Event.h"
#include "../../events/event/hid/MouseEvent.h"

#define MOUSE_BUFFER_SIZE 4
#define MOUSE_LOOK_UP_SIZE 3

struct MouseDev{
    UsbDev* usb_dev;
    unsigned int endpoint_addr;
    void* buffer;
    unsigned int buffer_size;
    uint8_t priority;
    Interface* interface;
    uint16_t interval;
    UsbDriver* usb_driver;

    uint8_t look_up_buffer[MOUSE_LOOK_UP_SIZE];
    int8_t movement_buffer[MOUSE_LOOK_UP_SIZE]; // x,y,z
    uint8_t movement_submitted;

    void (*callback)(UsbDev* dev, Interface* interface, uint32_t status, void* data);
};

#define __INIT_MOUSE_DRIVER__(name, driver_name, entry) \
    __ENTRY__(name, constructEvent_mouse) = &constructEvent_mouse; \
    __ENTRY__(name, trigger_mouse_event) = &trigger_mouse_event; \
    __ENTRY__(name, look_for_mouse_released) = &look_for_mouse_released; \
    __ENTRY__(name, look_for_mouse_events) = &look_for_mouse_events; \
    __ENTRY__(name, map_mouse_to_input_event) = &map_mouse_to_input_event; \
    __ENTRY__(name, match_mouse) = &match_mouse; \
    __ENTRY__(name, get_free_mouse_dev) = &get_free_mouse_dev; \
    __ENTRY__(name, free_mouse_dev) = &free_mouse_dev; \
    \
    __SUPER__(name, probe) = &probe_mouse; \
    __SUPER__(name, disconnect) = &disconnect_mouse; \
    __SUPER__(name, new_usb_driver) = &new_usb_driver; \
    \
    __CALL_SUPER__(name->super, new_usb_driver, driver_name, entry)

struct MouseDriver{
    struct UsbDriver super;
    struct MouseDev dev[MAX_DEVICES_PER_USB_DRIVER];
    uint8_t mouse_map[MAX_DEVICES_PER_USB_DRIVER];

    void (*new_mouse_driver)(struct MouseDriver* driver, char* name, UsbDevice_ID* entry);
    MouseEvent (*constructEvent_mouse)(struct MouseDriver* driver, int index, uint16_t* value, uint16_t* type, int8_t* x, int8_t* y, int8_t* z);
    void (*trigger_mouse_event)(struct MouseDriver* driver, GenericEvent* event);
    void (*look_for_mouse_released)(struct MouseDriver* driver, struct MouseDev* mouse_dev, uint8_t* mouse_code, int index, int8_t* x, int8_t* y, int8_t* z);
    void (*look_for_mouse_events)(struct MouseDriver* driver, struct MouseDev* mouse_dev, uint8_t* mouse_code, int index, int8_t* x, int8_t* y, int8_t* z);
    uint16_t (*map_mouse_to_input_event)(struct MouseDriver* driver, int index);

    struct MouseDev* (*match_mouse)(struct MouseDriver* driver, UsbDev* dev);
    struct MouseDev* (*get_free_mouse_dev)(struct MouseDriver* driver);
    void (*free_mouse_dev)(struct MouseDriver* driver, struct MouseDev* mouse_dev);
};

typedef struct MouseDriver MouseDriver;
typedef struct MouseDev MouseDev;

void new_mouse_driver(MouseDriver* driver, char* name, UsbDevice_ID* entry);

extern uint8_t RAW_BUTTON_1;
extern uint8_t RAW_BUTTON_2;
extern uint8_t RAW_BUTTON_3;

#endif