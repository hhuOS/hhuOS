/*****************************************************************************
 *                                                                           *
 *                             I D L E T H R E A D                           *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Wird nur aktiviert, wenn kein Thread arbeiten moechte.   *
 *                                                                           *
 * Autor:           Michael, Schoettner, HHU, 13.8.2016                      *
 *****************************************************************************/

#ifndef __IdleThread_include__
#define __IdleThread_include__


#include "kernel/threads/Thread.h"



class IdleThread : public Thread {
    
private:
    IdleThread(const Thread &copy); // Verhindere Kopieren

public:

    IdleThread () : Thread ("IdleThread") {  }
    
    void run () {
        while (1) {
            // CPU sofort abgeben, damit ein evt.
            // wartender Thread sofort loslegen kann.
            //scheduler.yield ();
        }
    }
    
 };

#endif
