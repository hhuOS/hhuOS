/*****************************************************************************
 *                                                                           *
 *                          S C H E D U L E R                                *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Implementierung eines einfachen Zeitscheiben-Schedulers. *
 *                  Rechenbereite Threads werden in 'readQueue' verwaltet.   *
 *                  Threads koennen nun blockiert und deblockiert werden.    *
 *                  Dies ist fuer die Implementierung von Semaphoren         *
 *                  notwendig.                                               *
 *                                                                           *
 * Autor:           Michael, Schoettner, HHU, 5.1.2017                       *
 *****************************************************************************/

#ifndef __Scheduler_include__
#define __Scheduler_include__

#include <lib/util/BlockingQueue.h>
#include <kernel/services/InputService.h>
#include "kernel/threads/Thread.h"
#include "lib/deprecated/Queue.h"
#include "kernel/lock/Spinlock.h"

static Spinlock schedulerLock;

class Scheduler {
    
public:

    Scheduler();

    Scheduler(const Scheduler &copy) = delete;

    Scheduler &operator=(const Scheduler &copy) = delete;

    ~Scheduler() = default;

    /**
     * Inidcates if the Scheduler has been initialized.
     *
     * @return true, if the Scheduler has been initialized, false else
     */
    bool isInitialized();
    
    /**
     * Starts the Scheduler.
     */
    void schedule ();

    /**
     * Switches to the given Thread.
     *
     * @param next A Thread.
     */
    void dispatch(Thread& next);
    
    /**
     * Registers a new Thread.
     *
     * @param that A Thread.
     */
    void ready (Thread& that);
    
    /**
     * Terminates the current Thread.
     */
    void exit ();

    /**
     * Kills a specific Thread.
     *
     * @param that A Thread
     */
    void kill (Thread& that);

    /**
     * Switches to the next Thread.
     */
    void yield ();

    /**
     * Indicates if a Thread is waiting for execution.
     *
     * @return true, if a Thread is waiting, false else
     */
    bool isThreadWaiting ();

    /**
     * Blocks the current Thread.
     */
    void block ();
    
    /**
     * Unblocks a specific Thread.
     *
     * @param that A Thread
     */
    void deblock (Thread &that);

    /**
     * Returns the active Thread.
     *
     * @return The active Thread
     */
    Thread *active() { return currentThread; }

    /**
     * Returns an instance of the Scheduler.
     *
     * @return An instance of the Scheduler
     */
    static Scheduler* getInstance();

    Thread *currentThread;

private:

    Util::BlockingQueue<Thread*> readyQueue;

    bool  initialized = false;

    static Scheduler *scheduler;
};

#endif
