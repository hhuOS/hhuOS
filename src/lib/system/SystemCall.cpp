/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
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

#include "SystemCall.h"

void(*SystemCall::systemCalls[256])() = {};

extern "C" {
    int32_t atexit (void (*func)()) noexcept;
}

void SystemCall::registerSystemCall(SystemCall::SystemCallCode code, void (*func)()) {
    systemCalls[code] = func;
}

void SystemCall::trigger(InterruptFrame &frame) {
    systemCalls[frame.eax]();
}

int32_t atexit (void (*func)()) noexcept {
    return 0;
}

int32_t SystemCall::atExit(void (*func)()) {

    return atexit(func);
}
