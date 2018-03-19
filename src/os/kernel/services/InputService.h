//
// Created by burak on 05.03.18.
//

#ifndef HHUOS_INPUTSERVICE_H
#define HHUOS_INPUTSERVICE_H

#include <devices/input/Mouse.h>
#include "devices/input/Keyboard.h"

class InputService : public KernelService {

private:
    Keyboard *keyboard;
    Mouse *mouse;

public:
    InputService();

    static constexpr char* SERVICE_NAME = "InputService";

    Keyboard *getKeyboard();
    Mouse *getMouse();
};


#endif //HHUOS_INPUTSERVICE_H
