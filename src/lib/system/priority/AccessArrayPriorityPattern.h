#ifndef HHUOS_ACCESSARRAYPRIORITYPATTERN_H
#define HHUOS_ACCESSARRAYPRIORITYPATTERN_H

#include "lib/util/Array.h"
#include "PriorityPattern.h"

class AccessArrayPriorityPattern : public PriorityPattern {

public:

    explicit AccessArrayPriorityPattern(uint8_t priorityCount);

    AccessArrayPriorityPattern(const AccessArrayPriorityPattern &copy) = delete;

    ~AccessArrayPriorityPattern() = default;

    uint8_t getNextPriority() override;

    void reset() override;

private:

    Util::Array<uint8_t> accessArray;

    uint8_t currentIndex;

};

#endif
