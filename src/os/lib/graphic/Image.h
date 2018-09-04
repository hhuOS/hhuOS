#ifndef HHUOS_IMAGE_H
#define HHUOS_IMAGE_H

#include "Color.h"

class Image {

protected:

    Color *pixelBuf = nullptr;

    uint32_t width = 0;
    uint32_t height = 0;
    uint8_t depth = 0;

protected:

    Image() = default;

public:

    Image(const Image &copy) = delete;

    ~Image() = default;

    uint32_t getHeight();

    uint32_t getWidth();

    uint8_t getColorDepth();

    void scaleUp(uint8_t factor);

    void scaleDown(uint8_t factor);

    void draw(uint16_t x, uint16_t y);
};

#endif
