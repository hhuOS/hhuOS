#include "SimplePriorityPattern.h"

SimplePriorityPattern::SimplePriorityPattern(uint8_t priorityCount) : PriorityPattern(priorityCount) {

}

uint8_t SimplePriorityPattern::getNextPriority() {
    return currentPriority++ % priorityCount;
}

void SimplePriorityPattern::reset() {
    currentPriority = 0;
}