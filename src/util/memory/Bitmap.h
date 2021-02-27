#ifndef HHUOS_BITMAP_H
#define HHUOS_BITMAP_H

#include <cstdint>

namespace Util::Memory {

class Bitmap {

protected:

    uint32_t *bitmap = nullptr;

    uint32_t arraySize = 0;

    uint32_t blocks = 0;

public:

    Bitmap() = default;

    explicit Bitmap(uint32_t blockCount);

    Bitmap(uint32_t blockCount, uint32_t reservedBlocksAtBeginning);

    Bitmap(const Bitmap &copy) = delete;

    Bitmap &operator=(const Bitmap &other) = delete;

    virtual ~Bitmap();

    [[nodiscard]] uint32_t getSize() const;

    virtual void set(uint32_t block);

    virtual void unset(uint32_t block);

    virtual bool check(uint32_t block, bool set);

    virtual void setRange(uint32_t startBlock, uint32_t length);

    virtual void unsetRange(uint32_t startBlock, uint32_t length);

    virtual bool checkRange(uint32_t startBlock, uint32_t length, bool set);

    virtual uint32_t findAndSet(uint32_t blockCount);

private:

    uint32_t findNext(uint32_t startBlock, bool set);
};

}

#endif
