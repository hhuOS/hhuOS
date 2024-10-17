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


void Layer::setPixel(int x, int y, unsigned int color) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return; // Silent fail if out of bounds
    }
    int index = y * width + x;
    pixelData[index] = color;
}

void Layer::scale(double factor, ToolCorner kind) {
    if (factor <= 0) return;
    int newWidth = ceil(width * factor);
    int newHeight = ceil(height * factor);
    auto *newPixelData = new uint32_t[newWidth * newHeight];

    for (int y = 0; y < newHeight; ++y) {
        for (int x = 0; x < newWidth; ++x) {
            int oldX = floor(x / factor);
            int oldY = floor(y / factor);
            if (oldX < width && oldY < height) {
                newPixelData[y * newWidth + x] = pixelData[oldY * width + oldX];
            } else {
                newPixelData[y * newWidth + x] = 0;
            }
        }
    }

    delete[] pixelData;
    pixelData = newPixelData;

    if (kind == TOP_LEFT || kind == BOTTOM_LEFT)
        posX = posX + width - newWidth;
    if (kind == TOP_LEFT || kind == TOP_RIGHT)
        posY = posY + height - newHeight;

    width = newWidth;
    height = newHeight;
}

void Layer::crop(int left, int right, int top, int bottom) {
    int newWidth = width - left - right, newHeight = height - top - bottom;
    if (newWidth <= 0 || newHeight <= 0) return;
    uint32_t *newPixelData = new uint32_t[newWidth * newHeight];
    for (int i = 0; i < newWidth * newHeight; ++i) newPixelData[i] = 0x00000000;
    for (int y = 0; y < newHeight; ++y) {
        for (int x = 0; x < newWidth; ++x) {
            int oldX = x + left, oldY = y + top;
            if (oldX >= 0 && oldX < width && oldY >= 0 && oldY < height) {
                newPixelData[y * newWidth + x] = pixelData[oldY * width + oldX];
            }
        }
    }
    delete[] pixelData;
    pixelData = newPixelData;
    width = newWidth, height = newHeight;
    posX += left, posY += top;
}

uint32_t Layer::getPixel(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return 0;
    }
    return pixelData[y * width + x];
}

void Layer::rotate(int degree) {
    degree = (degree % 360 + 360) % 360;  // Normalize degree to 0-359
    if (degree == 0) return;  // No rotation needed

    int newWidth = abs(width * cos(degree * PI / 180.0)) + abs(height * sin(degree * PI / 180.0));
    int newHeight = abs(width * sin(degree * PI / 180.0)) + abs(height * cos(degree * PI / 180.0));
    auto *newPixelData = new uint32_t[newWidth * newHeight];
    for (int i = 0; i < newWidth * newHeight; ++i) newPixelData[i] = 0x00000000;

    double radians = degree * PI / 180.0;
    double cosTheta = cos(radians);
    double sinTheta = sin(radians);

    int centerX = width / 2;
    int centerY = height / 2;
    int newCenterX = newWidth / 2;
    int newCenterY = newHeight / 2;

    for (int y = 0; y < newHeight; ++y) {
        for (int x = 0; x < newWidth; ++x) {
            int srcX = static_cast<int>((x - newCenterX) * cosTheta + (y - newCenterY) * sinTheta) + centerX;
            int srcY = static_cast<int>(-(x - newCenterX) * sinTheta + (y - newCenterY) * cosTheta) + centerY;

            if (srcX >= 0 && srcX < width && srcY >= 0 && srcY < height) {
                newPixelData[y * newWidth + x] = pixelData[srcY * width + srcX];
            }
        }
    }

    delete[] pixelData;
    pixelData = newPixelData;

    posX -= (newWidth - width) / 2;
    posY -= (newHeight - height) / 2;
    width = newWidth;
    height = newHeight;
}
