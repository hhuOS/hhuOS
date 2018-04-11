/*****************************************************************************
 *                                                                           *
 *                                 L O O P                                   *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Einstieg in eine Anwendung.                              *
 *                                                                           *
 * Autor:           Michael Schoettner, HHU, 24.8.2016                       *
 *****************************************************************************/

#ifndef __loop_include__
#define __loop_include__


#include <kernel/lock/Spinlock.h>
#include <kernel/lock/Mutex.h>
#include "kernel/threads/Thread.h"


class Loop : public Thread {
   
private:

    uint32_t myID;

    static Mutex printLock;

    bool isRunning = true;

public:

    explicit Loop (int i) : Thread ("Loop") { myID = i; }

    Loop (const Loop &copy) = delete;

    ~Loop() = default;

    // Zaehlt einen Zaehler hoch und gibt ihn auf dem Bildschirm aus.
    void run () override;
};

#endif
