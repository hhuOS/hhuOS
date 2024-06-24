#include "KeyBoardDriver.h"
#include "lib/util/usb/input/InputEvents.h"
#include "../../dev/UsbDevice.h"
#include "../../events/EventDispatcher.h"
#include "../../events/event/Event.h"
#include "../../events/event/hid/KeyBoardEvent.h"
#include "../../include/UsbControllerInclude.h"
#include "../../include/UsbErrors.h"
#include "../UsbDriver.h"

static void callback_key_board(UsbDev* dev, Interface* interface, uint32_t status, void* data);
static int16_t probe_key_board(UsbDev* dev, Interface* interface);
static void disconnect_key_board(UsbDev* dev, Interface* interface);
static void look_for_events(KeyBoardDriver* driver, KeyBoardDev* kbd_dev, uint8_t* key_code, uint8_t* modifiers);
static void look_for_released(KeyBoardDriver* driver, KeyBoardDev* kbd_dev, uint8_t* key_codes, uint8_t* modifiers);
static KeyBoardEvent constructEvent_key_board(KeyBoardDriver* driver, uint8_t* key_code, uint8_t* modifiers, uint16_t* value, uint16_t* type);
static void trigger_key_board_event(KeyBoardDriver* key_board_driver, GenericEvent* event);
static uint16_t map_to_input_event_value(KeyBoardDriver* driver ,uint8_t raw_key, uint8_t modifiers);
static void trigger_led_report(KeyBoardDriver* driver, KeyBoardDev* kbd_dev);
static KeyBoardDev* get_free_kbd_dev(KeyBoardDriver* driver);
static void free_kbd_dev(KeyBoardDriver* driver, KeyBoardDev* kbd_dev);
static KeyBoardDev* match_kbd_dev(KeyBoardDriver* driver, UsbDev* dev);
static void key_board_report_callback(UsbDev* dev, Interface* interface, uint32_t status, void* data);

static KeyBoardDriver *internal_k_driver = 0;

static int16_t probe_key_board(UsbDev *dev, Interface *interface) {
  __REQUEST_OR_LEAVE__(KeyBoardDev, internal_k_driver, get_free_kbd_dev, kbd_dev);

  Endpoint **endpoints = interface->active_interface->endpoints;
  InterfaceDescriptor interface_desc =
      interface->active_interface->alternate_interface_desc;
  int e = interface_desc.bNumEndpoints;

  MemoryService_C* mem_service = __DEV_MEMORY(dev);
  // select only 1 endpoint
  __FOR_RANGE__(i, int , 0, e){
    // check for type , direction
    __DRIVER_DIRECTION_IN__(__STRUCT_CALL__(internal_k_driver, free_kbd_dev, kbd_dev),
      dev, endpoints[i]);
    __DRIVER_TYPE_INTERRUPT__(__STRUCT_CALL__(internal_k_driver, free_kbd_dev, kbd_dev),
      dev, endpoints[i]);

    if (!kbd_dev->endpoint_addr) {
      uint8_t *key_board_buffer = (uint8_t *)mem_service->mapIO(
          mem_service, sizeof(uint8_t) * KEYBOARD_BUFFER_SIZE, 1);

      kbd_dev->endpoint_addr = __STRUCT_CALL__(dev, __endpoint_number, endpoints[i]);
      kbd_dev->usb_dev = dev;
      kbd_dev->buffer = key_board_buffer;
      kbd_dev->buffer_size = KEYBOARD_BUFFER_SIZE;
      kbd_dev->priority = PRIORITY_8;
      kbd_dev->interface = interface;
      kbd_dev->interval = endpoints[i]->endpoint_desc.bInterval;
      return __RET_S__;
    }
  }
  return __RET_E__;
}

static void disconnect_key_board(UsbDev *dev, Interface *interface) {}

void new_key_board_driver(KeyBoardDriver *key_board_driver, char *name,
                          UsbDevice_ID *entry) {
  for (int i = 0; i < MAX_DEVICES_PER_USB_DRIVER; i++) {
    key_board_driver->key_board_map[i] = 0;
    key_board_driver->dev[i].usb_dev = 0;
    key_board_driver->dev[i].endpoint_addr = 0;
    key_board_driver->dev[i].buffer = 0;
    key_board_driver->dev[i].buffer_size = 0;
    key_board_driver->dev[i].priority = 0;
    key_board_driver->dev[i].interface = 0;
    key_board_driver->dev[i].interval = 0;
    key_board_driver->dev[i].callback = &callback_key_board;
    key_board_driver->dev[i].usb_driver = (UsbDriver *)key_board_driver;

    key_board_driver->dev[i].current_led_state =
        SCROLL_LOCK_MASK | NUM_LOCK_MASK;
    key_board_driver->dev[i].current_modifier_state = 0;
    key_board_driver->dev[i].current_modifier_count = 0;

    for (int j = 0; j < KEYBOARD_LOOKUP_SIZE; j++) {
      key_board_driver->dev[i].look_up_buffer[j] = 0;
    }
  }
  internal_k_driver = key_board_driver;

  __INIT_KBD_DRIVER__(key_board_driver, name, entry);
}

static KeyBoardDev *get_free_kbd_dev(KeyBoardDriver *driver) {
  __GET_FREE_DEV__(KeyBoardDev, driver->dev, driver->key_board_map);
}

static void free_kbd_dev(KeyBoardDriver *driver, KeyBoardDev *kbd_dev) {
  __FREE_DEV__(kbd_dev, driver->dev, driver->key_board_map);
}

static KeyBoardDev *match_kbd_dev(KeyBoardDriver *driver, UsbDev *dev) {
  __MATCH_DEV__(KeyBoardDev, driver->dev, usb_dev, dev);
}

static void callback_key_board(UsbDev *dev, Interface* interface, uint32_t status, void *data) {
  if (status & E_TRANSFER)
    return;

  KeyBoardDev *kbd_dev =
      internal_k_driver->match_kbd_dev(internal_k_driver, dev);

  if (kbd_dev == 0)
    return;

  uint8_t prev_state = kbd_dev->current_led_state;

  uint8_t *buffer = (uint8_t *)data;
  uint8_t modifiers = *buffer;
  uint8_t key_code_1 = *(buffer + 2);
  uint8_t key_code_2 = *(buffer + 3);
  uint8_t key_code_3 = *(buffer + 4);
  uint8_t key_code_4 = *(buffer + 5);
  uint8_t key_code_5 = *(buffer + 6);
  uint8_t key_code_6 = *(buffer + 7);

  internal_k_driver->look_for_events(internal_k_driver, kbd_dev, &key_code_1,
                                     &modifiers);
  internal_k_driver->look_for_events(internal_k_driver, kbd_dev, &key_code_2,
                                     &modifiers);
  internal_k_driver->look_for_events(internal_k_driver, kbd_dev, &key_code_3,
                                     &modifiers);
  internal_k_driver->look_for_events(internal_k_driver, kbd_dev, &key_code_4,
                                     &modifiers);
  internal_k_driver->look_for_events(internal_k_driver, kbd_dev, &key_code_5,
                                     &modifiers);
  internal_k_driver->look_for_events(internal_k_driver, kbd_dev, &key_code_6,
                                     &modifiers);

  internal_k_driver->look_for_released(internal_k_driver, kbd_dev, buffer + 2,
                                       &modifiers);

  if (prev_state != kbd_dev->current_led_state) {
    internal_k_driver->trigger_led_report(internal_k_driver, kbd_dev);
  }

  if (kbd_dev->current_modifier_count == 5) {
    kbd_dev->current_modifier_state = 0;
    kbd_dev->current_modifier_count = 0;
  }

  kbd_dev->current_modifier_count++;

  kbd_dev->look_up_buffer[0] = key_code_1;
  kbd_dev->look_up_buffer[1] = key_code_2;
  kbd_dev->look_up_buffer[2] = key_code_3;
  kbd_dev->look_up_buffer[3] = key_code_4;
  kbd_dev->look_up_buffer[4] = key_code_5;
  kbd_dev->look_up_buffer[5] = key_code_6;
}

static void look_for_events(KeyBoardDriver *k_driver, KeyBoardDev *kbd_dev,
                     uint8_t *key_code, uint8_t *modifiers) {
  uint16_t event_type = KEY_EVENT;
  uint16_t event_value;
  KeyBoardEvent event;

  if ((*key_code == 0x00) && (*modifiers == 0x00))
    return;

  /*if ((*key_code == 0x00) && (*modifiers != 0x00) &&
      (k_driver->current_modifier_state == 0)) {
    event_value = KEY_PRESSED;
    event =
        constructEvent_key_board(key_code, modifiers, &event_value,
                                 &event_type); // just for passing the modifiers
  }*/
  else {
    kbd_dev->current_modifier_state = 1;

    if (*key_code == RAW_KEY_CAPS_LOCK) {
      kbd_dev->current_led_state ^= CAPS_LOCK_MASK;
    }
    /*else if(*key_code & RAW_KEY_NUM){
        k_driver->current_led_state ^= NUM_LOCK_MASK;
    }*/
    else if (*key_code == RAW_KEY_SCROLL) {
      kbd_dev->current_led_state ^= SCROLL_LOCK_MASK;
    }

    int is_pressed = 0;
    for (int i = 0; i < 6; i++) {
      if (*(kbd_dev->look_up_buffer + i) == *key_code) {
        is_pressed = 1;
      }
    }
    if (is_pressed) {
      event_value = KEY_HOLD;
      event = k_driver->constructEvent_key_board(k_driver, key_code, modifiers,
                                                 &event_value,
                                                 &event_type); // hold event
    } else if (!is_pressed) {
      event_value = KEY_PRESSED;
      event = k_driver->constructEvent_key_board(k_driver, key_code, modifiers,
                                                 &event_value,
                                                 &event_type); // pressed event
    }
  }
  k_driver->trigger_key_board_event(k_driver, (GenericEvent *)&event);
}

static void look_for_released(KeyBoardDriver *k_driver, KeyBoardDev *kbd_dev,
                       uint8_t *key_codes, uint8_t *modifiers) {
  uint16_t event_type = KEY_EVENT;
  uint16_t event_value = KEY_RELEASED;
  KeyBoardEvent event;

  for (int k = 0; k < 6; k++) {
    if (*(kbd_dev->look_up_buffer + k) == 0)
      continue;
    int is_released = 1;
    for (int i = 0; i < 6; i++) {
      if (*(key_codes + i) == *(kbd_dev->look_up_buffer + k)) {
        is_released = 0;
      }
    }
    if (is_released) {
      event = k_driver->constructEvent_key_board(
          k_driver, kbd_dev->look_up_buffer + k, modifiers, &event_value,
          &event_type);
      k_driver->trigger_key_board_event(k_driver, (GenericEvent *)&event);
    }
  }
}

static void key_board_report_callback(UsbDev *dev, Interface* interface, uint32_t status, void *data) {
  MemoryService_C* m = __DEV_MEMORY(dev);
  m->unmap(m, (uint32_t)(uintptr_t)(uint8_t *)data);
}

// this method should only be callable inside the interrupt context !!!
static void trigger_led_report(KeyBoardDriver *driver, KeyBoardDev *kbd_dev) {
  uint8_t led_state = kbd_dev->current_led_state;

  UsbDev *kbd = kbd_dev->usb_dev;

  MemoryService_C* m = __DEV_MEMORY(kbd);

  uint8_t *data_buffer = (uint8_t *)m->mapIO(m, sizeof(uint8_t), 1);
  *data_buffer = led_state;

  kbd->set_report(kbd, kbd_dev->interface, REQUEST_OUTPUT, data_buffer, 1,
                  driver->key_board_report_callback);
}

// [a-zA-Z] will not be covered by us
// meaning reader component has to look at modifiers
static KeyBoardEvent constructEvent_key_board(KeyBoardDriver *k_driver,
                                       uint8_t *key_code, uint8_t *modifiers,
                                       uint16_t *value, uint16_t *type) {

  KeyBoardEvent event;
  uint16_t input_key =
      k_driver->map_to_input_event_value(k_driver, *key_code, *modifiers);

  event.modifiers = *modifiers;
  event.super.event_value = *value;
  event.super.event_type = *type;
  event.super.event_code = input_key;

  return event;
}

static void trigger_key_board_event(KeyBoardDriver *key_board_driver,
                             GenericEvent *event) {
  ((UsbDriver *)key_board_driver)
      ->dispatcher->publish_event(((UsbDriver *)key_board_driver)->dispatcher,
                                  event,
                                  ((UsbDriver *)key_board_driver)->listener_id);
}

static uint16_t map_to_input_event_value(KeyBoardDriver *k_driver, uint8_t raw_key,
                                  uint8_t modifiers) {
  uint16_t input_key = 0;

  if (raw_key == RAW_KEY_A) {
    input_key = KEY_A;
  } else if (raw_key == RAW_KEY_B) {
    input_key = KEY_B;
  } else if (raw_key == RAW_KEY_C) {
    input_key = KEY_C;
  } else if (raw_key == RAW_KEY_D) {
    input_key = KEY_D;
  } else if (raw_key == RAW_KEY_E) {
    input_key = KEY_E;
  } else if (raw_key == RAW_KEY_F) {
    input_key = KEY_F;
  } else if (raw_key == RAW_KEY_G) {
    input_key = KEY_G;
  } else if (raw_key == RAW_KEY_H) {
    input_key = KEY_H;
  } else if (raw_key == RAW_KEY_I) {
    input_key = KEY_I;
  } else if (raw_key == RAW_KEY_J) {
    input_key = KEY_J;
  } else if (raw_key == RAW_KEY_K) {
    input_key = KEY_K;
  } else if (raw_key == RAW_KEY_L) {
    input_key = KEY_L;
  } else if (raw_key == RAW_KEY_M) {
    input_key = KEY_M;
  } else if (raw_key == RAW_KEY_N) {
    input_key = KEY_N;
  } else if (raw_key == RAW_KEY_O) {
    input_key = KEY_O;
  } else if (raw_key == RAW_KEY_P) {
    input_key = KEY_P;
  } else if (raw_key == RAW_KEY_Q) {
    if (modifiers == LEFT_SHIFT_MASK || modifiers == RIGHT_SHIFT_MASK) {
      input_key = KEY_AROBE_SIGN;
    } else {
      input_key = KEY_Q;
    }
  } else if (raw_key == RAW_KEY_R) {
    input_key = KEY_R;
  } else if (raw_key == RAW_KEY_S) {
    input_key = KEY_S;
  } else if (raw_key == RAW_KEY_T) {
    input_key = KEY_T;
  } else if (raw_key == RAW_KEY_U) {
    input_key = KEY_U;
  } else if (raw_key == RAW_KEY_V) {
    input_key = KEY_V;
  } else if (raw_key == RAW_KEY_W) {
    input_key = KEY_W;
  } else if (raw_key == RAW_KEY_X) {
    input_key = KEY_X;
  } else if (raw_key == RAW_KEY_Y) {
    input_key = KEY_Y;
  } else if (raw_key == RAW_KEY_Z) {
    input_key = KEY_Z;
  } else if (raw_key == RAW_KEY_1) {
    if (modifiers == LEFT_SHIFT_MASK || modifiers == RIGHT_SHIFT_MASK) {
      input_key = KEY_EXCLAMTATION_MARK_SIGN;
    } else {
      input_key = KEY_1;
    }
  } else if (raw_key == RAW_KEY_2) {
    if (modifiers == LEFT_SHIFT_MASK || modifiers == RIGHT_SHIFT_MASK) {
      input_key = KEY_DOUBLE_QUOTE_SIGN;
    } else {
      input_key = KEY_2;
    }
  } else if (raw_key == RAW_KEY_3) {
    if (modifiers == LEFT_SHIFT_MASK || modifiers == RIGHT_SHIFT_MASK) {
      input_key = KEY_LAW_SIGN;
    } else {
      input_key = KEY_3;
    }
  } else if (raw_key == RAW_KEY_4) {
    if (modifiers == LEFT_SHIFT_MASK || modifiers == RIGHT_SHIFT_MASK) {
      input_key = KEY_DOLLAR_SIGN;
    } else {
      input_key = KEY_4;
    }
  } else if (raw_key == RAW_KEY_5) {
    if (modifiers == LEFT_SHIFT_MASK || modifiers == RIGHT_SHIFT_MASK) {
      input_key = KEY_PERCENTAGE;
    } else {
      input_key = KEY_5;
    }
  } else if (raw_key == RAW_KEY_6) {
    if (modifiers == LEFT_SHIFT_MASK || modifiers == RIGHT_SHIFT_MASK) {
      input_key = KEY_AND;
    } else {
      input_key = KEY_6;
    }
  } else if (raw_key == RAW_KEY_7) {
    if (modifiers == LEFT_SHIFT_MASK || modifiers == RIGHT_SHIFT_MASK) {
      input_key = KEY_SLASH;
    } else if (modifiers == LEFT_CONTROL_MASK ||
               modifiers == RIGHT_CONTROL_MASK) {
      input_key = KEY_CURLY_BRACKET_RIGHT;
    } else {
      input_key = KEY_7;
    }
  } else if (raw_key == RAW_KEY_8) {
    if (modifiers == LEFT_SHIFT_MASK || modifiers == RIGHT_SHIFT_MASK) {
      input_key = KEY_PARENTHESIS_RIGHT;
    } else if (modifiers == LEFT_CONTROL_MASK ||
               modifiers == RIGHT_CONTROL_MASK) {
      input_key = KEY_SQUARE_BRACKET_RIGHT;
    } else {
      input_key = KEY_8;
    }
  } else if (raw_key == RAW_KEY_9) {
    if (modifiers == LEFT_SHIFT_MASK || modifiers == RIGHT_SHIFT_MASK) {
      input_key = KEY_PARENTHESIS_LEFT;
    } else if (modifiers == LEFT_CONTROL_MASK ||
               modifiers == RIGHT_CONTROL_MASK) {
      input_key = KEY_SQUARE_BRACKET_LEFT;
    } else {
      input_key = KEY_9;
    }
  } else if (raw_key == RAW_KEY_0) {
    if (modifiers == LEFT_SHIFT_MASK || modifiers == RIGHT_SHIFT_MASK) {
      input_key = KEY_EQUALS_SIGN;
    } else if (modifiers == LEFT_CONTROL_MASK ||
               modifiers == RIGHT_CONTROL_MASK) {
      input_key = KEY_CURLY_BRACKET_LEFT;
    } else {
      input_key = KEY_0;
    }
  } else if (raw_key == RAW_KEY_ẞ) {
    if (modifiers == LEFT_SHIFT_MASK || modifiers == RIGHT_SHIFT_MASK) {
      input_key = KEY_QUESTION_MARK;
    } else if (modifiers == LEFT_CONTROL_MASK ||
               modifiers == RIGHT_CONTROL_MASK) {
      input_key = KEY_BACKSLASH;
    } else {
      input_key = KEY_ẞ;
    }
  } else if (raw_key == RAW_KEY_RIGHT_SINGLE) {
    if (modifiers == LEFT_SHIFT_MASK || modifiers == RIGHT_SHIFT_MASK) {
      input_key = KEY_LEFT_SINGLE;
    } else {
      input_key = KEY_RIGHT_SINGLE;
    }
  } else if (raw_key == RAW_KEY_DEL) {
    input_key = KEY_DEL;
  } else if (raw_key == RAW_KEY_TAB) {
    input_key = KEY_TAB;
  } else if (raw_key == RAW_KEY_SPACE) {
    input_key = KEY_SPACE;
  } else if (raw_key == RAW_KEY_ENTER) {
    input_key = KEY_ENTER;
  } else if (raw_key == RAW_KEY_ESC) {
    input_key = KEY_ESC;
  } else if (raw_key == RAW_KEY_U_DOUBLE_POINTS) {
    input_key = KEY_U_DOUBLE_POINTS;
  } else if (raw_key == RAW_KEY_PLUS) {
    if (modifiers == LEFT_SHIFT_MASK || modifiers == RIGHT_SHIFT_MASK) {
      input_key = KEY_STAR;
    } else if (modifiers == LEFT_CONTROL_MASK ||
               modifiers == RIGHT_CONTROL_MASK) {
      input_key = KEY_TILDE;
    } else {
      input_key = KEY_PLUS;
    }
  } else if (raw_key == RAW_KEY_LOWER_THAN) {
    if (modifiers == LEFT_SHIFT_MASK || modifiers == RIGHT_SHIFT_MASK) {
      input_key = KEY_GREATER_THEN;
    } else if (modifiers == LEFT_CONTROL_MASK ||
               modifiers == RIGHT_CONTROL_MASK) {
      input_key = KEY_PIPE;
    } else {
      input_key = KEY_LOWER_THAN;
    }
  } else if (raw_key == RAW_KEY_A_DOUBLE_POINTS) {
    input_key = KEY_A_DOUBLE_POINTS;
  } else if (raw_key == RAW_KEY_HASH_TAG) {
    if (modifiers == LEFT_SHIFT_MASK || modifiers == RIGHT_SHIFT_MASK) {
      input_key = KEY_QUOTE_SIGN;
    } else {
      input_key = KEY_HASH_TAG;
    }
  } else if (raw_key == RAW_KEY_EXP_SIGN) {
    if (modifiers == LEFT_SHIFT_MASK || modifiers == RIGHT_SHIFT_MASK) {
      input_key = KEY_CIRLCE_SIGN;
    } else {
      input_key = KEY_EXP_SIGN;
    }
  } else if (raw_key == RAW_KEY_KOMMA) {
    if (modifiers == LEFT_SHIFT_MASK || modifiers == RIGHT_SHIFT_MASK) {
      input_key = KEY_SEMICOLON;
    } else {
      input_key = KEY_KOMMA;
    }
  } else if (raw_key == RAW_KEY_POINT) {
    if (modifiers == LEFT_SHIFT_MASK || modifiers == RIGHT_SHIFT_MASK) {
      input_key = KEY_DOUBLE_POINTS;
    } else {
      input_key = KEY_POINT;
    }
  } else if (raw_key == RAW_KEY_MINUS) {
    if (modifiers == LEFT_SHIFT_MASK || modifiers == RIGHT_SHIFT_MASK) {
      input_key = KEY_UNDER_SCORE;
    } else {
      input_key = KEY_MINUS;
    }
  } else if (raw_key == RAW_KEY_CAPS_LOCK) {
    input_key = KEY_CAPS_LOCK;
  } else if (raw_key == RAW_KEY_F1) {
    input_key = KEY_F1;
  } else if (raw_key == RAW_KEY_F2) {
    input_key = KEY_F2;
  } else if (raw_key == RAW_KEY_F3) {
    input_key = KEY_F3;
  } else if (raw_key == RAW_KEY_F4) {
    input_key = KEY_F4;
  } else if (raw_key == RAW_KEY_F5) {
    input_key = KEY_F5;
  } else if (raw_key == RAW_KEY_F6) {
    input_key = KEY_F6;
  } else if (raw_key == RAW_KEY_F7) {
    input_key = KEY_F7;
  } else if (raw_key == RAW_KEY_F8) {
    input_key = KEY_F8;
  } else if (raw_key == RAW_KEY_F9) {
    input_key = KEY_F9;
  } else if (raw_key == RAW_KEY_F10) {
    input_key = KEY_F10;
  } else if (raw_key == RAW_KEY_F11) {
    input_key = KEY_F11;
  } else if (raw_key == RAW_KEY_F12) {
    input_key = KEY_F12;
  } else if (raw_key == RAW_KEY_INSERT) {
    input_key = KEY_INSERT;
  } else if (raw_key == RAW_KEY_SCROLL) {
    input_key = KEY_SCROLL;
  } else if (raw_key == RAW_KEY_PG_UP) {
    input_key = KEY_PG_UP;
  } else if (raw_key == RAW_KEY_PAD_DEL) {
    input_key = KEY_PAD_DEL;
  } else if (raw_key == RAW_KEY_END) {
    input_key = KEY_END;
  } else if (raw_key == RAW_KEY_PG_DOWN) {
    input_key = KEY_PG_UP;
  } else if (raw_key == RAW_KEY_RIGHT_ARROW) {
    input_key = KEY_RIGHT_ARROW;
  } else if (raw_key == RAW_KEY_LEFT_ARROW) {
    input_key = KEY_LEFT_ARROW;
  } else if (raw_key == RAW_KEY_DOWN_ARROW) {
    input_key = KEY_DOWN_ARROW;
  } else if (raw_key == RAW_KEY_UP_ARROW) {
    input_key = KEY_UP_ARROW;
  }

  return input_key;
}

uint16_t RAW_KEY_A = 4;
uint16_t RAW_KEY_B = 5;
uint16_t RAW_KEY_C = 6;
uint16_t RAW_KEY_D = 7;
uint16_t RAW_KEY_E = 8;
uint16_t RAW_KEY_F = 9;
uint16_t RAW_KEY_G = 10;
uint16_t RAW_KEY_H = 11;
uint16_t RAW_KEY_I = 12;
uint16_t RAW_KEY_J = 13;
uint16_t RAW_KEY_K = 14;
uint16_t RAW_KEY_L = 15;
uint16_t RAW_KEY_M = 16;
uint16_t RAW_KEY_N = 17;
uint16_t RAW_KEY_O = 18;
uint16_t RAW_KEY_P = 19;
uint16_t RAW_KEY_Q = 20;
uint16_t RAW_KEY_R = 21;
uint16_t RAW_KEY_S = 22;
uint16_t RAW_KEY_T = 23;
uint16_t RAW_KEY_U = 24;
uint16_t RAW_KEY_V = 25;
uint16_t RAW_KEY_W = 26;
uint16_t RAW_KEY_X = 27;
uint16_t RAW_KEY_Y = 29;
uint16_t RAW_KEY_Z = 28;
uint16_t RAW_KEY_1 = 30;
uint16_t RAW_KEY_2 = 31;
uint16_t RAW_KEY_3 = 32;
uint16_t RAW_KEY_4 = 33;
uint16_t RAW_KEY_5 = 34;
uint16_t RAW_KEY_6 = 35;
uint16_t RAW_KEY_7 = 36;
uint16_t RAW_KEY_8 = 37;
uint16_t RAW_KEY_9 = 38;
uint16_t RAW_KEY_0 = 39;
uint16_t RAW_KEY_ENTER = 40;
uint16_t RAW_KEY_ESC = 41;
uint16_t RAW_KEY_DEL = 42;
uint16_t RAW_KEY_TAB = 43;
uint16_t RAW_KEY_SPACE = 44;
uint16_t RAW_KEY_ẞ = 45;
uint16_t RAW_KEY_RIGHT_SINGLE = 46;
uint16_t RAW_KEY_U_DOUBLE_POINTS = 47;
uint16_t RAW_KEY_PLUS = 48;
uint16_t RAW_KEY_LOWER_THAN = 49; // is this really true ?
uint16_t RAW_KEY_A_DOUBLE_POINTS = 51;
uint16_t RAW_KEY_HASH_TAG = 52;
uint16_t RAW_KEY_EXP_SIGN = 53;
uint16_t RAW_KEY_KOMMA = 54;
uint16_t RAW_KEY_POINT = 55;
uint16_t RAW_KEY_MINUS = 56;
uint16_t RAW_KEY_CAPS_LOCK = 57;
uint16_t RAW_KEY_F1 = 58;
uint16_t RAW_KEY_F2 = 59;
uint16_t RAW_KEY_F3 = 60;
uint16_t RAW_KEY_F4 = 61;
uint16_t RAW_KEY_F5 = 62;
uint16_t RAW_KEY_F6 = 63;
uint16_t RAW_KEY_F7 = 64;
uint16_t RAW_KEY_F8 = 65;
uint16_t RAW_KEY_F9 = 66;
uint16_t RAW_KEY_F10 = 67;
uint16_t RAW_KEY_F11 = 68;
uint16_t RAW_KEY_F12 = 69;
uint16_t RAW_KEY_INSERT = 73;
uint16_t RAW_KEY_SCROLL = 74;
uint16_t RAW_KEY_PG_UP = 75;
uint16_t RAW_KEY_PAD_DEL = 76;
uint16_t RAW_KEY_END = 77;
uint16_t RAW_KEY_PG_DOWN = 78;
uint16_t RAW_KEY_RIGHT_ARROW = 79;
uint16_t RAW_KEY_LEFT_ARROW = 80;
uint16_t RAW_KEY_DOWN_ARROW = 81;
uint16_t RAW_KEY_UP_ARROW = 82;