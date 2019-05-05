#include "SimpleThread.h"

SimpleThread::SimpleThread(void (*work)()) noexcept : work(work) {

}

void SimpleThread::run() {
    work();
}
