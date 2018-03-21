/*****************************************************************************
 *                                                                           *
 *                                 T H R E A D                               *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Implementierung eines kooperativen Thread-Konzepts.      *
 *                  Thread-Objekte werden vom Scheduler in einer verketteten *
 *                  Liste 'readylist' verwaltet.                             *
 *                                                                           *
 *                  Im Konstruktor wird der initialie Kontext des Threads    *
 *                  eingerichtet. Mit 'start' wird ein Thread aktiviert.     *
 *                  Die CPU sollte mit 'yield' freiwillig abgegeben werden.  *
 *                  Um bei einem Threadwechsel den Kontext sichern zu        *
 *                  koennen, enthaelt jedes Threadobjekt eine Struktur       *
 *                  ThreadState, in dem die Werte der nicht-fluechtigen      *
 *                  Register gesichert werden koennen.                       *
 *                                                                           *
 *                  Zusaetzlich zum vorhandenen freiwilligen Umschalten der  *
 *                  CPU mit 'Thread_switch' gibt es nun ein forciertes Um-   *
 *                  durch den Zeitgeber-Interrupt ausgeloest wird und in     *
 *                  Assembler in startup.asm implementiert ist. Fuer das     *
 *                  Zusammenspiel mit dem Scheduler ist die Methode          *
 *                  'prepare_preemption' in Scheduler.cc wichtig.            *
 *                                                                           *
 * Autor:           Michael, Schoettner, HHU, 1.1.2017                       *
 *****************************************************************************/

#include "kernel/threads/Thread.h"
#include "kernel/threads/Scheduler.h"


#include <cstdint>

extern "C" {
    void interrupt_return();
}

// global counter for thread IDs
unsigned int TIDcnt = 0;

void kickoff () {

    Scheduler::getInstance()->currentThread->run();

    Scheduler::getInstance()->exit();
}

void Thread::init() {

    uint32_t *esp = (uint32_t*) stack->getStart();

    esp -= (sizeof(InterruptFrame) / 4);

    interruptFrame = (InterruptFrame*) esp;

    esp -= (sizeof(Context) / 4);

    context = (Context*) esp;

    context->eip = (uint32_t) interrupt_return;

    interruptFrame->cs    = 0x08;
    interruptFrame->fs    = 0x10;
    interruptFrame->gs    = 0x10;
    interruptFrame->ds    = 0x10;
    interruptFrame->es    = 0x10;
    interruptFrame->ss    = 0x10;

    interruptFrame->ebx   = 0;
    interruptFrame->esi   = 0;
    interruptFrame->edi   = 0;
    interruptFrame->esp   = (uint32_t) esp;
    interruptFrame->ebp   = (uint32_t) stack->getStart();
    interruptFrame->uesp  = 0;
    interruptFrame->eflags = 0x200;
    interruptFrame->eip   = (uint32_t) kickoff;
}

/**
 * Creates a new Thread and initializes its state
 */
Thread::Thread() {
    stack = new Stack(STACK_SIZE_DEFAULT);
    tid = TIDcnt++;

    this->name = new char[10];
    strcpy(this->name, "keylogger");

    init();
}

Thread::Thread(const char *name) {
    stack = new Stack(STACK_SIZE_DEFAULT);
    tid = TIDcnt++;

    uint32_t length = strlen(name);
    this->name = new char[length + 1];
    strcpy(this->name, name);

    init();
}

Thread::~Thread() {
    delete stack;
}

/**
 * Starts this Thread
 */
void Thread::start() {
	Scheduler::getInstance()->ready(*this);
}

uint32_t Thread::getTid() const {
    return tid;
}

const char *Thread::getName() const {
    return name;
}

Thread::Stack::Stack(uint32_t size) {
    this->size = size;
    this->stack = new uint8_t[size];

    this->stack[0] = 0x44; // D
    this->stack[1] = 0x41; // A
    this->stack[2] = 0x45; // E
    this->stack[3] = 0x44; // D
}

uint8_t *Thread::Stack::getStart() {
    return &stack[size];
}

Thread::Stack::~Stack() {
    delete stack;
}
