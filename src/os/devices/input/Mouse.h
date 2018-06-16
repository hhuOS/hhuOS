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
#include "kernel/IOport.h"
#include <kernel/events/input/MouseClickedEvent.h>
#include <kernel/events/input/MouseReleasedEvent.h>
#include <kernel/events/input/MouseMovedEvent.h>
#include <kernel/events/input/MouseDoubleClickEvent.h>

#define MOUSE_DEBUG 0

#if MOUSE_DEBUG
#define     MOUSE_TRACE(...) printf("[MOUSE] " __VA_ARGS__)
#else
#define     MOUSE_TRACE(...)
#endif

/**
 * A simple ps2 mouse driver
 *
 * @author Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * @date HHU, 2018
 */
class Mouse : public InterruptHandler {

private:

    TimeService *timeService;

    // IO ports (same as keyboard)
    const IOport ctrl_port;    // Status- (R) u. Steuerregister (W)
    const IOport data_port;    // Ausgabe- (R) u. Eingabepuffer (W)

    EventBus *eventBus;
    // TODO MouseDragedEvent
    Util::RingBuffer<MouseMovedEvent> movedEventBuffer;
    Util::RingBuffer<MouseClickedEvent> clickedEventBuffer;
    Util::RingBuffer<MouseReleasedEvent> releasedEventBuffer;
    Util::RingBuffer<MouseDoubleClickEvent> doubleclickEventBuffer;

    bool available = true;

    // Zykluiszähler für das Einlesen der Bytes
    unsigned int cycle;

    // Beim Maus-Interrupt: Flags und Offsets der Bewegung
    unsigned int flags;
    int dx, dy;

    // Buttons merken bei Interrupt
    unsigned int buttons;
    //
    uint32_t lastClickTimestamp = 0;


    // warte darauf, dass Daten am Controller zum Lesen bereit liegen
    void waitData();

    // Warte  darauf, dass ein Befehl zum Controller geschickt werden kann
    void waitControl();

    // lese Daten vom Controller ein
    unsigned char read();

    // schicke Kommando an den Mauscontroller
    void write(unsigned char byte_write);

    // Aktiviere Maus
    void activate();

    void writeCommandAndByte(unsigned char byte_write, unsigned char data, char* commandString);

    void writeCommand(unsigned char byte_write, char* commandString);

public:
    // Initialisierung der Maus.
    Mouse();

    // Verhindere Kopieren.
    Mouse(const Mouse &copy) = delete;

    // Aktivierung der Unterbrechungen fuer die Maus
    void plugin();

    // Unterbrechnungsroutine der Maus
    void trigger();

    bool isAvailable() {
        return available;
    }

    void cleanup();
};

#endif //HHUOS_MOUSE_H
