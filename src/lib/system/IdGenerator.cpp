#include "IdGenerator.h"

uint32_t IdGenerator::getId() {
    return nextId.fetchAndInc();
}
