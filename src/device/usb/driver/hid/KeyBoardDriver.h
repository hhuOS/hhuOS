#ifndef KeyBoardDriver__include
#define KeyBoardDriver__include

#include "../UsbDriver.h"
#include "stdint.h"
#include "../../events/event/hid/KeyBoardEvent.h"
#include "../../events/event/Event.h"

#define KEYBOARD_BUFFER_SIZE 8
#define KEYBOARD_LOOKUP_SIZE 6

struct KeyBoardDev{
    UsbDev* usb_dev;
    unsigned int endpoint_addr;
    void* buffer;
    unsigned int buffer_size;
    uint8_t priority;
    Interface* interface;
    uint16_t interval;
    UsbDriver* usb_driver;

    uint8_t look_up_buffer[KEYBOARD_LOOKUP_SIZE];
    uint8_t current_led_state;
    uint8_t current_modifier_state;
    uint8_t current_modifier_count; 

    void (*callback)(UsbDev* dev, Interface* interface, uint32_t status, void* data);
};

#define __INIT_KBD_DRIVER__(name, driver_name, entry) \
    __ENTRY__(name, look_for_events) = &look_for_events; \
    __ENTRY__(name, look_for_released) = &look_for_released; \
    __ENTRY__(name, constructEvent_key_board) = &constructEvent_key_board; \
    __ENTRY__(name, trigger_key_board_event) = &trigger_key_board_event; \
    __ENTRY__(name, map_to_input_event_value) = &map_to_input_event_value; \
    __ENTRY__(name, trigger_led_report) = &trigger_led_report; \
    __ENTRY__(name, key_board_report_callback) = &key_board_report_callback; \
    __ENTRY__(name, get_free_kbd_dev) = &get_free_kbd_dev; \
    __ENTRY__(name, free_kbd_dev) = &free_kbd_dev; \
    __ENTRY__(name, match_kbd_dev) = &match_kbd_dev; \
    \
    __SUPER__(name, probe) = &probe_key_board; \
    __SUPER__(name, disconnect) = &disconnect_key_board; \
    __SUPER__(name, new_usb_driver) = &new_usb_driver; \
    \
    __CALL_SUPER__(name->super, new_usb_driver, driver_name, entry)

struct KeyBoardDriver{
    struct UsbDriver super;
    struct KeyBoardDev dev[MAX_DEVICES_PER_USB_DRIVER];
    uint8_t key_board_map[MAX_DEVICES_PER_USB_DRIVER];

    void (*new_key_board_driver)(struct KeyBoardDriver* driver, char* name, struct UsbDevice_ID* entry);

    void (*look_for_events)(struct KeyBoardDriver* driver, struct KeyBoardDev* kbd_dev, uint8_t* key_code, uint8_t* modifiers);
    void (*look_for_released)(struct KeyBoardDriver* driver, struct KeyBoardDev* kbd_dev, uint8_t* key_codes, uint8_t* modifiers);
    KeyBoardEvent (*constructEvent_key_board)(struct KeyBoardDriver* driver ,uint8_t* key_code, uint8_t* modifiers, uint16_t* value, uint16_t* type);
    void (*trigger_key_board_event)(struct KeyBoardDriver* key_board_driver, GenericEvent* event);
    uint16_t (*map_to_input_event_value)(struct KeyBoardDriver* driver, uint8_t raw_key, uint8_t modifiers);
    void (*trigger_led_report)(struct KeyBoardDriver* driver, struct KeyBoardDev* kbd_dev);

    void (*key_board_report_callback)(UsbDev* dev, Interface* interface, uint32_t status, void* data);

    struct KeyBoardDev* (*get_free_kbd_dev)(struct KeyBoardDriver* driver);
    void (*free_kbd_dev)(struct KeyBoardDriver* driver, struct KeyBoardDev* kbd_dev);
    struct KeyBoardDev* (*match_kbd_dev)(struct KeyBoardDriver* driver, UsbDev* dev);
};

typedef struct KeyBoardDriver KeyBoardDriver;
typedef struct KeyBoardDev KeyBoardDev;

void new_key_board_driver(KeyBoardDriver* key_board_driver, char* name, struct UsbDevice_ID* entry);

// list of keycodes without modifiers -> in input.h list all events explicit
// + add field to keyboard event for retrieving modifiers -> input.h should contain masks for them

extern uint16_t RAW_KEY_A;
extern uint16_t RAW_KEY_B;
extern uint16_t RAW_KEY_C;
extern uint16_t RAW_KEY_D;
extern uint16_t RAW_KEY_E;
extern uint16_t RAW_KEY_F;
extern uint16_t RAW_KEY_G;
extern uint16_t RAW_KEY_H;
extern uint16_t RAW_KEY_I;
extern uint16_t RAW_KEY_J;
extern uint16_t RAW_KEY_K;
extern uint16_t RAW_KEY_L;
extern uint16_t RAW_KEY_M;
extern uint16_t RAW_KEY_N;
extern uint16_t RAW_KEY_O;
extern uint16_t RAW_KEY_P;
extern uint16_t RAW_KEY_Q;
extern uint16_t RAW_KEY_R;
extern uint16_t RAW_KEY_S;
extern uint16_t RAW_KEY_T;
extern uint16_t RAW_KEY_U;
extern uint16_t RAW_KEY_V;
extern uint16_t RAW_KEY_W;
extern uint16_t RAW_KEY_X;
extern uint16_t RAW_KEY_Y;
extern uint16_t RAW_KEY_Z;
extern uint16_t RAW_KEY_1;
extern uint16_t RAW_KEY_2;
extern uint16_t RAW_KEY_3;
extern uint16_t RAW_KEY_4;
extern uint16_t RAW_KEY_5;
extern uint16_t RAW_KEY_6;
extern uint16_t RAW_KEY_7;
extern uint16_t RAW_KEY_8;
extern uint16_t RAW_KEY_9;
extern uint16_t RAW_KEY_0;
extern uint16_t RAW_KEY_ENTER;
extern uint16_t RAW_KEY_ESC;
extern uint16_t RAW_KEY_DEL;
extern uint16_t RAW_KEY_TAB;
extern uint16_t RAW_KEY_SPACE;
extern uint16_t RAW_KEY_ẞ;
extern uint16_t RAW_KEY_RIGHT_SINGLE;
extern uint16_t RAW_KEY_U_DOUBLE_POINTS;
extern uint16_t RAW_KEY_PLUS;
extern uint16_t RAW_KEY_LOWER_THAN; // is this really true ?
extern uint16_t RAW_KEY_A_DOUBLE_POINTS;
extern uint16_t RAW_KEY_HASH_TAG;
extern uint16_t RAW_KEY_EXP_SIGN;
extern uint16_t RAW_KEY_KOMMA;
extern uint16_t RAW_KEY_POINT;
extern uint16_t RAW_KEY_MINUS;
extern uint16_t RAW_KEY_CAPS_LOCK;
extern uint16_t RAW_KEY_F1;
extern uint16_t RAW_KEY_F2;
extern uint16_t RAW_KEY_F3;
extern uint16_t RAW_KEY_F4;
extern uint16_t RAW_KEY_F5;
extern uint16_t RAW_KEY_F6;
extern uint16_t RAW_KEY_F7;
extern uint16_t RAW_KEY_F8;
extern uint16_t RAW_KEY_F9;
extern uint16_t RAW_KEY_F10;
extern uint16_t RAW_KEY_F11;
extern uint16_t RAW_KEY_F12;
extern uint16_t RAW_KEY_INSERT;
extern uint16_t RAW_KEY_SCROLL;
extern uint16_t RAW_KEY_PG_UP;
extern uint16_t RAW_KEY_PAD_DEL;
extern uint16_t RAW_KEY_END;
extern uint16_t RAW_KEY_PG_DOWN;
extern uint16_t RAW_KEY_RIGHT_ARROW;
extern uint16_t RAW_KEY_LEFT_ARROW;
extern uint16_t RAW_KEY_DOWN_ARROW;
extern uint16_t RAW_KEY_UP_ARROW;

#define LEFT_CONTROL_MASK 0x01
#define LEFT_SHIFT_MASK 0x02
#define LEFT_ALT_MASK 0x04
#define LEFT_GUI_MASK 0x08

#define RIGHT_CONTROL_MASK 0x10
#define RIGHT_SHIFT_MASK 0x20
#define RIGHT_ALT_MASK 0x40
#define RIGHT_GUI_MASK 0x80

#define NUM_LOCK_MASK 0x01
#define CAPS_LOCK_MASK 0x02
#define SCROLL_LOCK_MASK 0x04

#endif