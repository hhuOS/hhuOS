#ifndef HHUOS_ATOMICBITMAP_H
#define HHUOS_ATOMICBITMAP_H

#include "Bitmap.h"

class AtomicBitmap : public Bitmap {

public:

    AtomicBitmap() = default;

    explicit AtomicBitmap(uint32_t blockCount);

    AtomicBitmap(uint32_t blockCount, uint32_t reservedBlocksAtBeginning);

    AtomicBitmap(const AtomicBitmap &copy) = delete;

    AtomicBitmap& operator=(const AtomicBitmap &other) = delete;

    ~AtomicBitmap() override = default;

    void set(uint32_t block) override;

    void unset(uint32_t block) override;

    bool check(uint32_t block, bool set) override;

    uint32_t findAndSet();

    void setRange(uint32_t startBlock, uint32_t length) override;

    void unsetRange(uint32_t startBlock, uint32_t length) override;

    bool checkRange(uint32_t startBlock, uint32_t length, bool set) override;

    uint32_t findAndSet(uint32_t blockCount) override;

private:

    static uint32_t bitTestAndSet(uint32_t value, uint32_t bit);

    static uint32_t bitTestAndReset(uint32_t value, uint32_t bit);

};

#endif
