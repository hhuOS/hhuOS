//
// Created by Rafael Reip on 21.10.24.
//

#include "Layers.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "lib/util/graphic/stb_image.h"
#include "lib/util/graphic/stb_image_write.h"

Layers::Layers() {
    this->layers = new Layer *[20];
    this->layerCount = 0;
    this->maxLayerCount = 20;
    this->currentLayer = 0;
}

Layer *Layers::current() {
    return layers[this->currentLayer];
}

void Layers::addEmpty(int width, int height, int posX, int posY) {
    if (this->layerCount >= this->maxLayerCount) {
        return;
    }
    this->layers[this->layerCount] = new Layer(width, height, posX, posY, 1);
    this->layerCount++;
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
    this->layers[this->layerCount] = new Layer(width, height, posX, posY, 1, argbData);
    this->layerCount++;
    stbi_image_free(img);
}

Layer *Layers::at(int index) {
    if (index >= this->layerCount) {
        return nullptr;
    }
    return this->layers[index];
}

int Layers::countNum() const {
    return this->layerCount;
}

int Layers::currentNum() const {
    return this->currentLayer;
}

int Layers::maxNum() const {
    return this->maxLayerCount;
}

void Layers::setCurrent(int index) {
    if (index < 0 || index >= this->layerCount) {
        return;
    }
    this->currentLayer = index;
}

void Layers::setCurrentToNext() {
    this->currentLayer++;
    if (this->currentLayer >= this->layerCount) {
        this->currentLayer = 0;
    }
}

