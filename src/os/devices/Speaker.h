/*****************************************************************************
 *                                                                           *
 *                                P C S P K                                  *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Mit Hilfe dieser Klasse kann man Toene auf dem           *
 *                  PC-Lautsprecher ausgeben.                                *
 *                                                                           *
 * Achtung:         Qemu muss mit dem Parameter -soundhw pcspk aufgerufen    *
 *                  werden. Ansonsten kann man nichts hoeren.                *
 *                                                                           *
 * Autor:           Michael Schoettner, HHU, 22.9.2016                       *
 *****************************************************************************/

#ifndef __PCSPK_include__
#define __PCSPK_include__

#include "kernel/IOport.h"
#include "kernel/services/TimeService.h"

// Note, Frequenz
#define     C0      130.81
#define     C0X	 	138.59 
#define     D0 	 	146.83 
#define     D0X     155.56 
#define     E0      164.81 
#define     F0      174.61 
#define     F0X 	185.00 
#define     G0      196.00 
#define     G0X     207.65 
#define     A0      220.00 
#define     A0X 	233.08 
#define     B0      246.94 

#define     C1      261.63 
#define     C1X     277.18 
#define     D1      293.66 
#define     D1X     311.13 
#define     E1      329.63 
#define     F1  	349.23 
#define     F1X     369.99 
#define     G1      391.00 
#define     G1X     415.30 
#define     A1      440.00 
#define     A1X     466.16 
#define     B1      493.88 

#define     C2      523.25 
#define     C2X 	554.37 
#define     D2      587.33 
#define     D2X 	622.25 
#define     E2      659.26 
#define     F2      698.46 
#define     F2X     739.99 
#define     G2      783.99 
#define     G2X     830.61 
#define     A2      880.00 
#define     A2X     923.33 
#define     B2      987.77 
#define     C3      1046.50 


class Speaker {
    
private:
    IOport control;     // Steuerregister (write only)
    IOport data0;       // Zaehler-0 Datenregister (read/write)
    IOport data2;       // Zaehler-2 Datenregister
    IOport ppi;         // Status-Register des PPI

    Speaker (const Speaker &copy); // Verhindere Kopieren

    // Verzoegerung um X ms (in 1ms Schritten; Min. 1ms)
    inline void delay (int time);
    
    // Zaehler von PIT Channel 0 auslesen (wird fuer delay benoetigt)
    inline unsigned int readCounter ();

    TimeService *timeService = nullptr;

public:

    // Konstruktor. Initialisieren der Ports.
    Speaker () : control(0x43), data0(0x40), data2(0x42), ppi(0x61) {}
    
    // Demo Sounds
    void tetris ();
    void aerodynamic ();

    // Ton abspielen
    void play (float f, int len);

    void play (float f);
    
    // Lautsprecher ausschalten
    void off ();
};

#endif
