#ifndef LAYER_H
#define LAYER_H

#include <cstdint>
#include "DataWrapper.h"
#include "lib/libc/math.h"

class Layer {
public:
    Layer(int width, int height, int posX, int posY, int visible);
    Layer(int width, int height, int posX, int posY, int visible, const uint32_t * pixelData);
    ~Layer();

    // Copy constructor
    Layer(const Layer& other);

    // Assignment operator
    Layer& operator=(const Layer& other);

    [[nodiscard]] uint32_t * getPixelData() const { return pixelData; }

    void setPixel(int x, int y, unsigned int color);

    uint32_t getPixel(int x, int y) const;

    void scale(double factor, ToolCorner kind);

    void crop (int left, int right, int top, int bottom);

    int width;
    int height;
    int posX;
    int posY;
    int isVisible;
private:
    uint32_t * pixelData; // ARGB format, 32 bits per pixel
};

#endif // LAYER_H