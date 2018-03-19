/*****************************************************************************
 *                                                                           *
 *                            H E A P D E M O                                *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Demonstration der dynamischen Speicherverwaltung.        *
 *                                                                           *
 * Autor:           Michael Schoettner, HHU, 25.9.2016                       *
 *****************************************************************************/
#ifndef __HeapDemo_include__
#define __HeapDemo_include__


#include "kernel/threads/Thread.h"


class HeapDemo : public Thread {
    
private:
    HeapDemo (const HeapDemo &copy); // Verhindere Kopieren
    
   
public:
    // Gib dem Anwendungsthread einen Stack.
    HeapDemo () : Thread ("HeapDemo") { }

    // Thread-Startmethode
    void run ();

 };

#endif
