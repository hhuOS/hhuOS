/*****************************************************************************
*                                                                           *
*                                K E Y B O A R D                            *
*                                                                           *
*---------------------------------------------------------------------------*
* Beschreibung:    Treiber für den Tastaturcontroller des PCs.              *
*                                                                           *
* Autor:           Olaf Spinczyk, TU Dortmund                               *
*                  Modifikationen, Michael Schoettner, 21.12.2016           *
*****************************************************************************/
#ifndef __Keyboard_include__
#define __Keyboard_include__

#include <lib/util/RingBuffer.h>
#include "kernel/services/EventBus.h"
#include "Key.h"
#include "kernel/IOport.h"
#include "kernel/interrupts/ISR.h"
#include "devices/graphics/text/CgaText.h"
#include "kernel/events/input/KeyEvent.h"

class Keyboard : public ISR {

private:

    Keyboard(const Keyboard &copy); // Verhindere Kopieren

    const static int KB_BUFFER_SIZE = 4;

    unsigned char code;     // Byte von Tastatur
    unsigned char prefix;   // Prefix von Tastatur
    Key gather;             // letzter dekodierter Key
    char leds;              // Zustand LEDs

    int keysPressed;
    int buffer[KB_BUFFER_SIZE];

    // Benutzte Ports des Tastaturcontrollers
    const IOport ctrl_port;    // Status- (R) u. Steuerregister (W)
    const IOport data_port;    // Ausgabe- (R) u. Eingabepuffer (W)

    // Bits im Statusregister
    enum { outb = 0x01, inpb = 0x02, auxb = 0x20 };

    // Kommandos an die Tastatur
    struct kbd_cmd {
        enum { set_led = 0xed, set_speed = 0xf3 };
    };
    enum { cpu_reset = 0xfe };

    // Namen der LEDs
    struct led {
        enum { caps_lock = 4, num_lock = 2, scroll_lock = 1 };
    };

    // Antworten der Tastatur
    struct kbd_reply {
        enum { ack = 0xfa };
    };

    // Konstanten fuer die Tastaturdekodierung
    enum { break_bit = 0x80, prefix1 = 0xe0, prefix2   = 0xe1 };

    // Klassenvariablen
    static unsigned char normal_tab[];
    static unsigned char shift_tab[];
    static unsigned char alt_tab[];
    static unsigned char asc_num_tab[];
    static unsigned char scan_num_tab[];

    // Interpretiert die Make und Break-Codes der Tastatur.
    bool key_decoded ();

    // Ermittelt anhand von Tabellen den ASCII-Code.
    void get_ascii_code ();

    void addToBuffer (int scancode);

    void removeFromBuffer (int scancode);

    EventBus *eventBus;

    Util::RingBuffer<KeyEvent> eventBuffer;

public:

    char lastKey;   // zuletzt gedrueckte Taste

    // Initialisierung der Tastatur.
    Keyboard ();

    // Tastaturabfrage (vorerst Polling)
    Key key_hit ();

    // Letzter dekodierter Tastenanschlag
    Key lastHit ();

    void resetLast();

    // Fuehrt einen Neustart des Rechners durch.
    void reboot ();

    // Einstellen der Wiederholungsrate der Tastatur.
    void set_repeat_rate (int speed, int delay);

    // Setzt oder loescht die angegebene Leuchtdiode.
    void set_led (char led, bool on);

    // Aktivierung der Unterbrechungen fuer die Tastatur
    void plugin ();

    // Unterbrechnungsroutine der Tastatur.
    void trigger ();

    int getKeysPressed ();

    bool isKeyPressed (int scancode);
};

#endif
