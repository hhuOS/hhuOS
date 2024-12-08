#ifndef LAYER_H
#define LAYER_H

#include <cstdint>

class Layer {
public:
    Layer(int width, int height, int posX, int posY, int visible);

    Layer(int width, int height, int posX, int posY, int visible, const uint32_t *pixelData);

    ~Layer();

    // Copy constructor
    Layer(const Layer &other);

    // Assignment operator
    Layer &operator=(const Layer &other);

    [[nodiscard]] uint32_t *getPixelData() const;

    [[nodiscard]] uint32_t *getTempPixelData() const;

    [[nodiscard]] uint32_t getPixel(int x, int y) const;

    [[nodiscard]] uint32_t getTempPixel(int x, int y) const;

    void setNewBuffer(uint32_t *newBuffer, int x, int y, int w, int h);

    void setPixel(int x, int y, unsigned int color);

    void prepareNextDrawing();

    int width;
    int height;
    int posX;
    int posY;
    int isVisible;
private:
    uint32_t *pixelData; // ARGB format, 32 bits per pixel
    uint32_t *tempPixelData; // for pen blending
};

#endif // LAYER_H