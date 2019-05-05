#include "SimpleThread.h"

SimpleThread::SimpleThread(void (*work)()) : work(work) {

}

void SimpleThread::run() {
    work();
}
