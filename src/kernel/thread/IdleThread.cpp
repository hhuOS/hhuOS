#include "IdleThread.h"

namespace Kernel {

IdleThread::IdleThread() : KernelThread("IdleThread", 0) {

}

void IdleThread::run() {
    while (isRunning) {
        yield();
    }
}

}