#ifndef HHUOS_IDGENERATOR_H
#define HHUOS_IDGENERATOR_H

#include <cstdint>
#include <lib/async/Atomic.h>

class IdGenerator {

private:

    Atomic<uint32_t> nextId;

public:

    IdGenerator() = default;

    IdGenerator(const IdGenerator &other) = delete;

    IdGenerator &operator=(const IdGenerator &other) = delete;

    ~IdGenerator() = default;

    uint32_t getId();
};

#endif
