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

    void (*callback)(UsbDev* dev, uint32_t status, void* data);
};

struct MouseDriver{
    struct UsbDriver super;
    struct MouseDev dev;
    void (*new_mouse_driver)(struct MouseDriver* driver, char* name, UsbDevice_ID* entry);
    MouseEvent (*constructEvent_mouse)(struct MouseDriver* driver, int index, uint16_t* value, uint16_t* type, int8_t* x, int8_t* y, int8_t* z);
    void (*trigger_mouse_event)(struct MouseDriver* driver, GenericEvent* event);
    void (*look_for_mouse_released)(struct MouseDriver* driver, uint8_t* mouse_code, int index, int8_t* x, int8_t* y, int8_t* z);
    void (*look_for_mouse_events)(struct MouseDriver* driver, uint8_t* mouse_code, int index, int8_t* x, int8_t* y, int8_t* z);
    uint16_t (*map_mouse_to_input_event)(struct MouseDriver* driver, int index);

    uint8_t look_up_buffer[MOUSE_LOOK_UP_SIZE];
    int8_t movement_buffer[MOUSE_LOOK_UP_SIZE]; // x,y,z
    uint8_t movement_submitted;
};

typedef struct MouseDriver MouseDriver;
typedef struct MouseDev MouseDev;

void new_mouse_driver(MouseDriver* driver, char* name, UsbDevice_ID* entry);

int16_t probe_mouse(UsbDev* dev, Interface* interface);
void disconnect_mouse(UsbDev* dev, Interface* interface);
void callback_mouse(UsbDev* dev, uint32_t status, void* data);

MouseEvent constructEvent_mouse(MouseDriver* driver, int index, uint16_t* value, uint16_t* type, int8_t* x, int8_t* y, int8_t* z);
void trigger_mouse_event(MouseDriver* driver, GenericEvent* event);
void look_for_mouse_released(MouseDriver* driver, uint8_t* mouse_code, int index, int8_t* x, int8_t* y, int8_t* z);
void look_for_mouse_events(MouseDriver* driver, uint8_t* mouse_code, int index, int8_t* x, int8_t* y, int8_t* z);

uint16_t map_mouse_to_input_event(MouseDriver* driver, int index);

extern uint8_t RAW_BUTTON_1;
extern uint8_t RAW_BUTTON_2;
extern uint8_t RAW_BUTTON_3;

#endif