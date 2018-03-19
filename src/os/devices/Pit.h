/*****************************************************************************
 *                                                                           *
 *                                   P I T                                   *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Programmable Interval Timer.                             * 
 *                                                                           *
 * Autor:           Michael Schoettner, 23.8.2016                            *
 *****************************************************************************/

#ifndef __PIT_include__
#define __PIT_include__

#include <kernel/services/GraphicsService.h>
#include "kernel/services/TimeService.h"
#include "kernel/interrupts/ISR.h"
#include "kernel/IOport.h"
#include "devices/graphics/text/TextDriver.h"
#include "kernel/Kernel.h"

class Pit : public ISR {
    
private:
    bool cursor;
    GraphicsService *graphicsService;
    
    Pit(const Pit &copy); // Verhindere Kopieren
         
private:
    enum { time_base = 838 };  /* ns */
    int timer_interval;
    char signs[4];
    int signs_counter;

    TimeService *timeService;

    // Zeitgeber initialisieren.
    Pit (int us) {
        signs[0] = '|';
        signs[1] = '/';
        signs[2] = '-';
        signs[3] = '\\';
        signs_counter = 0;
        interval (us);
    }

    static Pit *pit;

public:


    static Pit *getInstance() {
        if(pit == nullptr) {
            pit = new Pit(10000);
        }
        return pit;
    }
    // Konfiguriertes Zeitintervall auslesen.
    int interval () {
        return timer_interval;
    }
    
    // Zeitintervall setzen.
    void interval (int us);

    // Aktivierung der Unterbrechungen fuer den Zeitgeber
    void plugin ();
    
    // Unterbrechnungsroutine des Zeitgebers.
    void trigger ();
    
    void setCursor(bool cursor);
    bool isCursorEnabled();
};

#endif
