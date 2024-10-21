//
// Created by Rafael Reip on 21.10.24.
//

#include "Layers.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "lib/util/graphic/stb_image.h"
#include "lib/util/graphic/stb_image_write.h"

Layers::Layers() {
    this->layers = new Layer *[15];
    this->layerCount = 0;
    this->maxLayerCount = 15;
    this->currentLayer = 0;
}

Layer *Layers::current() {
    return layers[currentLayer];
}

void Layers::addEmpty(int width, int height, int posX, int posY) {
    if (layerCount >= maxLayerCount) {
        return;
    }
    layers[layerCount] = new Layer(width, height, posX, posY, 1);
    layerCount++;
}

void Layers::addPicture(const char *path, int posX, int posY) {
    int width, height, channels;
    unsigned char *img = stbi_load(path, &width, &height, &channels, 0);
    print("Loaded image with width " << width << ", height " << height << ", and channels " << channels);
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
        return;
    }
    delete layers[index];
    for (int i = index; i < layerCount - 1; i++) {
        layers[i] = layers[i + 1];
    }
    layerCount--;
}

void Layers::swap(int index1, int index2) {
    if (index1 < 0 || index1 >= layerCount || index2 < 0 || index2 >= layerCount || index1 == index2) {
        return;
    }
    Layer *temp = layers[index1];
    layers[index1] = layers[index2];
    layers[index2] = temp;
}

void Layers::combine(int index1, int index2) {
// TODO
}

