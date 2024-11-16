#include "Layer.h"

Layer::Layer(int width, int height, int posX, int posY, int visible) : width(width), height(height), posX(posX),
                                                                       posY(posY), isVisible(visible) {
    int size = width * height;
    pixelData = new uint32_t[size];
    for (int i = 0; i < size; ++i) {
        pixelData[i] = 0;
    }
    tempPixelData = new uint32_t[0];
    prepareNextDrawing();
}

Layer::Layer(int width, int height, int posX, int posY, int visible, const uint32_t *pixelData)
        : width(width), height(height), posX(posX), posY(posY), isVisible(visible) {
    int size = width * height;
    this->pixelData = new uint32_t[size];
    for (int i = 0; i < size; ++i) {
        this->pixelData[i] = pixelData[i];
    }
    tempPixelData = new uint32_t[0];
    prepareNextDrawing();
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

uint32_t *Layer::getPixelData() const {
    return pixelData;
}

uint32_t *Layer::getTempPixelData() const {
    return tempPixelData;
}


uint32_t Layer::getPixel(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return 0;
    }
    return pixelData[y * width + x];
}

uint32_t Layer::getTempPixel(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return 0;
    }
    return tempPixelData[y * width + x];
}

void Layer::setPixel(int x, int y, unsigned int color) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return; // Silent fail if out of bounds
    }
    int index = y * width + x;
    pixelData[index] = color;
}

void Layer::setNewBuffer(uint32_t *newBuffer, int x, int y, int w, int h) {
    delete[] pixelData;
    pixelData = newBuffer;
    posX = x;
    posY = y;
    width = w;
    height = h;
}

void Layer::prepareNextDrawing() {
    delete[] tempPixelData;
    tempPixelData = new uint32_t[width * height];
    for (int i = 0; i < width * height; ++i) {
        tempPixelData[i] = pixelData[i];
    }
}
