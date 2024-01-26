#include "KeyBoardDriver.h"
#include "../../events/event/Event.h"
#include "../../../../lib/util/io/key/InputEvents.h"
#include "../../events/event/hid/KeyBoardEvent.h"
#include "../../include/UsbControllerInclude.h"
#include "../../dev/UsbDevice.h"
#include "../../include/UsbErrors.h"
#include "../../events/EventDispatcher.h"
#include "../UsbDriver.h"

static KeyBoardDriver* internal_k_driver = 0;

static void new_usb_driver(UsbDriver *usb_driver, char *name,
                           UsbDevice_ID *entry);

static void new_usb_driver(UsbDriver *usb_driver, char *name,
                           UsbDevice_ID *entry) {
  usb_driver->probe = &probe_key_board;
  usb_driver->disconnect = &disconnect_key_board;

  usb_driver->name = name;
  usb_driver->entry = entry;
  usb_driver->head.l_e = 0;
  usb_driver->l_e.l_e = 0;
}

int16_t probe_key_board(UsbDev *dev, Interface *interface) {

  Endpoint **endpoints = interface->active_interface->endpoints;
  InterfaceDescriptor interface_desc =
      interface->active_interface->alternate_interface_desc;
  // check if we should support that interface !!!
  int e = interface_desc.bNumEndpoints;

  MemoryService_C *mem_service =
      (MemoryService_C *)container_of(dev->mem_service, MemoryService_C, super);

  // select only 1 endpoint
  for (int i = 0; i < e; i++) {
    // check for type , direction
    if (!(endpoints[i]->endpoint_desc.bEndpointAddress & DIRECTION_IN)) {
      continue;
    }
    if (!(endpoints[i]->endpoint_desc.bmAttributes & TRANSFER_TYPE_INTERRUPT)) {
      continue;
    }
    if (!internal_k_driver->dev.endpoint_addr) {
      uint8_t *key_board_buffer = (uint8_t *)mem_service->mapIO(
          mem_service, sizeof(uint8_t) * KEYBOARD_BUFFER_SIZE, 1);

      internal_k_driver->dev.endpoint_addr =
          endpoints[i]->endpoint_desc.bEndpointAddress & ENDPOINT_MASK;
      internal_k_driver->dev.usb_dev = dev;
      internal_k_driver->dev.buffer = key_board_buffer;
      internal_k_driver->dev.buffer_size = KEYBOARD_BUFFER_SIZE;
      internal_k_driver->dev.priority = PRIORITY_8;
      internal_k_driver->dev.interface = interface;
      internal_k_driver->dev.interval = endpoints[i]->endpoint_desc.bInterval;
      return 1;
    }
  }
  
  return -1;
}

void disconnect_key_board(UsbDev *dev, Interface *interface) {}

void new_key_board_driver(KeyBoardDriver *key_board_driver, char *name,
                          UsbDevice_ID *entry) {
  for (int i = 0; i < KEYBOARD_LOOKUP_SIZE; i++) {
    key_board_driver->look_up_buffer[i] = 0;
  }

  key_board_driver->current_led_state = SCROLL_LOCK_MASK | NUM_LOCK_MASK;
  key_board_driver->current_modifier_state = 0;
  key_board_driver->current_modifier_count = 0;
  key_board_driver->look_for_events = &look_for_events;
  key_board_driver->look_for_released = &look_for_released;
  key_board_driver->constructEvent_key_board = &constructEvent_key_board;
  key_board_driver->trigger_key_board_event = &trigger_key_board_event;
  key_board_driver->map_to_input_event_value = &map_to_input_event_value;
  key_board_driver->trigger_led_report = &trigger_led_report;
  key_board_driver->key_board_report_callback = &key_board_report_callback;

  internal_k_driver = key_board_driver;

  key_board_driver->dev.usb_dev = 0;
  key_board_driver->dev.endpoint_addr = 0;
  key_board_driver->dev.buffer = 0;
  key_board_driver->dev.buffer_size = 0;
  key_board_driver->dev.priority = 0;
  key_board_driver->dev.interface = 0;
  key_board_driver->dev.interval = 0;
  key_board_driver->dev.callback = &callback_key_board;
  key_board_driver->dev.usb_driver = (UsbDriver*)key_board_driver;


  internal_k_driver->super.new_usb_driver = &new_usb_driver;
  internal_k_driver->super.new_usb_driver(&key_board_driver->super, name, entry);
}

void callback_key_board(UsbDev* dev, uint32_t status, void *data) {
  if (status & E_TRANSFER)
    return;

  uint8_t prev_state = internal_k_driver->current_led_state;

  uint8_t *buffer = (uint8_t *)data;
  uint8_t modifiers = *buffer;
  uint8_t key_code_1 = *(buffer + 2);
  uint8_t key_code_2 = *(buffer + 3);
  uint8_t key_code_3 = *(buffer + 4);
  uint8_t key_code_4 = *(buffer + 5);
  uint8_t key_code_5 = *(buffer + 6);
  uint8_t key_code_6 = *(buffer + 7);

  internal_k_driver->look_for_events(internal_k_driver, &key_code_1, &modifiers);
  internal_k_driver->look_for_events(internal_k_driver, &key_code_2, &modifiers);
  internal_k_driver->look_for_events(internal_k_driver, &key_code_3, &modifiers);
  internal_k_driver->look_for_events(internal_k_driver, &key_code_4, &modifiers);
  internal_k_driver->look_for_events(internal_k_driver, &key_code_5, &modifiers);
  internal_k_driver->look_for_events(internal_k_driver, &key_code_6, &modifiers);

  internal_k_driver->look_for_released(internal_k_driver, buffer + 2, &modifiers);

  if (prev_state != internal_k_driver->current_led_state) {
    internal_k_driver->trigger_led_report(internal_k_driver);
  }

  if (internal_k_driver->current_modifier_count == 5) {
    internal_k_driver->current_modifier_state = 0;
    internal_k_driver->current_modifier_count = 0;
  }

  internal_k_driver->current_modifier_count++;

  internal_k_driver->look_up_buffer[0] = key_code_1;
  internal_k_driver->look_up_buffer[1] = key_code_2;
  internal_k_driver->look_up_buffer[2] = key_code_3;
  internal_k_driver->look_up_buffer[3] = key_code_4;
  internal_k_driver->look_up_buffer[4] = key_code_5;
  internal_k_driver->look_up_buffer[5] = key_code_6;
}

void look_for_events(KeyBoardDriver *k_driver, uint8_t *key_code, uint8_t *modifiers) {
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
    k_driver->current_modifier_state = 1;

    if (*key_code == RAW_KEY_CAPS_LOCK) {
      k_driver->current_led_state ^= CAPS_LOCK_MASK;
    }
    /*else if(*key_code & RAW_KEY_NUM){
        k_driver->current_led_state ^= NUM_LOCK_MASK;
    }*/
    else if (*key_code == RAW_KEY_SCROLL) {
      k_driver->current_led_state ^= SCROLL_LOCK_MASK;
    }

    int is_pressed = 0;
    for (int i = 0; i < 6; i++) {
      if (*(k_driver->look_up_buffer + i) == *key_code) {
        is_pressed = 1;
      }
    }
    if (is_pressed) {
      event_value = KEY_HOLD;
      event = k_driver->constructEvent_key_board(k_driver, key_code, modifiers, &event_value,
                                       &event_type); // hold event
    } else if (!is_pressed) {
      event_value = KEY_PRESSED;
      event = k_driver->constructEvent_key_board(k_driver, key_code, modifiers, &event_value,
                                       &event_type); // pressed event
    }
  }
  k_driver->trigger_key_board_event(k_driver, (GenericEvent *)&event);
}

void look_for_released(KeyBoardDriver* k_driver, uint8_t *key_codes,
                       uint8_t *modifiers) {
  uint16_t event_type = KEY_EVENT;
  uint16_t event_value = KEY_RELEASED;
  KeyBoardEvent event;

  for (int k = 0; k < 6; k++) {
    if (*(k_driver->look_up_buffer + k) == 0)
      continue;
    int is_released = 1;
    for (int i = 0; i < 6; i++) {
      if (*(key_codes + i) == *(k_driver->look_up_buffer + k)) {
        is_released = 0;
      }
    }
    if (is_released) {
      event = k_driver->constructEvent_key_board(k_driver, k_driver->look_up_buffer + k, modifiers,
                                       &event_value, &event_type);
      k_driver->trigger_key_board_event(k_driver , (GenericEvent *)&event);
    }
  }
}

void key_board_report_callback(UsbDev *dev, uint32_t status, void *data) {
  MemoryService_C *m =
      (MemoryService_C *)container_of(dev->mem_service, MemoryService_C, super);
  m->unmap(m, (uint32_t)(uintptr_t)(uint8_t*)data);
}

// this method should only be callable inside the interrupt context !!!
void trigger_led_report(KeyBoardDriver *driver) {
  uint8_t led_state = driver->current_led_state;

  UsbDev *kbd = driver->dev.usb_dev;

  MemoryService_C *m =
      (MemoryService_C *)container_of(kbd->mem_service, MemoryService_C, super);

  uint8_t *data_buffer = (uint8_t *)m->mapIO(m, sizeof(uint8_t), 1);
  *data_buffer = led_state;

  kbd->set_report(kbd, driver->dev.interface, REQUEST_OUTPUT, data_buffer, 1,
                  driver->key_board_report_callback);
}

// [a-zA-Z] will not be covered by us
// meaning reader component has to look at modifiers
KeyBoardEvent constructEvent_key_board(KeyBoardDriver* k_driver, uint8_t *key_code, uint8_t *modifiers,
                                       uint16_t *value, uint16_t *type) {

  KeyBoardEvent event;
  uint16_t input_key = k_driver->map_to_input_event_value(k_driver ,*key_code, *modifiers);

  event.modifiers = *modifiers;
  event.super.event_value = *value;
  event.super.event_type = *type;
  event.super.event_code = input_key;

  return event;
}

void trigger_key_board_event(KeyBoardDriver *key_board_driver, GenericEvent *event) {
  ((UsbDriver*)key_board_driver)->dispatcher->publish_event(
      ((UsbDriver*)key_board_driver)->dispatcher, event, ((UsbDriver*)key_board_driver)->listener_id);
}

uint16_t map_to_input_event_value(KeyBoardDriver* k_driver, uint8_t raw_key, uint8_t modifiers) {
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