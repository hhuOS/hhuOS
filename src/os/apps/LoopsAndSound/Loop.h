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


#include <lib/lock/Spinlock.h>
#include <lib/lock/Mutex.h>
#include "kernel/threads/Thread.h"

/**
 * A simple thread, which counts up a variable and prints it on the screen in an infinite loop.
 *
 * @author Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * @date 2016, 2018
 */
class Loop : public Thread {
   
private:

    uint32_t myID;

    static Mutex printLock;

    bool isRunning = true;

public:

    /**
     * Constructor.
     */
    explicit Loop (uint32_t id);

    /**
     * Copy-constructor.
     */
    Loop (const Loop &copy) = delete;

    /**
     * Destructor.
     */
    ~Loop() override = default;

    /**
     * Overriding function from Thread.
     */
    void run() override;
};

#endif
