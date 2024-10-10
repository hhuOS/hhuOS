#include "Layer.h"

Layer::Layer(int width, int height, int posX, int posY, int visible) : width(width), height(height), posX(posX),
                                                                       posY(posY), isVisible(visible) {
    int size = width * height;
    pixelData = new uint32_t[size];
    for (int i = 0; i < size; ++i) {
        pixelData[i] = 0;
    }
}

Layer::Layer(int width, int height, int posX, int posY, int visible, const uint32_t *pixelData)
        : width(width), height(height), posX(posX), posY(posY), isVisible(visible) {
    int size = width * height;
    this->pixelData = new uint32_t[size];
    for (int i = 0; i < size; ++i) {
        this->pixelData[i] = pixelData[i];
    }
}

Layer::~Layer() {
    delete[] pixelData;
}

Layer::Layer(const Layer &other)
        : width(other.width), height(other.height), posX(other.posX), posY(other.posY) {
    int size = width * height;
    pixelData = new uint32_t[size];
    for (int i = 0; i < size; ++i) {
        pixelData[i] = other.pixelData[i];
    }
}

Layer &Layer::operator=(const Layer &other) {
    if (this != &other) {
        delete[] pixelData;

        width = other.width;
        height = other.height;
        posX = other.posX;
        posY = other.posY;

        int size = width * height;
        pixelData = new uint32_t[size];
        for (int i = 0; i < size; ++i) {
            pixelData[i] = other.pixelData[i];
        }
    }
    return *this;
}

void Layer::setPosition(int x, int y) {
    posX = x;
    posY = y;
}

void Layer::setPixel(int x, int y, unsigned int color) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return; // Silent fail if out of bounds
    }
    int index = y * width + x;
    pixelData[index] = color;
}