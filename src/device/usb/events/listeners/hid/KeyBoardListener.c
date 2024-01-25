#include "KeyBoardListener.h"
#include "../../event/Event.h"
#include "../../../../../lib/util/io/key/InputEvents.h"
#include "../../event/hid/KeyBoardEvent.h"
#include "../../../include/UsbControllerInclude.h"
#include "../../../include/UsbInterface.h"
#include "../../../interfaces/LoggerInterface.h"
#include "../../../utility/Utils.h"
#include "../EventListener.h"
#include "stdint.h"

// move as top method, since all listeners are just traversing the list_elements
void key_board_call(EventListener *listener, GenericEvent *event) {
  list_element *l_e = listener->head.l_e;
  //listener->listener_mutex->acquire_c(listener->listener_mutex);
  while (l_e != (void *)0) {
    Event_Callback *e_c =
        (Event_Callback *)container_of(l_e, Event_Callback, l_e);
    KeyBoardEvent* k_evt = container_of(event, KeyBoardEvent, super);    
    e_c->callback((void*)k_evt);
    l_e = l_e->l_e;
  }
  //listener->listener_mutex->release_c(listener->listener_mutex);
}

/*void extract_event(GenericEvent *event) {
  KeyBoardEvent *k_event =
      (KeyBoardEvent *)container_of(event, KeyBoardEvent, super);
  uint8_t modifiers = k_event->modifiers;
  uint8_t event_value = event->event_value;
  // uint8_t event_type  = event->event_type;
  uint8_t event_code = event->event_code;

  // just for purpose only read [a-zA-Z]
  char key = 0;
  // char *event_description;
  int display = 0;

  if (event_code == KEY_CAPS_LOCK && event_value == KEY_PRESSED) {
    caps_lock_on ^= 1;
  }

  if (event_code == KEY_A) {
    if ((modifiers & LEFT_SHIFT_MASK) || (modifiers & RIGHT_SHIFT_MASK) ||
        (caps_lock_on == 1)) {
      key = 'A';
    } else
      key = 'a';
  } else if (event_code == KEY_B) {
    if ((modifiers & LEFT_SHIFT_MASK) || (modifiers & RIGHT_SHIFT_MASK) ||
        caps_lock_on) {
      key = 'B';
    } else
      key = 'b';
  } else if (event_code == KEY_C) {
    if ((modifiers & LEFT_SHIFT_MASK) || (modifiers & RIGHT_SHIFT_MASK) ||
        caps_lock_on) {
      key = 'C';
    } else
      key = 'c';
  } else if (event_code == KEY_D) {
    if ((modifiers & LEFT_SHIFT_MASK) || (modifiers & RIGHT_SHIFT_MASK) ||
        caps_lock_on) {
      key = 'D';
    } else
      key = 'd';
  } else if (event_code == KEY_E) {
    if ((modifiers & LEFT_SHIFT_MASK) || (modifiers & RIGHT_SHIFT_MASK) ||
        caps_lock_on) {
      key = 'E';
    } else
      key = 'e';
  } else if (event_code == KEY_F) {
    if ((modifiers & LEFT_SHIFT_MASK) || (modifiers & RIGHT_SHIFT_MASK) ||
        caps_lock_on) {
      key = 'F';
    } else
      key = 'f';
  } else if (event_code == KEY_G) {
    if ((modifiers & LEFT_SHIFT_MASK) || (modifiers & RIGHT_SHIFT_MASK) ||
        caps_lock_on) {
      key = 'G';
    } else
      key = 'g';
  } else if (event_code == KEY_H) {
    if ((modifiers & LEFT_SHIFT_MASK) || (modifiers & RIGHT_SHIFT_MASK) ||
        caps_lock_on) {
      key = 'H';
    } else
      key = 'h';
  } else if (event_code == KEY_I) {
    if ((modifiers & LEFT_SHIFT_MASK) || (modifiers & RIGHT_SHIFT_MASK) ||
        caps_lock_on) {
      key = 'I';
    } else
      key = 'i';
  } else if (event_code == KEY_J) {
    if ((modifiers & LEFT_SHIFT_MASK) || (modifiers & RIGHT_SHIFT_MASK) ||
        caps_lock_on) {
      key = 'J';
    } else
      key = 'j';
  } else if (event_code == KEY_K) {
    if ((modifiers & LEFT_SHIFT_MASK) || (modifiers & RIGHT_SHIFT_MASK) ||
        caps_lock_on) {
      key = 'K';
    } else
      key = 'k';
  } else if (event_code == KEY_L) {
    if ((modifiers & LEFT_SHIFT_MASK) || (modifiers & RIGHT_SHIFT_MASK) ||
        caps_lock_on) {
      key = 'L';
    } else
      key = 'l';
  } else if (event_code == KEY_M) {
    if ((modifiers & LEFT_SHIFT_MASK) || (modifiers & RIGHT_SHIFT_MASK) ||
        caps_lock_on) {
      key = 'M';
    } else
      key = 'm';
  } else if (event_code == KEY_N) {
    if ((modifiers & LEFT_SHIFT_MASK) || (modifiers & RIGHT_SHIFT_MASK) ||
        caps_lock_on) {
      key = 'N';
    } else
      key = 'n';
  } else if (event_code == KEY_O) {
    if ((modifiers & LEFT_SHIFT_MASK) || (modifiers & RIGHT_SHIFT_MASK) ||
        caps_lock_on) {
      key = 'O';
    } else
      key = 'o';
  } else if (event_code == KEY_P) {
    if ((modifiers & LEFT_SHIFT_MASK) || (modifiers & RIGHT_SHIFT_MASK) ||
        caps_lock_on) {
      key = 'P';
    } else
      key = 'p';
  } else if (event_code == KEY_Q) {
    if ((modifiers & LEFT_SHIFT_MASK) || (modifiers & RIGHT_SHIFT_MASK) ||
        caps_lock_on) {
      key = 'Q';
    } else
      key = 'q';
  } else if (event_code == KEY_R) {
    if ((modifiers & LEFT_SHIFT_MASK) || (modifiers & RIGHT_SHIFT_MASK) ||
        caps_lock_on) {
      key = 'R';
    } else
      key = 'r';
  } else if (event_code == KEY_S) {
    if ((modifiers & LEFT_SHIFT_MASK) || (modifiers & RIGHT_SHIFT_MASK) ||
        caps_lock_on) {
      key = 'S';
    } else
      key = 's';
  } else if (event_code == KEY_T) {
    if ((modifiers & LEFT_SHIFT_MASK) || (modifiers & RIGHT_SHIFT_MASK) ||
        caps_lock_on) {
      key = 'T';
    } else
      key = 't';
  } else if (event_code == KEY_U) {
    if ((modifiers & LEFT_SHIFT_MASK) || (modifiers & RIGHT_SHIFT_MASK) ||
        caps_lock_on) {
      key = 'U';
    } else
      key = 'u';
  } else if (event_code == KEY_V) {
    if ((modifiers & LEFT_SHIFT_MASK) || (modifiers & RIGHT_SHIFT_MASK) ||
        caps_lock_on) {
      key = 'V';
    } else
      key = 'v';
  } else if (event_code == KEY_W) {
    if ((modifiers & LEFT_SHIFT_MASK) || (modifiers & RIGHT_SHIFT_MASK) ||
        caps_lock_on) {
      key = 'W';
    } else
      key = 'w';
  } else if (event_code == KEY_X) {
    if ((modifiers & LEFT_SHIFT_MASK) || (modifiers & RIGHT_SHIFT_MASK) ||
        caps_lock_on) {
      key = 'X';
    } else
      key = 'x';
  } else if (event_code == KEY_Y) {
    if ((modifiers & LEFT_SHIFT_MASK) || (modifiers & RIGHT_SHIFT_MASK) ||
        caps_lock_on) {
      key = 'Y';
    } else
      key = 'y';
  } else if (event_code == KEY_Z) {
    if ((modifiers & LEFT_SHIFT_MASK) || (modifiers & RIGHT_SHIFT_MASK) ||
        caps_lock_on) {
      key = 'Z';
    } else
      key = 'z';
  } else if (event_code == KEY_AROBE_SIGN) {
    key = '@';
  } else if (event_code == KEY_0) {
    key = '0';
  } else if (event_code == KEY_1) {
    key = '1';
  } else if (event_code == KEY_2) {
    key = '2';
  } else if (event_code == KEY_3) {
    key = '3';
  } else if (event_code == KEY_4) {
    key = '4';
  } else if (event_code == KEY_5) {
    key = '5';
  } else if (event_code == KEY_6) {
    key = '6';
  } else if (event_code == KEY_7) {
    key = '7';
  } else if (event_code == KEY_8) {
    key = '8';
  } else if (event_code == KEY_9) {
    key = '9';
  } else if (event_code == KEY_PLUS) {
    key = '+';
  } else if (event_code == KEY_STAR) {
    key = '*';
  } else if (event_code == KEY_HASH_TAG) {
    key = '#';
  } else if (event_code == KEY_MINUS) {
    key = '-';
  } else if (event_code == KEY_UNDER_SCORE) {
    key = '_';
  } else if (event_code == KEY_POINT) {
    key = '.';
  } else if (event_code == KEY_DOUBLE_POINTS) {
    key = ':';
  } else if (event_code == KEY_KOMMA) {
    key = ',';
  } else if (event_code == KEY_SEMICOLON) {
    key = ';';
  } else if (event_code == KEY_GREATER_THEN) {
    key = '>';
  } else if (event_code == KEY_PIPE) {
    key = '|';
  } else if (event_code == KEY_CURLY_BRACKET_LEFT) {
    key = '}';
  } else if (event_code == KEY_CURLY_BRACKET_RIGHT) {
    key = '{';
  } else if (event_code == KEY_EQUALS_SIGN) {
    key = '=';
  } else if (event_code == KEY_EXCLAMTATION_MARK_SIGN) {
    key = '!';
  } else if (event_code == KEY_QUESTION_MARK) {
    key = '?';
  } else if (event_code == KEY_AND) {
    key = '&';
  } else if (event_code == KEY_SLASH) {
    key = '/';
  } else if (event_code == KEY_PERCENTAGE) {
    key = '%';
  } else if (event_code == KEY_DOUBLE_QUOTE_SIGN) {
    key = '"';
  } else if (event_code == KEY_QUOTE_SIGN) {
    key = '\'';
  } else if (event_code == KEY_TAB) {
    key = '\t';
  } else if (event_code == KEY_SPACE) {
    key = ' ';
  } else if ((event_code == KEY_ENTER) && (event_value == KEY_PRESSED)) {
    display = 1;
  } else if ((event_code == KEY_DEL) && (event_value == KEY_PRESSED)) {
    if (sub_index < buffer_index) {
      for (int i = sub_index; i < buffer_index; i++) {
        string_b[i - 1] = string_b[i];
      }
      buffer_index--;
    } else {
      buffer_index = buffer_index == 0 ? 0 : buffer_index - 1;
      sub_index = buffer_index;
    }
  } else if (event_code == KEY_BACKSLASH) {
    key = '\\';
  } else if (event_code == KEY_PARENTHESIS_LEFT) {
    key = ')';
  } else if (event_code == KEY_PARENTHESIS_RIGHT) {
    key = '(';
  } else if (event_code == KEY_SQUARE_BRACKET_LEFT) {
    key = ']';
  } else if (event_code == KEY_SQUARE_BRACKET_RIGHT) {
    key = '[';
  } else if (event_code == KEY_TILDE) {
    key = '~';
  } else if ((event_code == KEY_LEFT_ARROW) && (event_value == KEY_PRESSED)) {
    if (sub_index > 0) {
      sub_index--;
    }
  } else if ((event_code == KEY_RIGHT_ARROW) && (event_value == KEY_PRESSED)) {
    if (sub_index < buffer_index) {
      sub_index++;
    }
  }

  if ((event_value == KEY_PRESSED) && (key != 0) &&
      (event_code != KEY_LEFT_ARROW) && (event_code != KEY_RIGHT_ARROW)) {
    if (sub_index < buffer_index) {
      for (int i = buffer_index; i > sub_index; i--) {
        string_b[i] = string_b[i - 1];
      }
      buffer_index++;
      string_b[sub_index++] = key;
    } else {
      string_b[buffer_index++] = key;
      sub_index++;
    }
    string_b[buffer_index] = '\0';
    c->trace_c(c, "%s", string_b);
  }
  /*if (event_value == KEY_PRESSED) {
    event_description = "Key Pressed";
  } else if (event_value == KEY_HOLD) {
    event_description = "Key Hold";
  } else if (event_value == KEY_RELEASED) {
    event_description = "Key Released";
  }

  //if (key != 0) {
  //  c->trace_c(c, "%s - %c : modifiers = (%d)", event_description, key,
  //             modifiers);
  //}

  if ((event_value == KEY_PRESSED)) {
    char local[200];
    for (int i = 0; i < sub_index; i++) {
      local[i] = ' ';
    }
    local[sub_index] = '^';
    local[sub_index + 1] = '\0';
    c->trace_c(c, "%s", local);
  }

  if (display) {
    string_b[buffer_index] = '\0';

    buffer_index = 0;
    sub_index = 0;
    display = 0;
  }
} */

uint16_t type_of_key_board(EventListener *listener) {
  return KEY_BOARD_LISTENER;
}

void new_key_board_listener(struct KeyBoardListener *listener) {
  listener->super.call = &key_board_call;
  listener->super.type_of = &type_of_key_board;
  listener->super.new_super_event_listener = &new_super_event_listener;
  listener->super.new_super_event_listener(&listener->super);
}