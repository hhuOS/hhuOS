#ifndef __Key_include__
#define __Key_include__

#include <cstdint>

/**
 * Represents a key, consisting of a key- and an ascii-code.
 *
 * @author Olaf Spinczyk, TU Dortmund; Fabian Ruhland, HHU
 * @date 2016, 2018
 */
class Key {

private:

    uint8_t asc;
    uint8_t scan;
    uint8_t modi;
    bool pressed;

private:

    /**
     * Bit-masks for key-modifiers.
     */
    struct mbit {
        enum {
            shift       = 1,
            alt_left    = 2,
            alt_right   = 4,
            ctrl_left   = 8,
            ctrl_right  = 16,
            caps_lock   = 32,
            num_lock    = 64,
            scroll_lock = 128
        };
    };

public:

    /**
     * Constructor.
     */
    Key() : asc(0), scan(0), modi(0), pressed(false) {}

    /**
     * Check, if a key is valid(scancode == 0 -> invalid).
     */
    bool valid() {
        return scan != 0;
    }

    /**
     * Check, if a key is being pressed.
     */
    bool isPressed() {
        return pressed;
    }

    /**
     * Set the pressed-value.
     */
    void setPressed(bool value) {
        pressed = value;
    }

    /**
     * Invalidate this key(Set scancode to 0).
     */
    void invalidate() {
        scan = 0;
    }

    /**
     * Set the ascii-code.
     */
    void ascii(uint8_t a) {
        asc = a;
    }

    /**
     * Set the scancode.
     */
    void scancode(uint8_t s)  {
        scan = s;
    }

    /**
     * Get the ascii-code.
     */
    unsigned char ascii() {
        return asc;
    }

    /**
     * Get the scancode.
     */
    unsigned char scancode() {
        return scan;
    }

    /**
     * Set the shift-modifier.
     */
    void shift(bool pressed) {
        modi = pressed ? modi | mbit::shift : modi & ~mbit::shift;
    }

    /**
     * Set the alt-modifier for the left alt-key.
     */
    void altLeft(bool pressed) {
        modi = pressed ? modi | mbit::alt_left : modi & ~mbit::alt_left;
    }

    /**
     * Set the alt-modifier for the right alt-key.
     */
    void altRight(bool pressed) {
        modi = pressed ? modi | mbit::alt_right : modi & ~mbit::alt_right;
    }

    /**
     * Set the ctrl-modifier for the left alt-key.
     */
    void ctrlLeft(bool pressed) {
        modi = pressed ? modi | mbit::ctrl_left : modi & ~mbit::ctrl_left;
    }

    /**
     * Set the ctrl-modifier for the right alt-key.
     */
    void ctrlRight(bool pressed) {
        modi = pressed ? modi | mbit::ctrl_right : modi & ~mbit::ctrl_right;
    }

    /**
     * Set the caps-lock modifier.
     */
    void capsLock(bool pressed) {
        modi = pressed ? modi | mbit::caps_lock : modi & ~mbit::caps_lock;
    }

    /**
     * Set the num-lock modifier.
     */
    void numLock(bool pressed) {
        modi = pressed ? modi | mbit::num_lock : modi & ~mbit::num_lock;
    }

    /**
     * Set the scroll-lock modifier.
     */
    void scrollLock(bool pressed) {
        modi = pressed ? modi | mbit::scroll_lock : modi & ~mbit::scroll_lock;
    }


    /**
     * Get the shift-modifier.
     */
    bool shift() {
        return modi & mbit::shift;
    }

    /**
     * Get the alt-modifier for the left alt-key.
     */
    bool altLeft() {
        return modi & mbit::alt_left;
    }

    /**
     * Get the alt-modifier for the right alt-key.
     */
    bool altRight() {
        return modi & mbit::alt_right;
    }

    /**
     * Get the ctrl-modifier for the left alt-key.
     */
    bool ctrlLeft() {
        return modi & mbit::ctrl_left;
    }

    /**
     * Get the ctrl-modifier for the right alt-key.
     */
    bool ctrlRight() {
        return modi & mbit::ctrl_right;
    }

    /**
     * Get the caps-lock modifier.
     */
    bool capsLock() {
        return modi & mbit::caps_lock;
    }

    /**
     * Get the num-lock modifier.
     */
    bool numLock() {
        return modi & mbit::num_lock;
    }

    /**
     * Get the scroll-lock modifier.
     */
    bool scrollLock() {
        return modi & mbit::scroll_lock;
    }

    /**
     * Check, if one of the alt-keys is pressed.
     */
    bool alt() {
        return altLeft() | altRight();
    }

    /**
     * Check, if one of the ctrl-keys is pressed.
     */
    bool ctrl() {
        return ctrlLeft() | ctrlRight();
    }

    /**
     *(char)-operator. Return the ascii-code.
     */
    explicit operator char() {
        return(char) asc;
    }

    /**
     *(unsigned char)-operator. Return the ascii-code.
     */
    explicit operator unsigned char() {
        return(unsigned char) asc;
    }

    /**
     * Some pre-defined scancodes.
     */
    enum {
        f1 = 0x3b,
        del = 0x53,
        up=72,
        down=80,
        left=75,
        right=77,
        div = 8
    };
 };

#endif
