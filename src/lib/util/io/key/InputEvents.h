#ifndef InputEvents__include
#define InputEvents__include

#include "stdint.h"

extern uint16_t KEY_A;
extern uint16_t KEY_B;
extern uint16_t KEY_C;
extern uint16_t KEY_D;
extern uint16_t KEY_E;
extern uint16_t KEY_F;
extern uint16_t KEY_G;
extern uint16_t KEY_H;
extern uint16_t KEY_I;
extern uint16_t KEY_J;
extern uint16_t KEY_K;
extern uint16_t KEY_L;
extern uint16_t KEY_M;
extern uint16_t KEY_N;
extern uint16_t KEY_O;
extern uint16_t KEY_P;
extern uint16_t KEY_Q;
extern uint16_t KEY_R;
extern uint16_t KEY_S;
extern uint16_t KEY_T;
extern uint16_t KEY_U;
extern uint16_t KEY_V;
extern uint16_t KEY_W;
extern uint16_t KEY_X;
extern uint16_t KEY_Y;
extern uint16_t KEY_Z;
extern uint16_t KEY_1;
extern uint16_t KEY_2;
extern uint16_t KEY_3;
extern uint16_t KEY_4;
extern uint16_t KEY_5;
extern uint16_t KEY_6;
extern uint16_t KEY_7;
extern uint16_t KEY_8;
extern uint16_t KEY_9;
extern uint16_t KEY_0;
extern uint16_t KEY_ENTER;
extern uint16_t KEY_ESC  ;
extern uint16_t KEY_DEL;
extern uint16_t KEY_TAB;
extern uint16_t KEY_SPACE;
extern uint16_t KEY_ẞ;
extern uint16_t KEY_RIGHT_SINGLE;
extern uint16_t KEY_U_DOUBLE_POINTS;
extern uint16_t KEY_PLUS;
extern uint16_t KEY_LOWER_THAN; // is this really true ?
extern uint16_t KEY_A_DOUBLE_POINTS;
extern uint16_t KEY_HASH_TAG;
extern uint16_t KEY_EXP_SIGN;
extern uint16_t KEY_KOMMA;
extern uint16_t KEY_POINT;
extern uint16_t KEY_MINUS;
extern uint16_t KEY_CAPS_LOCK;
extern uint16_t KEY_F1;
extern uint16_t KEY_F2;
extern uint16_t KEY_F3;
extern uint16_t KEY_F4;
extern uint16_t KEY_F5;
extern uint16_t KEY_F6;
extern uint16_t KEY_F7;
extern uint16_t KEY_F8;
extern uint16_t KEY_F9;
extern uint16_t KEY_F10;
extern uint16_t KEY_F11;
extern uint16_t KEY_F12;
extern uint16_t KEY_INSERT;
extern uint16_t KEY_SCROLL;
extern uint16_t KEY_PG_UP;
extern uint16_t KEY_PAD_DEL;
extern uint16_t KEY_END;
extern uint16_t KEY_PG_DOWN;
extern uint16_t KEY_RIGHT_ARROW;
extern uint16_t KEY_LEFT_ARROW;
extern uint16_t KEY_DOWN_ARROW;
extern uint16_t KEY_UP_ARROW;
 
extern uint16_t KEY_GREATER_THEN;
extern uint16_t KEY_PIPE;
extern uint16_t KEY_SEMICOLON;
extern uint16_t KEY_DOUBLE_POINTS;
extern uint16_t KEY_UNDER_SCORE;
extern uint16_t KEY_QUOTE_SIGN; // '
extern uint16_t KEY_STAR;
extern uint16_t KEY_TILDE;
extern uint16_t KEY_LEFT_SINGLE; // `
extern uint16_t KEY_QUESTION_MARK;
extern uint16_t KEY_BACKSLASH;
extern uint16_t KEY_CURLY_BRACKET_LEFT; // }
extern uint16_t KEY_EQUALS_SIGN;
extern uint16_t KEY_SQUARE_BRACKET_LEFT;
extern uint16_t KEY_PARENTHESIS_LEFT;
extern uint16_t KEY_SQUARE_BRACKET_RIGHT;
extern uint16_t KEY_PARENTHESIS_RIGHT;
extern uint16_t KEY_CURLY_BRACKET_RIGHT;
extern uint16_t KEY_SLASH;
extern uint16_t KEY_AND;
extern uint16_t KEY_PERCENTAGE;
extern uint16_t KEY_DOLLAR_SIGN;
extern uint16_t KEY_LAW_SIGN;
extern uint16_t KEY_DOUBLE_QUOTE_SIGN;
extern uint16_t KEY_EXCLAMTATION_MARK_SIGN;
extern uint16_t KEY_CIRLCE_SIGN;
 
extern uint16_t MOUSE_EVENT;
extern uint16_t MOUSE_ENTERED;
extern uint16_t MOUSE_RELEASED;
extern uint16_t MOUSE_HOLD;
 
extern uint16_t KEY_EVENT;
extern uint16_t KEY_PRESSED;
extern uint16_t KEY_HOLD;
extern uint16_t KEY_RELEASED;
 
extern uint16_t MOUSE_M1;
extern uint16_t MOUSE_M2;
extern uint16_t MOUSE_SCROLL;
extern uint16_t MOUSE_DPI_BUTTON;
extern uint16_t MOUSE_LEFT;
extern uint16_t MOUSE_RIGHT;

extern uint16_t KEY_AROBE_SIGN;

#define LEFT_CONTROL_MASK 0x01
#define LEFT_SHIFT_MASK 0x02
#define LEFT_ALT_MASK 0x04
#define LEFT_GUI_MASK 0x08

#define RIGHT_CONTROL_MASK 0x10
#define RIGHT_SHIFT_MASK 0x20
#define RIGHT_ALT_MASK 0x40
#define RIGHT_GUI_MASK 0x80

extern uint16_t AUDIO_EVENT;
extern uint16_t AUDIO_IN;
extern uint16_t AUDIO_OUT;

#endif