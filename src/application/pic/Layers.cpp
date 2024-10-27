//
// Created by Rafael Reip on 21.10.24.
//

#include "Layers.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "lib/util/graphic/stb_image.h"
#include "lib/util/graphic/stb_image_write.h"

Layers::Layers(MessageHandler *mHandler) {
    this->layers = new Layer *[18];
    this->layerCount = 0;
    this->maxLayerCount = 18;
    this->currentLayer = 0;
    this->mHandler = mHandler;
}

Layer *Layers::current() {
    return layers[currentLayer];
}

void Layers::addEmpty(int posX, int posY, int width, int height) {
    if (layerCount >= maxLayerCount) {
        mHandler->addMessage(Util::String::format("Maximum number of layers reached: %d", maxLayerCount).operator const char *());
        return;
    }
    layers[layerCount] = new Layer(width, height, posX, posY, 1);
    layerCount++;
    currentLayer = layerCount - 1;
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
    currentLayer = layerCount - 1;
}

void Layers::exportPicture(const char *path, int x, int y, int w, int h, bool png, bool jpg, bool bmp) {
    if (w < 0) w = -w, x -= w;
    if (h < 0) h = -h, y -= h;

    auto *pixels = new uint32_t[w * h];
    for (int i = 0; i < w * h; i++) pixels[i] = 0x00000000;

    for (int i = 0; i < layerCount; i++) {
        Layer *layer = layers[i];
        blendBuffers(pixels, layer->getPixelData(), w, h, layer->width, layer->height, layer->posX - x, layer->posY - y);
    }

    int channels = 4;
    auto *rgbaData = new unsigned char[w * h * channels];
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int i = y * w + x;
            int j = y * w + x;
            uint32_t pixel = pixels[i];
            rgbaData[j * channels] = (pixel >> 16) & 0xFF;     // R
            rgbaData[j * channels + 1] = (pixel >> 8) & 0xFF;  // G
            rgbaData[j * channels + 2] = pixel & 0xFF;         // B
            rgbaData[j * channels + 3] = (pixel >> 24) & 0xFF; // A
        }
    }

    if (png && !jpg && !bmp) {
        Util::String filename = Util::String::format("%s.png", path);
        stbi_write_png(filename.operator const char *(), w, h, channels, rgbaData, w * channels);
    } else if (jpg && !png && !bmp) {
        Util::String filename = Util::String::format("%s.jpg", path);
        stbi_write_jpg(filename.operator const char *(), w, h, channels, rgbaData, 100);
    } else if (bmp && !png && !jpg) {
        Util::String filename = Util::String::format("%s.bmp", path);
        stbi_write_bmp(filename.operator const char *(), w, h, channels, rgbaData);
    } else {
        mHandler->addMessage("Invalid export format");
    }

    delete[] rgbaData;
    delete[] pixels;
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
    if (index == currentLayer) currentLayer = 0;
    delete layers[index];
    for (int i = index; i < layerCount - 1; i++) {
        layers[i] = layers[i + 1];
    }
    layerCount--;
    if (layerCount == 0) addEmpty(0, 0, 100, 100);
    if (currentLayer >= layerCount) currentLayer = layerCount - 1;
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

void Layers::duplicate(int index) {
    if (layerCount >= maxLayerCount) {
        mHandler->addMessage(Util::String::format("Maximum number of layers reached: %d", maxLayerCount).operator const char *());
        return;
    }
    if (index < 0 || index >= layerCount) {
        mHandler->addMessage(Util::String::format("Layer index out of bounds: %d", index).operator const char *());
        return;
    }
    Layer *layer = layers[index];
    auto *newPixelData = new uint32_t[layer->width * layer->height];
    for (int i = 0; i < layer->width * layer->height; i++) newPixelData[i] = layer->getPixelData()[i];
    layers[layerCount] = new Layer(layer->width, layer->height, layer->posX, layer->posY, 1, newPixelData);
    layerCount++;
    currentLayer = layerCount - 1;
}

void Layers::move(int index, int x, int y) {
    if (index < 0 || index >= layerCount) return;
    Layer *layer = layers[index];
    layer->posX = x;
    layer->posY = y;
}

void Layers::moveCurrent(int x, int y) {
    move(currentLayer, x, y);
}

void Layers::scale(int index, double factor, ToolCorner kind) {
    if (index < 0 || index >= layerCount) return;
    Layer *layer = layers[index];

    if (factor <= 0) return;
    int newWidth = ceil(layer->width * factor);
    int newHeight = ceil(layer->height * factor);
    auto *newPixelData = new uint32_t[newWidth * newHeight];

    for (int y = 0; y < newHeight; ++y) {
        for (int x = 0; x < newWidth; ++x) {
            int oldX = floor(x / factor);
            int oldY = floor(y / factor);
            if (oldX < layer->width && oldY < layer->height) {
                newPixelData[y * newWidth + x] = layer->getPixel(oldX, oldY);
            } else {
                newPixelData[y * newWidth + x] = 0;
            }
        }
    }

    int newX = layer->posX;
    int newY = layer->posY;
    if (kind == TOP_LEFT || kind == BOTTOM_LEFT)
        newX = layer->posX + layer->width - newWidth;
    if (kind == TOP_LEFT || kind == TOP_RIGHT)
        newY = layer->posY + layer->height - newHeight;

    layer->setNewBuffer(newPixelData, newX, newY, newWidth, newHeight);
}

void Layers::scaleCurrent(double factor, ToolCorner kind) {
    scale(currentLayer, factor, kind);
}

void Layers::crop(int index, int left, int right, int top, int bottom) {
    if (index < 0 || index >= layerCount) return;
    Layer *layer = layers[index];

    int newWidth = layer->width - left - right;
    int newHeight = layer->height - top - bottom;
    if (newWidth <= 0 || newHeight <= 0) return;

    auto *newPixelData = new uint32_t[newWidth * newHeight];
    for (int i = 0; i < newWidth * newHeight; ++i) newPixelData[i] = 0x00000000;

    for (int y = 0; y < newHeight; ++y) {
        for (int x = 0; x < newWidth; ++x) {
            int oldX = x + left, oldY = y + top;
            if (oldX >= 0 && oldX < layer->width && oldY >= 0 && oldY < layer->height) {
                newPixelData[y * newWidth + x] = layer->getPixel(oldX, oldY);
            }
        }
    }

    layer->setNewBuffer(newPixelData, layer->posX + left, layer->posY + top, newWidth, newHeight);
}

void Layers::cropCurrent(int left, int right, int top, int bottom) {
    crop(currentLayer, left, right, top, bottom);
}

void Layers::autoCrop(int index) {
    if (index < 0 || index >= layerCount) return;
    Layer *layer = layers[index];

    int left = 0, right = 0, top = 0, bottom = 0;
    for (int x = 0; x < layer->width; x++) {
        bool hasNonAlpha = false;
        for (int y = 0; y < layer->height; y++) {
            if ((layer->getPixel(x, y) >> 24) & 0xFF) {
                hasNonAlpha = true;
                break;
            }
        }
        if (hasNonAlpha) break;
        left++;
    }
    for (int x = layer->width - 1; x >= left; x--) {
        bool hasNonAlpha = false;
        for (int y = 0; y < layer->height; y++) {
            if ((layer->getPixel(x, y) >> 24) & 0xFF) {
                hasNonAlpha = true;
                break;
            }
        }
        if (hasNonAlpha) break;
        right++;
    }
    for (int y = 0; y < layer->height; y++) {
        bool hasNonAlpha = false;
        for (int x = left; x < layer->width - right; x++) {
            if ((layer->getPixel(x, y) >> 24) & 0xFF) {
                hasNonAlpha = true;
                break;
            }
        }
        if (hasNonAlpha) break;
        top++;
    }
    for (int y = layer->height - 1; y >= top; y--) {
        bool hasNonAlpha = false;
        for (int x = left; x < layer->width - right; x++) {
            if ((layer->getPixel(x, y) >> 24) & 0xFF) {
                hasNonAlpha = true;
                break;
            }
        }
        if (hasNonAlpha) break;
        bottom++;
    }

    crop(index, left, right, top, bottom);
}

void Layers::autoCropCurrent() {
    autoCrop(currentLayer);
}

void Layers::rotate(int index, int degree) {
    if (index < 0 || index >= layerCount) return;
    Layer *layer = layers[index];

    degree = (degree % 360 + 360) % 360;  // Normalize degree to 0-359
    if (degree == 0) return;  // No rotation needed

    int newWidth = abs(layer->width * cos(degree * PI / 180.0)) + abs(layer->height * sin(degree * PI / 180.0));
    int newHeight = abs(layer->width * sin(degree * PI / 180.0)) + abs(layer->height * cos(degree * PI / 180.0));
    auto *newPixelData = new uint32_t[newWidth * newHeight];
    for (int i = 0; i < newWidth * newHeight; ++i) newPixelData[i] = 0x00000000;

    double radians = degree * PI / 180.0;
    double cosTheta = cos(radians);
    double sinTheta = sin(radians);

    int centerX = layer->width / 2;
    int centerY = layer->height / 2;
    int newCenterX = newWidth / 2;
    int newCenterY = newHeight / 2;

    for (int y = 0; y < newHeight; ++y) {
        for (int x = 0; x < newWidth; ++x) {
            int srcX = static_cast<int>((x - newCenterX) * cosTheta + (y - newCenterY) * sinTheta) + centerX;
            int srcY = static_cast<int>(-(x - newCenterX) * sinTheta + (y - newCenterY) * cosTheta) + centerY;

            if (srcX >= 0 && srcX < layer->width && srcY >= 0 && srcY < layer->height) {
                newPixelData[y * newWidth + x] = layer->getPixel(srcX, srcY);
            }
        }
    }

    layer->setNewBuffer(newPixelData,
                        layer->posX - (newWidth - layer->width) / 2, layer->posY - (newHeight - layer->height) / 2,
                        newWidth, newHeight);
}

void Layers::rotateCurrent(int degree) {
    rotate(currentLayer, degree);
}

void Layers::drawCircle(int index, int x, int y, uint32_t color, int thickness) {
    if (index < 0 || index >= layerCount) return;
    Layer *layer = layers[index];

    int r = thickness / 2;
    int rr = r * r;
    for (int ty = -r; ty <= r; ty++) {
        for (int tx = -r; tx <= r; tx++) {
            if (tx * tx + ty * ty <= rr) {
                int px = x + tx;
                int py = y + ty;
                if (px >= 0 && px < layer->width && py >= 0 && py < layer->height) {
                    uint32_t oldColor = layer->getTempPixel(px, py);
                    layer->setPixel(px, py, color == 0x00000000 ? 0x00000000 : blendPixels(oldColor, color));
                }
            }
        }
    }
}

void Layers::drawCircleCurrent(int x, int y, uint32_t color, int thickness) {
    drawCircle(currentLayer, x, y, color, thickness);
}

void Layers::drawLine(int index, int x1, int y1, int x2, int y2, uint32_t color, int thickness) {
    if (index < 0 || index >= layerCount) return;

    // Bressenham's line algorithm
    // https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm#cite_note-Zingl-3
    // http://members.chello.at/~easyfilter/Bresenham.pdf
    // seite Page 13 of 98

    int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = dx + dy, e2;

    while (true) {
        drawCircle(index, x1, y1, color, thickness);
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 >= dy) err += dy, x1 += sx;
        if (e2 <= dx) err += dx, y1 += sy;
    }
}

void Layers::drawLineCurrent(int x1, int y1, int x2, int y2, uint32_t color, int thickness) {
    drawLine(currentLayer, x1, y1, x2, y2, color, thickness);
}

void Layers::prepareNextDrawing(int index) {
    if (index < 0 || index >= layerCount) return;
    Layer *layer = layers[index];
    layer->prepareNextDrawing();
}

void Layers::prepareNextDrawingCurrent() {
    prepareNextDrawing(currentLayer);
}
