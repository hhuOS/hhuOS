//
// Created by burak on 05.03.18.
//

#include <devices/input/Keyboard.h>
#include "InputService.h"

InputService::InputService() {
    keyboard = new Keyboard();
    mouse = new Mouse();
}

Keyboard* InputService::getKeyboard() {
    return keyboard;
}

Mouse* InputService::getMouse() {
    return mouse;
}