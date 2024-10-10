#ifndef LAYER_H
#define LAYER_H

#include <cstdint>

class Layer {
public:
    Layer(int width, int height, int posX, int posY, int visible);
    Layer(int width, int height, int posX, int posY, int visible, const uint32_t * pixelData);
    ~Layer();

    // Copy constructor
    Layer(const Layer& other);

    // Assignment operator
    Layer& operator=(const Layer& other);

    [[nodiscard]] int getWidth() const { return width; }
    [[nodiscard]] int getHeight() const { return height; }
    [[nodiscard]] int getPosX() const { return posX; }
    [[nodiscard]] int getPosY() const { return posY; }
    [[nodiscard]] uint32_t * getPixelData() const { return pixelData; }
    [[nodiscard]] int getVisibility() const { return isVisible; }

    void setPosition(int x, int y);
    void setPosX(int x) { posX = x; }
    void setPosY(int y) { posY = y; }
    void setPixel(int x, int y, unsigned int color);
    void setVisibility(int visible) { isVisible = visible; }

private:
    int width;
    int height;
    int posX;
    int posY;
    int isVisible;
    uint32_t * pixelData; // ARGB format, 32 bits per pixel
};

#endif // LAYER_H