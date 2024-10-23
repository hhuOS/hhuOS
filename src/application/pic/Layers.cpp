//
// Created by Rafael Reip on 21.10.24.
//

#include "Layers.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "lib/util/graphic/stb_image.h"
#include "lib/util/graphic/stb_image_write.h"

Layers::Layers(MessageHandler *mHandler) {
    this->layers = new Layer *[15];
    this->layerCount = 0;
    this->maxLayerCount = 15;
    this->currentLayer = 0;
    this->mHandler = mHandler;
}

Layer *Layers::current() {
    return layers[currentLayer];
}

void Layers::addEmpty(int width, int height, int posX, int posY) {
    if (layerCount >= maxLayerCount) {
        mHandler->addMessage(Util::String::format("Maximum number of layers reached: %d", maxLayerCount).operator const char *());
        return;
    }
    layers[layerCount] = new Layer(width, height, posX, posY, 1);
    layerCount++;
}

void Layers::addPicture(const char *path, int posX, int posY) {
    if (layerCount >= maxLayerCount) {
        mHandler->addMessage(Util::String::format("Maximum number of layers reached: %d", maxLayerCount).operator const char *());
        return;
    }
    int width, height, channels;
    unsigned char *img = stbi_load(path, &width, &height, &channels, 0);
    if (img == nullptr) {
        mHandler->addMessage(Util::String::format("Failed to load image: %s", path).operator const char *());
        mHandler->addMessage(Util::String::format("stbi_error: %s", stbi_failure_reason()).operator const char *());
        return;
    }
    mHandler->addMessage(Util::String::format("Loaded image with width %d, height %d, and channels %d", width, height,
                                              channels).operator const char *());
    auto *argbData = new uint32_t[width * height];
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int i = y * width + x;
            int j = (height - 1 - y) * width + x;
            argbData[i] = (0xFF000000 |
                           (img[j * channels] << 16) |
                           (img[j * channels + 1] << 8) |
                           img[j * channels + 2]);
        }
    }
    layers[layerCount] = new Layer(width, height, posX, posY, 1, argbData);
    layerCount++;
    stbi_image_free(img);
}

Layer *Layers::at(int index) {
    if (index >= layerCount) {
        return nullptr;
    }
    return layers[index];
}

int Layers::countNum() const {
    return layerCount;
}

int Layers::currentNum() const {
    return currentLayer;
}

int Layers::maxNum() const {
    return maxLayerCount;
}

void Layers::setCurrent(int index) {
    if (index < 0 || index >= layerCount) {
        return;
    }
    currentLayer = index;
}

void Layers::setCurrentToNext() {
    currentLayer++;
    if (currentLayer >= layerCount) {
        currentLayer = 0;
    }
}

void Layers::deletetAt(int index) {
    if (index < 0 || index >= layerCount) {
        mHandler->addMessage(Util::String::format("Layer index out of bounds: %d", index).operator const char *());
        return;
    }
    if (index == currentLayer) {
        currentLayer = 0;
    }
    delete layers[index];
    for (int i = index; i < layerCount - 1; i++) {
        layers[i] = layers[i + 1];
    }
    layerCount--;
}

void Layers::swap(int index1, int index2) {
    if (index1 < 0 || index1 >= layerCount || index2 < 0 || index2 >= layerCount || index1 == index2) {
        mHandler->addMessage(Util::String::format("Invalid layer indices: %d, %d", index1, index2).operator const char *());
        return;
    }
    Layer *temp = layers[index1];
    layers[index1] = layers[index2];
    layers[index2] = temp;
}

void Layers::combine(int index1, int index2) {
    if (index1 < 0 || index1 >= layerCount || index2 < 0 || index2 >= layerCount || index1 == index2) {
        mHandler->addMessage(Util::String::format("Invalid layer indices: %d, %d", index1, index2));
        return;
    }
    if (index1 > index2) { // to preserve the order of the layers
        int temp = index1;
        index1 = index2;
        index2 = temp;
    }
    Layer *l1 = layers[index1], *l2 = layers[index2];

    int newX = min(l1->posX, l2->posX);
    int newY = min(l1->posY, l2->posY);
    int newWidth = max(l1->posX + l1->width, l2->posX + l2->width) - newX;
    int newHeight = max(l1->posY + l1->height, l2->posY + l2->height) - newY;

    auto *combinedLayer = new Layer(newWidth, newHeight, newX, newY, 1);
    auto *b = combinedLayer->getPixelData();
    auto *b1 = l1->getPixelData();
    auto *b2 = l2->getPixelData();

    for (int i = 0; i < newWidth * newHeight; i++) b[i] = 0x00000000;

    blendBuffers(b, b1, newWidth, newHeight, l1->width, l1->height, l1->posX - newX, l1->posY - newY);
    blendBuffers(b, b2, newWidth, newHeight, l2->width, l2->height, l2->posX - newX, l2->posY - newY);

    delete l1;
    layers[index1] = combinedLayer;
    deletetAt(index2);
}
