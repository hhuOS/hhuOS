#ifndef HHUOS_ATOMICBITMAP_H
#define HHUOS_ATOMICBITMAP_H

#include <cstdint>

namespace Util::Memory {

class AtomicBitmap {

public:

    AtomicBitmap() = default;

    explicit AtomicBitmap(uint32_t blockCount);

    AtomicBitmap(const AtomicBitmap &copy) = delete;

    AtomicBitmap &operator=(const AtomicBitmap &other) = delete;

    ~AtomicBitmap() = default;

    [[nodiscard]] uint32_t getSize() const;

    void set(uint32_t block);

    void unset(uint32_t block);

    bool check(uint32_t block, bool set);

    uint32_t findAndSet();

private:

    uint32_t *bitmap = nullptr;
    uint32_t arraySize = 0;
    uint32_t blocks = 0;

};

}

#endif