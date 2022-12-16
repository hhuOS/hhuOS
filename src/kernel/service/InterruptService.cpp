/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "InterruptService.h"

namespace Kernel {
class InterruptHandler;
struct InterruptFrame;

void InterruptService::assignInterrupt(InterruptDispatcher::Interrupt slot, InterruptHandler &handler) {
    dispatcher.assign(slot, handler);
}

void InterruptService::dispatchInterrupt(const InterruptFrame &frame) {
    dispatcher.dispatch(frame);
}

void InterruptService::allowHardwareInterrupt(Device::Pic::Interrupt interrupt) {
    pic.allow(interrupt);
}

void InterruptService::forbidHardwareInterrupt(Device::Pic::Interrupt interrupt) {
    pic.forbid(interrupt);
}

void InterruptService::sendEndOfInterrupt(InterruptDispatcher::Interrupt interrupt) {
    if (interrupt >= InterruptDispatcher::PIT && interrupt <= InterruptDispatcher::SECONDARY_ATA) {
        pic.sendEndOfInterrupt(static_cast<Device::Pic::Interrupt>(interrupt - InterruptDispatcher::PIT));
    }
}

bool InterruptService::checkSpuriousInterrupt(InterruptDispatcher::Interrupt interrupt) {
    if (interrupt != InterruptDispatcher::LPT1 && interrupt != InterruptDispatcher::SECONDARY_ATA) {
        return false;
    }

    return pic.isSpurious(static_cast<Device::Pic::Interrupt>(interrupt - InterruptDispatcher::PIT));
}

}