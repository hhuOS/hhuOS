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

    bool isRunning = false;

public:

    IdleThread () : Thread ("IdleThread") {  }

    IdleThread(const Thread &copy) = delete;

    IdleThread &operator=(const Thread &other) = delete;

    ~IdleThread() override = default;
    
    void run () override {

        while (isRunning) {

            yield();
        }
    }
    
 };

#endif
