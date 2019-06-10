#include "Key.h"

Key::Key() : asc(0), scan(0), modi(0), pressed(false) {

}


bool Key::valid() {
    return scan != 0;
}


bool Key::isPressed() {
    return pressed;
}


void Key::setPressed(bool value) {
    pressed = value;
}


void Key::invalidate() {
    scan = 0;
}


void Key::ascii(uint8_t a) {
    asc = a;
}


void Key::scancode(uint8_t s)  {
    scan = s;
}


unsigned char Key::ascii() {
    return asc;
}


unsigned char Key::scancode() {
    return scan;
}


void Key::shift(bool pressed) {
    modi = pressed ? modi | mbit::shift : modi & ~mbit::shift;
}


void Key::altLeft(bool pressed) {
    modi = pressed ? modi | mbit::alt_left : modi & ~mbit::alt_left;
}


void Key::altRight(bool pressed) {
    modi = pressed ? modi | mbit::alt_right : modi & ~mbit::alt_right;
}


void Key::ctrlLeft(bool pressed) {
    modi = pressed ? modi | mbit::ctrl_left : modi & ~mbit::ctrl_left;
}


void Key::ctrlRight(bool pressed) {
    modi = pressed ? modi | mbit::ctrl_right : modi & ~mbit::ctrl_right;
}


void Key::capsLock(bool pressed) {
    modi = pressed ? modi | mbit::caps_lock : modi & ~mbit::caps_lock;
}


void Key::numLock(bool pressed) {
    modi = pressed ? modi | mbit::num_lock : modi & ~mbit::num_lock;
}


void Key::scrollLock(bool pressed) {
    modi = pressed ? modi | mbit::scroll_lock : modi & ~mbit::scroll_lock;
}



bool Key::shift() {
    return modi & mbit::shift;
}


bool Key::altLeft() {
    return modi & mbit::alt_left;
}


bool Key::altRight() {
    return modi & mbit::alt_right;
}


bool Key::ctrlLeft() {
    return modi & mbit::ctrl_left;
}


bool Key::ctrlRight() {
    return modi & mbit::ctrl_right;
}


bool Key::capsLock() {
    return modi & mbit::caps_lock;
}


bool Key::numLock() {
    return modi & mbit::num_lock;
}


bool Key::scrollLock() {
    return modi & mbit::scroll_lock;
}


bool Key::alt() {
    return altLeft() | altRight();
}


bool Key::ctrl() {
    return ctrlLeft() | ctrlRight();
}

Key::operator char() {
    return(char) asc;
}

Key::operator unsigned char() {
    return(unsigned char) asc;
}