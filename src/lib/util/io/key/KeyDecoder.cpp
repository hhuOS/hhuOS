/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include "KeyDecoder.h"

#include "lib/util/io/key/Key.h"
#include "lib/util/usb/input/InputEvents.h"

namespace Util::Io {

uint8_t KeyDecoder::normalTab[] = {
        0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 225, 39, '\b',
        '\t', 'q', 'w', 'e', 'r', 't', 'z', 'u', 'i', 'o', 'p', 129, '+', '\n',
        0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 148, 132, '^', 0, '#',
        'y', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '-', 0,
        '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-',
        0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, '<', 0, 0
};

uint8_t KeyDecoder::shiftTab[] = {
        0, 0, '!', '"', 21, '$', '%', '&', '/', '(', ')', '=', '?', 96, 0,
        0, 'Q', 'W', 'E', 'R', 'T', 'Z', 'U', 'I', 'O', 'P', 154, '*', 0,
        0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 153, 142, 248, 0, 39,
        'Y', 'X', 'C', 'V', 'B', 'N', 'M', ';', ':', '_', 0,
        0, 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '>', 0, 0
};

uint8_t KeyDecoder::altTab[] = {
        0, 0, 0, 253, 0, 0, 0, 0, '{', '[', ']', '}', '\\', 0, 0,
        0, '@', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '~', 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 230, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '|', 0, 0
};

uint8_t KeyDecoder::asciiNumTab[] = {
        '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', ','
};

uint8_t KeyDecoder::scanNumTab[] = {
        8, 9, 10, 53, 5, 6, 7, 27, 2, 3, 4, 11, 51
};

bool Util::Io::KeyDecoder::parseUsbCodes(uint8_t code, uint8_t type){
    if(type == KEY_MODIFIER){
        if(code & LEFT_CONTROL_MASK){
            currentKey.setCtrlLeft(true);
        }
        else if((code & LEFT_SHIFT_MASK) || (code & RIGHT_SHIFT_MASK)){
            currentKey.setShift(true);
        }
        else if(code & LEFT_ALT_MASK){
            currentKey.setAltLeft(true);
        }
        else if(code & RIGHT_CONTROL_MASK){
            currentKey.setCtrlRight(true);
        }
        else if(code & RIGHT_ALT_MASK){
            currentKey.setAltRight(true);
        }
    }
    else if(type == KEY_TYPE){
        if(code == KEY_PRESSED){
            currentKey.setPressed(true);
        }
        else if(code == KEY_RELEASED){
            currentKey.setPressed(false);
        }
    }
    else if(type == KEY_CODE){
        parseAsciiUsb(code);
        currentKey.setScancode(code);
    }

    return true;
}

void Util::Io::KeyDecoder::parseAsciiUsb(uint8_t event_code){
    char key = 0;

    if(event_code == KEY_CAPS_LOCK){
        currentKey.setCapsLock(!currentKey.getCapsLock());
    }
    if (event_code == KEY_A) {
        if (isHighLetter()) 
            key = 'A';
        else key = 'a';
    } else if (event_code == KEY_B) {
        if (isHighLetter()) 
            key = 'B';
        else
            key = 'b';
    } else if (event_code == KEY_C) {
        if ( isHighLetter())
            key = 'C';
        else
            key = 'c';
    } else if (event_code == KEY_D) {
        if ( isHighLetter())
            key = 'D';
        else key = 'd';
    } else if (event_code == KEY_E) {
        if ( isHighLetter())
            key = 'E';
        else
            key = 'e';
    } else if (event_code == KEY_F) {
        if (isHighLetter()) 
        key = 'F';
        else
        key = 'f';
    } else if (event_code == KEY_G) {
        if (isHighLetter()) 
        key = 'G';
        else
        key = 'g';
    } else if (event_code == KEY_H) {
        if (isHighLetter()) 
        key = 'H';
        else
        key = 'h';
    } else if (event_code == KEY_I) {
        if (isHighLetter()) 
        key = 'I';
        else
        key = 'i';
    } else if (event_code == KEY_J) {
        if (isHighLetter()) 
        key = 'J';
        else
        key = 'j';
    } else if (event_code == KEY_K) {
        if (isHighLetter()) 
        key = 'K';
        else
        key = 'k';
    } else if (event_code == KEY_L) {
        if (isHighLetter()) 
        key = 'L';
        else
        key = 'l';
    } else if (event_code == KEY_M) {
        if (isHighLetter()) 
        key = 'M';
        else
        key = 'm';
    } else if (event_code == KEY_N) {
        if (isHighLetter()) 
        key = 'N';
        else
        key = 'n';
    } else if (event_code == KEY_O) {
        if (isHighLetter()) 
        key = 'O';
        else
        key = 'o';
    } else if (event_code == KEY_P) {
        if (isHighLetter())
        key = 'P';
        else
        key = 'p';
    } else if (event_code == KEY_Q) {
        if (isHighLetter())
        key = 'Q';
        else
        key = 'q';
    } else if (event_code == KEY_R) {
        if (isHighLetter())
        key = 'R';
        else
        key = 'r';
    } else if (event_code == KEY_S) {
        if (isHighLetter()) 
        key = 'S';
        else
        key = 's';
    } else if (event_code == KEY_T) {
        if (isHighLetter()) 
        key = 'T';
        else
        key = 't';
    } else if (event_code == KEY_U) {
        if (isHighLetter()) 
        key = 'U';
        else
        key = 'u';
    } else if (event_code == KEY_V) {
        if (isHighLetter()) 
        key = 'V';
        else
        key = 'v';
    } else if (event_code == KEY_W) {
        if (isHighLetter()) 
        key = 'W';
        else
        key = 'w';
    } else if (event_code == KEY_X) {
        if (isHighLetter()) 
        key = 'X';
        else
        key = 'x';
    }  else if (event_code == KEY_Y) {
        if (isHighLetter()) 
        key = 'Y';
        else
        key = 'y';
    }  else if (event_code == KEY_Z) {
        if (isHighLetter()) 
        key = 'Z';
        else
        key = 'z';
    }
    else if (event_code == KEY_AROBE_SIGN) key = '@';
    else if (event_code == KEY_0) key = '0';
    else if (event_code == KEY_1) key = '1';
    else if (event_code == KEY_2) key = '2';
    else if (event_code == KEY_3) key = '3';
    else if (event_code == KEY_4) key = '4';
    else if (event_code == KEY_5) key = '5';
    else if (event_code == KEY_6) key = '6';
    else if (event_code == KEY_7) key = '7';
    else if (event_code == KEY_8) key = '8';
    else if (event_code == KEY_9) key = '9';
    else if (event_code == KEY_PLUS) key = '+';
    else if (event_code == KEY_STAR) key = '*';
    else if (event_code == KEY_HASH_TAG) key = '#';
    else if (event_code == KEY_MINUS) key = '-';
    else if (event_code == KEY_UNDER_SCORE) key = '_';
    else if (event_code == KEY_POINT) key = '.';
    else if (event_code == KEY_DOUBLE_POINTS) key = ':';
    else if (event_code == KEY_KOMMA) key = ',';
    else if (event_code == KEY_SEMICOLON) key = ';';
    else if (event_code == KEY_GREATER_THEN) key = '>';
    else if (event_code == KEY_PIPE) key = '|';
    else if (event_code == KEY_CURLY_BRACKET_LEFT) key = '}';
    else if (event_code == KEY_CURLY_BRACKET_RIGHT) key = '{';
    else if (event_code == KEY_EQUALS_SIGN) key = '=';
    else if (event_code == KEY_EXCLAMTATION_MARK_SIGN) key = '!';
    else if (event_code == KEY_QUESTION_MARK) key = '?';
    else if (event_code == KEY_AND) key = '&';
    else if (event_code == KEY_SLASH) key = '/';
    else if (event_code == KEY_PERCENTAGE) key = '%';
    else if (event_code == KEY_DOUBLE_QUOTE_SIGN) key = '"';
    else if (event_code == KEY_QUOTE_SIGN) key = '\'';
    else if (event_code == KEY_TAB) key = '\t';
    else if (event_code == KEY_SPACE) key = ' ';
    else if (event_code == KEY_ENTER) key = '\n';
    else if (event_code == KEY_DEL) key = '\b';
    else if (event_code == KEY_BACKSLASH) key = '\\';
    else if (event_code == KEY_PARENTHESIS_LEFT) key = ')';
    else if (event_code == KEY_PARENTHESIS_RIGHT) key = '(';
    else if (event_code == KEY_SQUARE_BRACKET_LEFT) key = ']';
    else if (event_code == KEY_SQUARE_BRACKET_RIGHT) key = '[';
    else if (event_code == KEY_TILDE) key = '~';

    currentKey.setAscii(key);
}

bool Util::Io::KeyDecoder::isHighLetter(){
    return currentKey.getShift() == false ? currentKey.getCapsLock() == false ? false : true : true;
}

void Util::Io::KeyDecoder::defaulting(){
    currentKey.setDefault();
}

bool Util::Io::KeyDecoder::parseScancode(uint8_t code) {
    bool done = false;
    bool isBreak = (code & BREAK_BIT);

    if (isBreak) {
        currentKey.setPressed(false);
    } else {
        currentKey.setPressed(true);
    }

    if (code == PREFIX1 || code == PREFIX2) {
        currentPrefix = code;
        return false;
    }

    if (code & BREAK_BIT) {
        code &= ~BREAK_BIT;

        switch (code) {
            case 42:
            case 54:
                currentKey.setShift(false);
                currentPrefix = 0;
                return false;
            case 56:
                if (currentPrefix == PREFIX1) {
                    currentKey.setAltRight(false);
                } else {
                    currentKey.setAltLeft(false);
                }
                currentPrefix = 0;
                return false;
            case 29:
                if (currentPrefix == PREFIX1) {
                    currentKey.setCtrlRight(false);
                } else {
                    currentKey.setCtrlLeft(false);
                }
                currentPrefix = 0;
                return false;
            default:
                break;
        }
    }

    switch (code) {
        case 42:
        case 54:
            currentKey.setShift(true);
            break;
        case 56:
            if (currentPrefix == PREFIX1) {
                currentKey.setAltRight(true);
            } else {
                currentKey.setAltLeft(true);
            }
            break;
        case 29:
            if (currentPrefix == PREFIX1) {
                currentKey.setCtrlRight(true);
            } else {
                currentKey.setCtrlLeft(true);
            }
            break;
        case 58:
            currentKey.setCapsLock(!currentKey.getCapsLock());
            break;
        case 70:
            currentKey.setScrollLock(!currentKey.getScrollLock());
            break;
        case 69:
            if (currentKey.getCtrlLeft()) {
                parseAsciiCode(code);
                done = true;
            } else {
                currentKey.setNumLock(!currentKey.getNumLock());
            }
            break;

        default:
            parseAsciiCode(code);
            done = true;
    }

    currentPrefix = 0;
    return done;
}

void Util::Io::KeyDecoder::parseAsciiCode(uint8_t code) {
    if(code == 53 && currentPrefix == PREFIX1) {
        currentKey.setAscii('/');
        currentKey.setScancode(Util::Io::Key::DIV);
    } else if(currentKey.getNumLock() && !currentPrefix && code >= 71 && code <= 83) {
        currentKey.setAscii(asciiNumTab[code-71]);
        currentKey.setScancode(scanNumTab[code-71]);
    } else if(currentKey.getAltRight()) {
        currentKey.setAscii(altTab[code]);
        currentKey.setScancode(code);
    } else if(currentKey.getShift()) {
        currentKey.setAscii(shiftTab[code]);
        currentKey.setScancode(code);
    } else if(currentKey.getCapsLock()) {
        if((code >= 16 && code <= 26) ||(code >= 30 && code <= 40) ||(code >= 44 && code <= 50)) {
            currentKey.setAscii(shiftTab[code]);
            currentKey.setScancode(code);
        } else {
            currentKey.setAscii(normalTab[code]);
            currentKey.setScancode(code);
        }
    } else {
        currentKey.setAscii(normalTab[code]);
        currentKey.setScancode(code);
    }
}

Util::Io::Key Util::Io::KeyDecoder::getCurrentKey() const {
    return currentKey;
}

}