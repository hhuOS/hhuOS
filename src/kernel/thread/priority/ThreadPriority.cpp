#include "ThreadPriority.h"

namespace Kernel {

ThreadPriority::ThreadPriority(uint8_t priorityCount) : priorityCount(
        static_cast<uint8_t>(priorityCount == 0 ? 1 : priorityCount)) {

}

uint8_t ThreadPriority::getMaxPriority() {
    return static_cast<uint8_t>(priorityCount - 1);
}

uint8_t ThreadPriority::getMinPriority() {
    return 0;
}

uint8_t ThreadPriority::getPriorityCount() {
    return priorityCount;
}

}