#include "SimpleThreadPriority.h"

SimpleThreadPriority::SimpleThreadPriority(uint8_t priorityCount) : ThreadPriority(priorityCount) {

}

uint8_t SimpleThreadPriority::getNextPriority() {
    return currentPriority++ % priorityCount;
}

void SimpleThreadPriority::reset() {
    currentPriority = 0;
}
