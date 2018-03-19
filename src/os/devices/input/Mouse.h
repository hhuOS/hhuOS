//
// Created by burak on 05.03.18.
//

#ifndef HHUOS_MOUSE_H
#define HHUOS_MOUSE_H

#include <kernel/interrupts/ISR.h>
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

class Mouse : public ISR {

private:

    TimeService *timeService;

    EventBus *eventBus;

    // Benutzte Ports des Mauscontroller
    const IOport ctrl_port;    // Status- (R) u. Steuerregister (W)
    const IOport data_port;    // Ausgabe- (R) u. Eingabepuffer (W)

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

    // Verhindere Kopieren.
    Mouse(const Mouse &copy);

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
