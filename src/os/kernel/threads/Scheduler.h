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

#include "kernel/threads/Thread.h"
#include "lib/deprecated/Queue.h"

class Scheduler {
    
private:
    Scheduler (const Scheduler &copy); // Verhindere Kopieren
     
private:
    Queue<Thread> readyQueue;   // auf die CPU wartende Threads
    
    // Scheduler wird evt. von einer Unterbrechung vom Zeitgeber gerufen,
    // bevor er initialisiert wurde
    bool  initialized;
    
    static Scheduler *scheduler;

public:

    Scheduler ();

    Thread *currentThread;

    unsigned int *currentRegs;
    unsigned int *nextRegs;

    // Scheduler initialisiert?
    // Zeitgeber-Unterbrechung kommt evt. bevor der Scheduler fertig
    // intiialisiert wurde!
    bool isInitialized() { return initialized; }
    
    // Scheduler starten
    void schedule ();

    void dispatch(Thread& next);
    
    // Thread in ReadyQueue eintragen
    void ready (Thread& that);
    
    // Thread terminiert sich selbst
    void exit ();

    // Thread mit 'Gewalt' terminieren
    void kill (Thread& that);

    // CPU freiwillig abgeben und Auswahl des naechsten Threads
    void yield ();

    // Wartet ein Thread auf CPU-Zuteilung?
    bool isThreadWaiting () {
        return readyQueue.count() > 0;
    }

    // Aktuellen Thread blockieren, weil er auf eine Ressource wartet.
    // Scheduler soll auf naechsten Thread in readyQueue umschalten
    void block ();
    
    // Thread 'that' deblockieren -> in readyQueue einfuegen
    void deblock (Thread &that);

    // CPU soll aktuellem Thread entzogen werden (Vorbereitungen)
    // Wird von Unterbrechungsroutine des PIT gerufen
    bool preparePreemption();

    Thread *active() { return currentThread; }

    static Scheduler* getInstance() {
    	if(scheduler == nullptr) {
    		scheduler = new Scheduler();
    	}

    	return scheduler;
    }
};

#endif
