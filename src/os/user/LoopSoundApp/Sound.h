/*****************************************************************************
 *                                                                           *
 *                               S O U N D                                   *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Einstieg in eine Anwendung.                              *
 *                                                                           *
 * Autor:           Michael Schoettner, HHU, 24.8.2016                       *
 *****************************************************************************/

#ifndef __Sound_include__
#define __Sound_include__


#include "kernel/threads/Thread.h"


class Sound : public Thread {
   
private:
    Sound (const Sound &copy); // Verhindere Kopieren
    
public:
    Sound (int i) : Thread ("Sound") { }

    // Spielt sound ab.
    void run ();
};

#endif
