#ifndef HHUOS_BITMAP_H
#define HHUOS_BITMAP_H

#include <cstdint>

class Bitmap {

private:

    uint32_t *bitmap = nullptr;

    uint32_t arraySize = 0;

    uint32_t blocks = 0;

public:

    Bitmap() = default;

    explicit Bitmap(uint32_t blockCount);

    Bitmap(uint32_t blockCount, uint32_t reservedBlocksAtBeginning);

    Bitmap(const Bitmap &copy) = delete;

    Bitmap& operator=(const Bitmap &other) = delete;

    ~Bitmap();

    uint32_t getSize();

    void set(uint32_t block);

    void unset(uint32_t block);

    bool check(uint32_t block, bool set);

    void setRange(uint32_t startBlock, uint32_t length);

    void unsetRange(uint32_t startBlock, uint32_t length);

    bool checkRange(uint32_t startBlock, uint32_t length, bool set);

    uint32_t findAndSet(uint32_t blockCount);

    void dump();

private:

    uint32_t findNext(uint32_t startBlock, bool set);
};

#endif
