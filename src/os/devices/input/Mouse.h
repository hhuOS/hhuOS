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

#ifndef HHUOS_MOUSE_H
#define HHUOS_MOUSE_H

#include <kernel/interrupts/InterruptHandler.h>
#include <kernel/services/TimeService.h>
#include <kernel/services/EventBus.h>
#include "devices/cpu/IOport.h"
#include <kernel/events/input/MouseClickedEvent.h>
#include <kernel/events/input/MouseReleasedEvent.h>
#include <kernel/events/input/MouseMovedEvent.h>
#include <kernel/events/input/MouseDoubleClickedEvent.h>
#include "kernel/log/Logger.h"

/**
 * A simple ps2 mouse driver
 *
 * @author Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * @date HHU, 2018
 */
class Mouse : public InterruptHandler {

private:

    static Logger &log;

    TimeService *timeService;

    // IO ports (same as keyboard)
    const IOport ctrl_port;
    const IOport data_port;

    EventBus *eventBus;
    // TODO MouseDragedEvent
    Util::RingBuffer<MouseMovedEvent> movedEventBuffer;
    Util::RingBuffer<MouseClickedEvent> clickedEventBuffer;
    Util::RingBuffer<MouseReleasedEvent> releasedEventBuffer;
    Util::RingBuffer<MouseDoubleClickedEvent> doubleclickEventBuffer;

    Util::RingBuffer<uint8_t> interruptDataBuffer;

    bool available = true;

    // Cycle count for reading bytes from the mouse
    unsigned int cycle;

    // Interrupt flags and mouse offset
    unsigned int flags;
    int dx, dy;

    // Button status
    unsigned int buttons;

    uint32_t lastClickTimestamp = 0;

    /**
     * Wait for the controller to have data available.
     */
    void waitData();

    /**
     * Wait for the controller to be ready to receive commands
     */
    void waitControl();

    /**
     * Read a byte from the controller.
     */
    unsigned char read();

    /**
     * Write a command to the controller.
     */
    void write(unsigned char byte_write);

    /**
     * Activate the mouse.
     */
    void activate();

    void writeCommandAndByte(unsigned char byte_write, unsigned char data, char* commandString);

    void writeCommand(unsigned char byte_write, char* commandString);

public:
    /**
     * Constructor.
     */
    Mouse();

    /**
     * Copy-constructor.
     */
    Mouse(const Mouse &copy) = delete;

    /**
     * Enable interrupts for the mouse.
     */
    void plugin();

    bool hasInterruptData() override;

    void parseInterruptData() override;

    /**
     * Overriding function from InterruptHandler.
     */
    void trigger(InterruptFrame &frame) override;

    bool isAvailable() {
        return available;
    }

    void cleanup();
};

#endif //HHUOS_MOUSE_H
