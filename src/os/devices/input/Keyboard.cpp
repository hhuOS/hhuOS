#include <kernel/Kernel.h>
#include <kernel/interrupts/IntDispatcher.h>
#include <kernel/interrupts/Pic.h>
#include <kernel/events/input/KeyEvent.h>
#include <lib/libc/printf.h>
#include <devices/sound/PcSpeaker.h>
#include <kernel/threads/Scheduler.h>
#include "Keyboard.h"

uint8_t Keyboard::normal_tab[] = {
        0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 225, 39, '\b',
        0, 'q', 'w', 'e', 'r', 't', 'z', 'u', 'i', 'o', 'p', 129, '+', '\n',
        0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 148, 132, '^', 0, '#',
        'y', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '-', 0,
        '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-',
        0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, '<', 0, 0
};

uint8_t Keyboard::shift_tab[] = {
        0, 0, '!', '"', 21, '$', '%', '&', '/', '(', ')', '=', '?', 96, 0,
        0, 'Q', 'W', 'E', 'R', 'T', 'Z', 'U', 'I', 'O', 'P', 154, '*', 0,
        0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 153, 142, 248, 0, 39,
        'Y', 'X', 'C', 'V', 'B', 'N', 'M', ';', ':', '_', 0,
        0, 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '>', 0, 0
};

uint8_t Keyboard::alt_tab[] = {
        0, 0, 0, 253, 0, 0, 0, 0, '{', '[', ']', '}', '\\', 0, 0,
        0, '@', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '~', 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 230, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '|', 0, 0
};

uint8_t Keyboard::asc_num_tab[] = {
        '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', ','
};

uint8_t Keyboard::scan_num_tab[] = {
        8, 9, 10, 53, 5, 6, 7, 27, 2, 3, 4, 11, 51
};

bool Keyboard::decodeKey() {
    bool done = false;
    bool isBreak =(code & break_bit);
    uint32_t scancode = code & ~break_bit;

    if(isBreak) {
        removeFromBuffer(scancode);
        gather.setPressed(false);
    } else {
        addToBuffer(scancode);
        gather.setPressed(true);
    }
    
    if(code == prefix1 || code == prefix2) {
        prefix = code;
        return false;
    }

    if(code & break_bit) {
        code &= ~break_bit;

        
        switch(code) {
            case 42:
            case 54:
                gather.shift(false);
                break;
            case 56:
                if(prefix == prefix1) {
                    gather.altRight(false);
                } else {
                    gather.altLeft(false);
                }
                break;
            case 29:
                if(prefix == prefix1) {
                    gather.ctrlRight(false);
                } else {
                    gather.ctrlLeft(false);
                }
                break;
            default:
                return false;
        }

        prefix = 0;
        
        return false;
    }

    switch(code) {
        case 42:
        case 54:
            gather.shift(true);
            break;
        case 56:
            if(prefix == prefix1) {
                gather.altRight(true);
            } else {
                gather.altLeft(true);
            }
            break;
        case 29:
            if(prefix == prefix1) {
                gather.ctrlRight(true);
            } else {
                gather.ctrlLeft(true);
            }
            break;
        case 58:
            gather.capsLock(!gather.capsLock());
            setLed(caps_lock, gather.capsLock());
            break;
        case 70:
            gather.scrollLock(!gather.scrollLock());
            setLed(scroll_lock, gather.scrollLock());
            break;
        case 69:
            if(gather.ctrlLeft()) {
                getAsciiCode();
                done = true;
            }
            else {
                gather.numLock(!gather.numLock());
                setLed(num_lock, gather.numLock());
            }
            break;

        default:
            getAsciiCode();
            done = true;
    }

    prefix = 0;

    return done;
}

void Keyboard::getAsciiCode() {
    if(code == 53 && prefix == prefix1) {
        gather.ascii('/');
        gather.scancode(Key::div);
    }

    else if(gather.numLock() && !prefix && code >= 71 && code <= 83) {
        gather.ascii(asc_num_tab[code-71]);
        gather.scancode(scan_num_tab[code-71]);
    }
    else if(gather.altRight()) {
        gather.ascii(alt_tab[code]);
        gather.scancode(code);
    }
    else if(gather.shift()) {
        gather.ascii(shift_tab[code]);
        gather.scancode(code);
    }
    else if(gather.capsLock()) {
        if((code >= 16 && code <= 26) ||(code >= 30 && code <= 40) ||(code >= 44 && code <= 50)) {
            gather.ascii(shift_tab[code]);
            gather.scancode(code);
        }
        else {
            gather.ascii(normal_tab[code]);
            gather.scancode(code);
        }
    }
    else {
        gather.ascii(normal_tab[code]);
        gather.scancode(code);
    }
}

Keyboard::Keyboard() : controlPort(0x64), dataPort(0x60), eventBuffer(1024) {

    eventBus = Kernel::getService<EventBus>();

    setLed(caps_lock, false);
    setLed(scroll_lock, false);
    setLed(num_lock, false);

    keysPressed = 0;

    setRepeatRate(0, 0);
}

Key Keyboard::keyHit() {
    Key invalid;

    uint32_t control;

    control = controlPort.inb();

    if((control & 0x1) != 0x1) {
        return invalid;
    }

    code =(uint8_t) dataPort.inb();

    if(!(control & auxb) && decodeKey()) {
        return gather;
    }

    return invalid;
}

void Keyboard::reboot() {
    uint32_t status;

    // Don't perform memory test on next boot.
    *(uint32_t*) 0xc0000472 = 0x1234;


    do {
        status = controlPort.inb();
    } while((status & inpb) != 0);

    controlPort.outb(cpu_reset);
}

void Keyboard::setRepeatRate(uint32_t speed, uint32_t delay) {
    uint32_t status, reply;

    dataPort.outb(set_speed);
    do {
        status = controlPort.inb();
    } while((status & outb) == 0);

    reply = dataPort.inb();

    if(reply == ack) {
        dataPort.outb(static_cast<uint8_t>(((delay & 3) << 5) | (speed & 31)));

        do {
            status = controlPort.inb();
        } while((status & outb) == 0);

        dataPort.inb();
    }
}

void Keyboard::setLed(uint8_t led, bool on) {
    uint32_t status, reply;

    dataPort.outb(set_led);
    
    do {
        status = controlPort.inb();
    } while((status & outb) == 0);

    reply = dataPort.inb();
    
    if(reply == ack) {
        if(on) {
            leds |= led;
        } else {
            leds &= ~led;
        }
        
        dataPort.outb(leds);
        
        do {
            status = controlPort.inb();
        } while((status & outb) == 0);
        
        dataPort.inb();
    }
}

uint32_t Keyboard::getKeysPressed(){
    return keysPressed;
}

bool Keyboard::isKeyPressed(uint32_t scancode){
    for (uint32_t &i : buffer) {
        if(i == scancode) {
            return true;
        }
    }
    return false;
}

void Keyboard::addToBuffer(uint32_t scancode){
    if(isKeyPressed(scancode)) {
        return;
    }

    for (uint32_t &i : buffer) {
        if(i == 0){
            i = scancode;
            keysPressed++;
            break;
        }
    }
}

void Keyboard::removeFromBuffer(uint32_t scancode){
    for (uint32_t &i : buffer) {
        if(i == scancode){
            i = 0;
            keysPressed--;
            break;
        }
    }
}

void Keyboard::plugin() {
    memset(buffer, 0, KB_BUFFER_SIZE * sizeof(uint32_t));

    Scheduler::getInstance()->registerIODevice(this);

    IntDispatcher::getInstance().assign(IntDispatcher::keyboard, *this);
    Pic::getInstance()->allow(Pic::Interrupt::KEYBOARD);
}

void Keyboard::trigger() {
    Key key = keyHit();

    if(key.valid()) {

        if(key.ctrl() && key.alt() && key.scancode() == Key::del) {
            reboot();
        }

        eventBuffer.push(KeyEvent(key));

        KeyEvent &event = eventBuffer.pop();

        eventBus->publish(event);
    }
}

bool Keyboard::checkForData() {
    return(controlPort.inb() & 0x1) == 0x1;
}
