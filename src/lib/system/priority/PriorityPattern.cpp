#include "PriorityPattern.h"

PriorityPattern::PriorityPattern(uint8_t priorityCount) : priorityCount(
        static_cast<uint8_t>(priorityCount == 0 ? 1 : priorityCount)) {

}

uint8_t PriorityPattern::getMaxPriority() {
    return static_cast<uint8_t>(priorityCount - 1);
}

uint8_t PriorityPattern::getMinPriority() {
    return 0;
}

uint8_t PriorityPattern::getPriorityCount() {
    return priorityCount;
}