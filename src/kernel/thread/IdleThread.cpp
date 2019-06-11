#include "IdleThread.h"

namespace Kernel {

IdleThread::IdleThread() : Thread("IdleThread", 0) {

}

void IdleThread::run() {
    while (isRunning) {
        yield();
    }
}

}