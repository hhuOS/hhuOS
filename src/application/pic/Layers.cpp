//
// Created by Rafael Reip on 21.10.24.
//

#include "Layers.h"

#include "Layer.h"
#include "History.h"
#include "MessageHandler.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_ASSERT(x) ((void)0)  // Do nothing when assert fails

#include "lib/util/graphic/stb_image.h"
#include "lib/util/graphic/stb_image_write.h"

/**
 * Constructor for the Layers class.
 *
 * @param mHandler Pointer to the MessageHandler object.
 * @param history Pointer to the History object.
 */
Layers::Layers(MessageHandler *mHandler, History *history) {
    this->layers = new Layer *[18];
    this->layerCount = 0;
    this->maxLayerCount = 18; // fits onto minimum screen resolution of 800x600
    this->currentLayer = -1;
    this->mHandler = mHandler;
    this->history = history;
}

/**
 * Destructor for the Layers class.
 */
Layers::~Layers() {
    delete[] layers;
}

/**
 * Resets the layers by deleting all existing layers and setting the layer count and current layer to initial values.
 */
void Layers::reset() {
    for (int i = 0; i < layerCount; i++) {
        delete layers[i];
    }
    layerCount = 0;
    currentLayer = -1;
}

/**
 * Returns the current layer.
 *
 * @return Pointer to the current Layer object, or nullptr if no current layer is set.
 */
Layer *Layers::current() const {
    if (currentLayer == -1) return nullptr;
    return layers[currentLayer];
}

/**
 * \brief Adds an empty layer to the Layers object.
 *
 * @param posX The x-coordinate of the layer's position.
 * @param posY The y-coordinate of the layer's position.
 * @param width The width of the layer.
 * @param height The height of the layer.
 * @param writeHistory A boolean indicating whether to write this action to the history.
 */
void Layers::addEmpty(int posX, int posY, int width, int height, bool writeHistory) {
    if (layerCount >= maxLayerCount) {
        mHandler->addMessage(Util::String::format("Maximum number of layers reached: %d", maxLayerCount).operator const char *());
        return;
    }
    layers[layerCount] = new Layer(width, height, posX, posY, 1);
    layerCount++;
    currentLayer = layerCount - 1;
    if (writeHistory)
        history->addCommand(Util::String::format("addEmpty %d %d %d %d", posX, posY, width, height), &layers, &layerCount);
}

/**
 * \brief Adds a picture layer to the Layers object.
 *
 * This function loads an image from the specified file path using the stb_image.h library and adds it as a new layer to the Layers object.
 *
 * @param path The file path to the image.
 * @param posX The x-coordinate of the layer's position.
 * @param posY The y-coordinate of the layer's position.
 * @param writeHistory A boolean indicating whether to write this action to the history.
 */
void Layers::addPicture(const char *path, int posX, int posY, bool writeHistory) {
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
    for (int y = 0; y < height; y++) { // flip image vertically to match screen coordinates
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
    if (writeHistory)
        history->addCommand(Util::String::format("addPicture %s %d %d", path, posX, posY), &layers, &layerCount);
}

/**
 * \brief Exports the final picture (all layers) to a file.
 *
 * This function exports the current layers to an image file in the specified format (PNG, JPG, or BMP) using the stb_image_write.h library.
 *
 * \param path The file path to save the image.
 * \param x The x-coordinate of the top-left corner of the export area.
 * \param y The y-coordinate of the top-left corner of the export area.
 * \param w The width of the export area.
 * \param h The height of the export area.
 * \param png Boolean flag to export as PNG.
 * \param jpg Boolean flag to export as JPG.
 * \param bmp Boolean flag to export as BMP.
 */
void Layers::exportPicture(const char *path, int x, int y, int w, int h, bool png, bool jpg, bool bmp) {
    if (w < 0) w = -w, x -= w;
    if (h < 0) h = -h, y -= h;

    auto *pixels = new uint32_t[w * h];
    for (int i = 0; i < w * h; i++) pixels[i] = 0x00000000; // clear buffer

    for (int i = 0; i < layerCount; i++) { // render layers to buffer
        Layer *layer = layers[i];
        blendBuffers(pixels, layer->getPixelData(), w, h, layer->width, layer->height, layer->posX - x, layer->posY - y);
    }

    int channels = 4;
    auto *rgbaData = new unsigned char[w * h * channels];
    for (int yy = 0; yy < h; yy++) { // convert ARGB to RGBA
        for (int xx = 0; xx < w; xx++) {
            int i = yy * w + xx;
            uint32_t pixel = pixels[i];
            rgbaData[i * channels] = (pixel >> 16) & 0xFF;         // R
            rgbaData[i * channels + 1] = (pixel >> 8) & 0xFF;    // G
            rgbaData[i * channels + 2] = pixel & 0xFF;               // B
            rgbaData[i * channels + 3] = (pixel >> 24) & 0xFF; // A
        }
    }

    if (png && !jpg && !bmp) { // export image as PNG
        Util::String filename = Util::String::format("%s.png", path);
        stbi_write_png(filename.operator const char *(), w, h, channels, rgbaData, w * channels);
    } else if (jpg && !png && !bmp) { // export image as JPG
        Util::String filename = Util::String::format("%s.jpg", path);
        stbi_write_jpg(filename.operator const char *(), w, h, channels, rgbaData, 100);
    } else if (bmp && !png && !jpg) { // export image as BMP
        Util::String filename = Util::String::format("%s.bmp", path);
        stbi_write_bmp(filename.operator const char *(), w, h, channels, rgbaData);
    } else {
        mHandler->addMessage("Invalid export format");
    }

    delete[] rgbaData;
    delete[] pixels;
}

/**
 * Returns the layer at the specified index.
 *
 * @param index The index of the layer to retrieve.
 * @return Pointer to the Layer object at the specified index, or nullptr if the index is out of bounds.
 */
Layer *Layers::at(int index) {
    if (index >= layerCount) {
        mHandler->addMessage(Util::String::format("Layers::at(%d) index out of bounds", index).operator const char *());
        return nullptr;
    }
    return layers[index];
}

/**
 * Returns the current number of layers.
 *
 * @return The current number of layers.
 */
int Layers::countLayersNum() const {
    return layerCount;
}

/**
 * Returns the index of the current layer.
 *
 * @return The index of the current layer.
 */
int Layers::currentLayerNum() const {
    return currentLayer;
}

/**
 * Returns the maximum number of layers.
 *
 * @return The maximum number of layers.
 */
int Layers::maxLayersNum() const {
    return maxLayerCount;
}

/**
 * Sets the current layer to the specified index.
 *
 * @param index The index to set as the current layer.
 */
void Layers::setCurrent(int index) {
    if (index < 0 || index >= layerCount) {
        mHandler->addMessage(Util::String::format("Layers::setCurrent(%d) index out of bounds", index).operator const char *());
        return;
    }
    currentLayer = index;
}

/**
 * Sets the current layer to the next layer in the list.
 * If the current layer is the last one, it wraps around to the first layer.
 */
void Layers::setCurrentToNext() {
    currentLayer++;
    if (currentLayer >= layerCount) {
        currentLayer = 0;
    }
}

/**
 * Deletes the layer at the specified index.
 *
 * @param index The index of the layer to delete.
 * @param writeHistory A boolean indicating whether to write this action to the history.
 */
void Layers::deleteAt(int index, bool writeHistory) {
    if (index < 0 || index >= layerCount) {
        mHandler->addMessage(Util::String::format("Layers::deleteAt(%d) index out of bounds", index).operator const char *());
        return;
    }
    if (index == currentLayer) currentLayer = 0;
    delete layers[index];
    for (int i = index; i < layerCount - 1; i++) {
        layers[i] = layers[i + 1];
    }
    layerCount--;
    if (currentLayer >= layerCount) currentLayer = layerCount - 1; // if current does not exist anymore
    if (writeHistory)
        history->addCommand(Util::String::format("delete %d", index), &layers, &layerCount);
}

/**
 * \brief Swaps the positions of two layers in the Layers object.
 *
 * This function swaps the positions of the layers at the specified indices.
 * If either index is out of bounds or the indices are the same, an error message is added to the MessageHandler.
 *
 * \param index1 The index of the first layer.
 * \param index2 The index of the second layer.
 * \param writeHistory A boolean indicating whether to write this action to the history.
 */
void Layers::swap(int index1, int index2, bool writeHistory) {
    if (index1 < 0 || index1 >= layerCount || index2 < 0 || index2 >= layerCount || index1 == index2) {
        mHandler->addMessage(Util::String::format("Layers::swap(%d, %d) invalid layer indices", index1, index2).operator const char *());
        return;
    }
    Layer *temp = layers[index1];
    layers[index1] = layers[index2];
    layers[index2] = temp;
    if (currentLayer == index1) currentLayer = index2;
    else if (currentLayer == index2) currentLayer = index1;
    if (writeHistory)
        history->addCommand(Util::String::format("swap %d %d", index1, index2), &layers, &layerCount);
}

/**
 * Toggles the visibility of the layer at the specified index.
 *
 * @param index The index of the layer to toggle visibility.
 * @param writeHistory A boolean indicating whether to write this action to the history.
 */
void Layers::changeVisibleAt(int index, bool writeHistory) {
    if (index < 0 || index >= layerCount) {
        mHandler->addMessage(Util::String::format("Layers::changeVisibleAt(%d) index out of bounds", index).operator const char *());
        return;
    }
    layers[index]->isVisible = !layers[index]->isVisible;
    if (writeHistory)
        history->addCommand(Util::String::format("visible %d", index), &layers, &layerCount);
}

/**
 * \brief Combines two layers into one.
 *
 * This function merges the two specified layers into a single layer. The new layer will have a size that encompasses both original layers.
 * Both  layers are deleted, and the first layer is replaced with the combined layer.
 *
 * \param index1 The index of the first layer.
 * \param index2 The index of the second layer.
 * \param writeHistory A boolean indicating whether to write this action to the history.
 */
void Layers::combine(int index1, int index2, bool writeHistory) {
    if (index1 < 0 || index1 >= layerCount || index2 < 0 || index2 >= layerCount || index1 == index2) {
        mHandler->addMessage(Util::String::format("Layers::combine(%d, %d) invalid layer indices", index1, index2));
        return;
    }
    if (index1 > index2) { // to preserve the order of the layers
        int temp = index1;
        index1 = index2;
        index2 = temp;
    }

    // calc new layer size and position
    Layer *l1 = layers[index1], *l2 = layers[index2];
    int newX = min(l1->posX, l2->posX);
    int newY = min(l1->posY, l2->posY);
    int newWidth = max(l1->posX + l1->width, l2->posX + l2->width) - newX;
    int newHeight = max(l1->posY + l1->height, l2->posY + l2->height) - newY;

    auto *combinedLayer = new Layer(newWidth, newHeight, newX, newY, 1);
    auto *combinedPixelData = combinedLayer->getPixelData();
    for (int i = 0; i < newWidth * newHeight; i++) combinedPixelData[i] = 0x00000000; // clear buffer
    // blend both layers into new buffer
    blendBuffers(combinedPixelData, l1->getPixelData(), newWidth, newHeight, l1->width, l1->height, l1->posX - newX, l1->posY - newY);
    blendBuffers(combinedPixelData, l2->getPixelData(), newWidth, newHeight, l2->width, l2->height, l2->posX - newX, l2->posY - newY);

    // clean up
    delete l1;
    delete l2;
    layers[index1] = combinedLayer;
    if (index2 == currentLayer) currentLayer = 0;
    delete layers[index2];
    for (int i = index2; i < layerCount - 1; i++) {
        layers[i] = layers[i + 1];
    }
    layerCount--;
    if (currentLayer >= layerCount) currentLayer = layerCount - 1;

    if (writeHistory)
        history->addCommand(Util::String::format("combine %d %d", index1, index2), &layers, &layerCount);
}

/**
 * \brief Duplicates the layer at the specified index.
 *
 * This function creates a duplicate of the layer at the given index.
 * The new layer will have the same properties and pixel data as the original layer and will appear as the topmost layer.
 *
 * \param index The index of the layer to duplicate.
 * \param writeHistory A boolean indicating whether to write this action to the history.
 */
void Layers::duplicate(int index, bool writeHistory) {
    if (layerCount >= maxLayerCount) {
        mHandler->addMessage(
                Util::String::format("Layers::duplicate(%d) max number of layers reached", maxLayerCount).operator const char *());
        return;
    }
    if (index < 0 || index >= layerCount) {
        mHandler->addMessage(Util::String::format("Layers::duplicate(%d)  index out of bounds", index).operator const char *());
        return;
    }

    Layer *layer = layers[index];
    auto *newPixelData = new uint32_t[layer->width * layer->height];
    for (int i = 0; i < layer->width * layer->height; i++) newPixelData[i] = layer->getPixelData()[i]; // copy pixel data
    layers[layerCount] = new Layer(layer->width, layer->height, layer->posX, layer->posY, 1, newPixelData);
    layerCount++;
    currentLayer = layerCount - 1;

    if (writeHistory)
        history->addCommand(Util::String::format("duplicate %d", index), &layers, &layerCount);
}

/**
 * Moves the layer at the specified index to the given coordinates.
 *
 * @param index The index of the layer to move.
 * @param x The new x-coordinate of the layer.
 * @param y The new y-coordinate of the layer.
 * @param writeHistory A boolean indicating whether to write this action to the history.
 */
void Layers::move(int index, int x, int y, bool writeHistory) {
    if (index < 0 || index >= layerCount) {
        mHandler->addMessage(Util::String::format("Layers::move(%d, %d, %d) index out of bounds", index, x, y).operator const char *());
        return;
    }
    Layer *layer = layers[index];
    layer->posX = x;
    layer->posY = y;
    if (writeHistory)
        history->addCommand(Util::String::format("move %d %d %d", index, x, y), &layers, &layerCount);
}

/**
 * Moves the current layer to the given coordinates.
 *
 * @param x The new x-coordinate of the current layer.
 * @param y The new y-coordinate of the current layer.
 */
void Layers::moveCurrent(int x, int y) {
    move(currentLayer, x, y);
}

/**
 * \brief Scales the layer at the specified index by a given factor.
 *
 * This function scales the layer at the specified index by the given factor.
 * The scaling is performed from the specified corner.
 *
 * \param index The index of the layer to scale.
 * \param factor The scaling factor. Must be greater than 0.
 * \param kind The corner from which to scale (e.g., TOP_LEFT, BOTTOM_LEFT).
 * \param writeHistory A boolean indicating whether to write this action to the history.
 */
void Layers::scale(int index, double factor, ToolCorner kind, bool writeHistory) {
    if (index < 0 || index >= layerCount) {
        auto fac = double_to_string(factor, 2);
        mHandler->addMessage(
                Util::String::format("Layers::scale(%d, %s, %d) index out of bounds", index, fac, kind).operator const char *());
        return;
    }
    if (factor <= 0) {
        auto fac = double_to_string(factor, 2);
        mHandler->addMessage(Util::String::format("Layers::scale(%d, %s, %d) invalid factor", index, fac, kind).operator const char *());
        return;
    }

    // create new buffer with scaled pixel data
    Layer *layer = layers[index];
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

    // set new buffer and update position and dimensions
    int newX = layer->posX;
    int newY = layer->posY;
    if (kind == TOP_LEFT || kind == BOTTOM_LEFT)
        newX = layer->posX + layer->width - newWidth;
    if (kind == TOP_LEFT || kind == TOP_RIGHT)
        newY = layer->posY + layer->height - newHeight;
    layer->setNewBuffer(newPixelData, newX, newY, newWidth, newHeight);

    auto scaleString = double_to_string(factor, 2);
    if (writeHistory)
        history->addCommand(Util::String::format("scale %d %s %d", index, scaleString, kind), &layers, &layerCount);
}

/**
 * \brief Scales the current layer by a given factor.
 *
 * This function scales the current layer by the given factor.
 * The scaling is performed from the specified corner.
 *
 * \param factor The scaling factor. Must be greater than 0.
 * \param kind The corner from which to scale (e.g., TOP_LEFT, BOTTOM_LEFT).
 */
void Layers::scaleCurrent(double factor, ToolCorner kind) {
    scale(currentLayer, factor, kind);
}

/**
 * \brief Crops the layer at the specified index.
 *
 * This function crops the layer at the given index by removing the specified number of pixels
 * from the left, right, top, and bottom edges. The new dimensions of the layer are calculated
 * based on the specified crop values.
 *
 * \param index The index of the layer to crop.
 * \param left The number of pixels to remove from the left edge.
 * \param right The number of pixels to remove from the right edge.
 * \param top The number of pixels to remove from the top edge.
 * \param bottom The number of pixels to remove from the bottom edge.
 * \param writeHistory A boolean indicating whether to write this action to the history.
 */
void Layers::crop(int index, int left, int right, int top, int bottom, bool writeHistory) {
    if (index < 0 || index >= layerCount) {
        mHandler->addMessage(Util::String::format("Layers::crop(%d, %d, %d, %d, %d) index out of bounds", index, left, right, top,
                                                  bottom).operator const char *());
        return;
    }

    Layer *layer = layers[index];
    int newWidth = layer->width - left - right;
    int newHeight = layer->height - top - bottom;
    if (newWidth <= 0 || newHeight <= 0) {
        mHandler->addMessage(Util::String::format("Layers::crop(%d, %d, %d, %d, %d) invalid crop",
                                                  index, left, right, top, bottom).operator const char *());
        return;
    }

    // create new buffer with cropped pixel data
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

    // set new buffer and update position and dimensions
    layer->setNewBuffer(newPixelData, layer->posX + left, layer->posY + top, newWidth, newHeight);

    if (writeHistory)
        history->addCommand(Util::String::format("crop %d %d %d %d %d", index, left, right, top, bottom), &layers, &layerCount);
}

/**
 * Crops the current layer by removing the specified number of pixels
 * from the left, right, top, and bottom edges.
 *
 * @param left The number of pixels to remove from the left edge.
 * @param right The number of pixels to remove from the right edge.
 * @param top The number of pixels to remove from the top edge.
 * @param bottom The number of pixels to remove from the bottom edge.
 */
void Layers::cropCurrent(int left, int right, int top, int bottom) {
    crop(currentLayer, left, right, top, bottom);
}

/**
 * \brief Automatically crops the layer at the specified index to remove transparent pixels and only show used pixels.
 *
 * This function calculates the amount of transparent pixels on each side of the layer
 * and crops the layer accordingly.
 *
 * \param index The index of the layer to auto-crop.
 * \param writeHistory A boolean indicating whether to write this action to the history.
 */
void Layers::autoCrop(int index, bool writeHistory) {
    if (index < 0 || index >= layerCount) {
        mHandler->addMessage(Util::String::format("Layers::autoCrop(%d) index out of bounds", index).operator const char *());
        return;
    }

    Layer *layer = layers[index];
    int left = 0, right = 0, top = 0, bottom = 0;
    for (int x = 0; x < layer->width; x++) { // calculate amount of transparent pixels columns on left side
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
    for (int x = layer->width - 1; x >= left; x--) { // calculate amount of transparent pixels columns on right side
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
    for (int y = 0; y < layer->height; y++) { // calculate amount of transparent pixels rows on top side
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
    for (int y = layer->height - 1; y >= top; y--) { // calculate amount of transparent pixels rows on bottom side
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

    // crop layer according to calculated values
    crop(index, left, right, top, bottom);

    if (writeHistory)
        history->addCommand(Util::String::format("autoCrop %d", index), &layers, &layerCount);
}

/**
 * \brief Automatically crops the current layer to remove transparent pixels and only show used pixels.
 *
 * This function calls autoCrop on the current layer.
 */
void Layers::autoCropCurrent() {
    autoCrop(currentLayer);
}

/**
 * \brief Rotates the layer at the specified index by a given degree.
 *
 * This function rotates the layer at the specified index by the given degree.
 * The rotation is performed around the center of the layer.
 *
 * \param index The index of the layer to rotate.
 * \param degree The degree by which to rotate the layer. Must be between 0 and 359.
 * \param writeHistory A boolean indicating whether to write this action to the history.
 */
void Layers::rotate(int index, int degree, bool writeHistory) {
    if (index < 0 || index >= layerCount) {
        mHandler->addMessage(Util::String::format("Layers::rotate(%d, %d) index out of bounds", index, degree).operator const char *());
        return;
    }
    Layer *layer = layers[index];

    degree = (degree % 360 + 360) % 360;  // Normalize degree to 0-359
    if (degree == 0) return;  // No rotation needed

    // prepare new buffer
    int newWidth = abs(static_cast<int>(layer->width * cos(degree * PI / 180.0))) + abs(static_cast<int>(layer->height * sin(degree * PI / 180.0)));
    int newHeight = abs(static_cast<int>(layer->width * sin(degree * PI / 180.0))) + abs(static_cast<int>(layer->height * cos(degree * PI / 180.0)));
    auto *newPixelData = new uint32_t[newWidth * newHeight];
    for (int i = 0; i < newWidth * newHeight; ++i) newPixelData[i] = 0x00000000; // clear buffer

    // rotate pixel data
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

    // set new buffer and update position and dimensions
    layer->setNewBuffer(newPixelData, layer->posX - (newWidth - layer->width) / 2, layer->posY - (newHeight - layer->height) / 2,
                        newWidth, newHeight);

    if (writeHistory)
        history->addCommand(Util::String::format("rotate %d %d", index, degree), &layers, &layerCount);
}

/**
 * \brief Rotates the current layer by a given degree.
 *
 * This function calls rotate on the current layer.
 *
 * \param degree The degree by which to rotate the current layer. Must be between 0 and 359.
 */
void Layers::rotateCurrent(int degree) {
    rotate(currentLayer, degree);
}

/**
 * \brief Draws a filled circle on the specified layer.
 *
 * This function draws a filled circle with the given color and thickness at the specified coordinates on the layer.
 *
 * \param index The index of the layer on which to draw the circle.
 * \param x The x-coordinate of the center of the circle.
 * \param y The y-coordinate of the center of the circle.
 * \param color The color of the circle in ARGB format.
 * \param thickness The diameter of the circle.
 */
void Layers::drawCircle(int index, int x, int y, uint32_t color, int thickness) const {
    if (index < 0 || index >= layerCount) return;
    Layer *layer = layers[index];

    int r = thickness / 2;
    int rr = r * r;
    for (int ty = -r; ty <= r; ty++) { // square with side length thickness around the center
        for (int tx = -r; tx <= r; tx++) {
            if (tx * tx + ty * ty <= rr) { // check if point is inside circle
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

/**
 * \brief Draws a filled circle on the current layer.
 *
 * This function draws a filled circle with the given color and thickness at the specified coordinates on the current layer.
 *
 * \param x The x-coordinate of the center of the circle.
 * \param y The y-coordinate of the center of the circle.
 * \param color The color of the circle in ARGB format.
 * \param thickness The diameter of the circle.
 */
void Layers::drawCircleCurrent(int x, int y, uint32_t color, int thickness) const {
    drawCircle(currentLayer, x, y, color, thickness);
}

/**
 * \brief Draws a line on the specified layer.
 *
 * This function uses Bresenham's line algorithm to draw a line from (x1, y1) to (x2, y2) with the specified color and thickness.
 *
 * \param index The index of the layer on which to draw the line.
 * \param x1 The x-coordinate of the start point of the line.
 * \param y1 The y-coordinate of the start point of the line.
 * \param x2 The x-coordinate of the end point of the line.
 * \param y2 The y-coordinate of the end point of the line.
 * \param color The color of the line in ARGB format.
 * \param thickness The thickness of the line.
 * \param writeHistory A boolean indicating whether to write this action to the history.
 */
void Layers::drawLine(int index, int x1, int y1, int x2, int y2, uint32_t color, int thickness, bool writeHistory) {
    if (index < 0 || index >= layerCount) return;
    int x1c = x1, y1c = y1, x2c = x2, y2c = y2; // for history

    // Bresenham's line algorithm
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

    if (writeHistory)
        history->addCommand(Util::String::format("line %d %d %d %d %d %d %d",
                                                 index, x1c, y1c, x2c, y2c, color, thickness), &layers, &layerCount);
}

/**
 * \brief Draws a line on the current layer.
 *
 * This function calls drawLine on the current layer with the specified parameters.
 *
 * \param x1 The x-coordinate of the start point of the line.
 * \param y1 The y-coordinate of the start point of the line.
 * \param x2 The x-coordinate of the end point of the line.
 * \param y2 The y-coordinate of the end point of the line.
 * \param color The color of the line in ARGB format.
 * \param thickness The thickness of the line.
 */
void Layers::drawLineCurrent(int x1, int y1, int x2, int y2, uint32_t color, int thickness) {
    drawLine(currentLayer, x1, y1, x2, y2, color, thickness);
}

/**
 * Prepares the next drawing operation on the specified layer to indicate the current line is finished.
 *
 * @param index The index of the layer to prepare for the next drawing.
 * @param writeHistory A boolean indicating whether to write this action to the history.
 */
void Layers::prepareNextDrawing(int index, bool writeHistory) {
    if (index < 0 || index >= layerCount) {
        mHandler->addMessage(Util::String::format("Layers::prepareNextDrawing(%d) index out of bounds", index).operator const char *());
        return;
    }
    Layer *layer = layers[index];
    layer->prepareNextDrawing();
    if (writeHistory)
        history->addCommand(Util::String::format("prepareNextDrawing %d", index), &layers, &layerCount);
}

/**
 * Prepares the next drawing operation on the current layer to indicate the current line is finished.
 */
void Layers::prepareNextDrawingCurrent(bool writeHistory) {
    prepareNextDrawing(currentLayer, writeHistory);
}

/**
 * \brief Draws a shape on the specified layer.
 *
 * This function draws a shape (square, rectangle, circle, or ellipse) with the given color and dimensions
 * at the specified coordinates on the layer.
 *
 * \param index The index of the layer on which to draw the shape.
 * \param shape The shape to draw (e.g., SQUARE, RECTANGLE, CIRCLE, ELLIPSE).
 * \param x The x-coordinate of the top-left corner of the shape.
 * \param y The y-coordinate of the top-left corner of the shape.
 * \param w The width of the shape.
 * \param h The height of the shape.
 * \param color The color of the shape in ARGB format.
 * \param writeHistory A boolean indicating whether to write this action to the history.
 */
void Layers::drawShape(int index, Shape shape, int x, int y, int w, int h, uint32_t color, bool writeHistory) {
    Layer *l = layers[index];
    int size = max(abs(w), abs(h));
    if (w < 0) w = -w, x -= (shape == Shape::CIRCLE || shape == Shape::SQUARE) ? size : w;
    if (h < 0) h = -h, y -= (shape == Shape::CIRCLE || shape == Shape::SQUARE) ? size : h;

    if (shape == Shape::SQUARE || shape == Shape::RECTANGLE) {
        if (shape == Shape::SQUARE) w = size, h = size;
        for (int px = x; px < x + w; px++) { // square with side length w and h around the center
            for (int py = y; py < y + h; py++) {
                uint32_t oldColor = l->getPixel(px, py);
                l->setPixel(px, py, color == 0x00000000 ? 0x00000000 : blendPixels(oldColor, color));
            }
        }
    } else if (shape == Shape::CIRCLE || shape == Shape::ELLIPSE) {
        if (shape == Shape::CIRCLE) w = size, h = size;
        double a = w / 2.0;
        double b = h / 2.0;
        double centerX = x + a;
        double centerY = y + b;
        for (int px = x; px < x + w; px++) { // square with side length w and h around the center
            for (int py = y; py < y + h; py++) {
                double p = ((px - centerX) * (px - centerX)) / (a * a);
                double q = ((py - centerY) * (py - centerY)) / (b * b);
                if (p + q <= 1.0) { // check if point is inside ellipse
                    uint32_t oldColor = l->getPixel(px, py);
                    l->setPixel(px, py, color == 0x00000000 ? 0x00000000 : blendPixels(oldColor, color));
                }
            }
        }
    }

    if (writeHistory)
        history->addCommand(Util::String::format("shape %d %d %d %d %d %d %d", index, shape, x, y, w, h, color), &layers, &layerCount);
}

/**
 * \brief Draws a shape on the current layer.
 *
 * This function calls drawShape on the current layer with the specified parameters.
 *
 * \param shape The shape to draw (e.g., SQUARE, RECTANGLE, CIRCLE, ELLIPSE).
 * \param x The x-coordinate of the top-left corner of the shape.
 * \param y The y-coordinate of the top-left corner of the shape.
 * \param w The width of the shape.
 * \param h The height of the shape.
 * \param color The color of the shape in ARGB format.
 */
void Layers::drawShapeCurrent(Shape shape, int x, int y, int w, int h, uint32_t color) {
    drawShape(currentLayer, shape, x, y, w, h, color);
}

/**
 * \brief Replaces the color at the specified coordinates with a new color.
 *
 * This function replaces the color at the given coordinates (x, y) on the specified layer
 * with the provided penColor. The replacement is done for all pixels within the specified
 * tolerance level.
 *
 * \param index The index of the layer to modify.
 * \param x The x-coordinate of the pixel to replace.
 * \param y The y-coordinate of the pixel to replace.
 * \param penColor The new color to apply in ARGB format.
 * \param tolerance The tolerance level for color replacement (0.0 to 1.0).
 * \param writeHistory A boolean indicating whether to write this action to the history.
 */
void Layers::replaceColor(int index, int x, int y, uint32_t penColor, double tolerance, bool writeHistory) {
    if (index < 0 || index >= layerCount) {
        mHandler->addMessage(Util::String::format("Layers::replaceColor(%d, %d, %d, %d, %d, %f) index out of bounds",
                                                  index, x, y, penColor, tolerance).operator const char *());
        return;
    }

    Layer *layer = layers[index];
    uint32_t targetColor = layer->getPixel(x, y);
    if (targetColor == penColor) return;
    tolerance = max(0.0, min(1.0, tolerance));
    auto maxDiff = uint32_t(255 * tolerance);
    for (int i = 0; i < layer->width * layer->height; i++) {
        uint32_t pixel = layer->getPixelData()[i];
        uint32_t rDiff = abs(int(((targetColor >> 16) & 0xFF) - ((pixel >> 16) & 0xFF)));
        uint32_t gDiff = abs(int(((targetColor >> 8) & 0xFF) - ((pixel >> 8) & 0xFF)));
        uint32_t bDiff = abs(int((targetColor & 0xFF) - (pixel & 0xFF)));
        if (rDiff <= maxDiff && gDiff <= maxDiff && bDiff <= maxDiff) { // check if pixel is within tolerance and if so replace it
            layer->getPixelData()[i] = penColor;
        }
    }

    auto toleranceString = double_to_string(tolerance, 2);
    if (writeHistory)
        history->addCommand(Util::String::format("replaceColor %d %d %d %d %s",
                                                 index, x, y, penColor, toleranceString), &layers, &layerCount);
}

/**
 * \brief Replaces the color at the specified coordinates with a given tolerance on the current layer.
 *
 * This function calls replaceColor on the current layer with the specified parameters.
 *
 * \param x The x-coordinate of the pixel to replace.
 * \param y The y-coordinate of the pixel to replace.
 * \param penColor The new color to apply in ARGB format.
 * \param tolerance The tolerance level for color replacement (0.0 to 1.0).
 */
void Layers::replaceColorCurrent(int x, int y, uint32_t penColor, double tolerance) {
    replaceColor(currentLayer, x, y, penColor, tolerance);
}

/**
 * \brief Applies a black and white filter to the specified layer.
 *
 * This function converts the colors of the specified layer to grayscale.
 *
 * \param index The index of the layer to apply the filter to.
 * \param writeHistory A boolean indicating whether to write this action to the history.
 */
void Layers::filterBlackWhite(int index, bool writeHistory) {
    if (index < 0 || index >= layerCount) {
        mHandler->addMessage(Util::String::format("Layers::filterBlackWhite(%d) index out of bounds", index).operator const char *());
        return;
    }

    Layer *layer = layers[index];
    for (int i = 0; i < layer->width * layer->height; i++) {
        uint32_t pixel = layer->getPixelData()[i];
        uint32_t a = (pixel >> 24) & 0xFF;
        uint32_t r = (pixel >> 16) & 0xFF;
        uint32_t g = (pixel >> 8) & 0xFF;
        uint32_t b = pixel & 0xFF;
        uint32_t avg = (r + g + b) / 3;
        layer->getPixelData()[i] = ((a << 24) | (avg << 16) | (avg << 8) | avg);
    }

    if (writeHistory)
        history->addCommand(Util::String::format("filterBlackWhite %d", index), &layers, &layerCount);
}

/**
 * \brief Applies a black and white filter to the current layer.
 *
 * This function calls filterBlackWhite on the current layer.
 */
void Layers::filterBlackWhiteCurrent() {
    filterBlackWhite(currentLayer);
}

/**
 * \brief Inverts the colors of the specified layer.
 *
 * This function inverts the colors of the specified layer.
 *
 * \param index The index of the layer to apply the filter to.
 * \param writeHistory A boolean indicating whether to write this action to the history.
 */
void Layers::filterInvert(int index, bool writeHistory) {
    if (index < 0 || index >= layerCount) {
        mHandler->addMessage(Util::String::format("Layers::filterInvert(%d) index out of bounds", index).operator const char *());
        return;
    }

    Layer *layer = layers[index];
    for (int i = 0; i < layer->width * layer->height; i++) {
        uint32_t pixel = layer->getPixelData()[i];
        uint32_t a = (pixel >> 24) & 0xFF;
        uint32_t r = 0xFF - ((pixel >> 16) & 0xFF);
        uint32_t g = 0xFF - ((pixel >> 8) & 0xFF);
        uint32_t b = 0xFF - (pixel & 0xFF);
        layer->getPixelData()[i] = ((a << 24) | (r << 16) | (g << 8) | b);
    }

    if (writeHistory)
        history->addCommand(Util::String::format("filterInvert %d", index), &layers, &layerCount);
}

/**
 * \brief Inverts the colors of the current layer.
 *
 * This function calls filterInvert on the current layer.
 */
void Layers::filterInvertCurrent() {
    filterInvert(currentLayer);
}

/**
 * \brief Applies a sepia filter to the specified layer.
 *
 * This function converts the colors of the specified layer to sepia tones. The formula used is:
 * R = 0.393*R + 0.769*G + 0.189*B;
 * G = 0.349*R + 0.686*G + 0.168*B;
 * B = 0.272*R + 0.534*G + 0.131*B;
 *
 * \param index The index of the layer to apply the filter to.
 * \param writeHistory A boolean indicating whether to write this action to the history.
 */
void Layers::filterSepia(int index, bool writeHistory) {
    if (index < 0 || index >= layerCount) {
        mHandler->addMessage(Util::String::format("Layers::filterSepia(%d) index out of bounds", index).operator const char *());
        return;
    }

    Layer *layer = layers[index];
    for (int i = 0; i < layer->width * layer->height; i++) {
        uint32_t pixel = layer->getPixelData()[i];
        uint32_t a = (pixel >> 24) & 0xFF;
        // floating values from: https://stackoverflow.com/a/9449159/18030268
        uint32_t r = min(0xFF, int((uint32_t) (0.393 * ((pixel >> 16) & 0xFF) + 0.769 * ((pixel >> 8) & 0xFF) + 0.189 * (pixel & 0xFF))));
        uint32_t g = min(0xFF, int((uint32_t) (0.349 * ((pixel >> 16) & 0xFF) + 0.686 * ((pixel >> 8) & 0xFF) + 0.168 * (pixel & 0xFF))));
        uint32_t b = min(0xFF, int((uint32_t) (0.272 * ((pixel >> 16) & 0xFF) + 0.534 * ((pixel >> 8) & 0xFF) + 0.131 * (pixel & 0xFF))));
        layer->getPixelData()[i] = ((a << 24) | (r << 16) | (g << 8) | b);
    }

    if (writeHistory)
        history->addCommand(Util::String::format("filterSepia %d", index), &layers, &layerCount);
}

/**
 * \brief Applies a sepia filter to the current layer.
 *
 * This function calls filterSepia on the current layer.
 */
void Layers::filterSepiaCurrent() {
    filterSepia(currentLayer);
}

/**
 * \brief Applies a convolution filter to the specified layer using the given 3x3 kernel.
 *
 * This function applies a convolution filter to the specified layer using the provided kernel, divisor, and offset.
 * The kernel is a 3x3 matrix that is applied to each pixel in the layer to produce a new pixel value.
 *
 * \param index The index of the layer to apply the filter to.
 * \param kernel The 3x3 kernel matrix to use for the convolution filter.
 * \param divisor The divisor to normalize the kernel values.
 * \param offset The offset to add to the resulting pixel values.
 * \param writeHistory A boolean indicating whether to write this action to the history.
 */
void Layers::filterKernel(int index, int kernel[9], int divisor, int offset, bool writeHistory) {
    if (index < 0 || index >= layerCount) {
        mHandler->addMessage(Util::String::format("Layers::filterKernel(%d, %d, %d, %d) index out of bounds",
                                                  index, kernel, divisor, offset).operator const char *());
        return;
    }

    // prepare new buffer
    Layer *layer = layers[index];
    auto *newPixelData = new uint32_t[layer->width * layer->height];
    for (int i = 0; i < layer->width * layer->height; i++)newPixelData[i] = 0x00000000;

    // apply kernel to each pixel
    int kernelRadius = 3 / 2;
    for (int y = 0; y < layer->height; y++) {
        for (int x = 0; x < layer->width; x++) {
            int r = 0, g = 0, b = 0;
            for (int ky = 0; ky < 3; ky++) {
                for (int kx = 0; kx < 3; kx++) {
                    int px = x + kx - kernelRadius;
                    int py = y + ky - kernelRadius;
                    if (px >= 0 && px < layer->width && py >= 0 && py < layer->height) {
                        uint32_t pixel = layer->getPixel(px, py);
                        r += int(((pixel >> 16) & 0xFF) * kernel[ky * 3 + kx]);
                        g += int(((pixel >> 8) & 0xFF) * kernel[ky * 3 + kx]);
                        b += int((pixel & 0xFF) * kernel[ky * 3 + kx]);
                    }
                }
            }
            r = min(0xFF, max(0, r / divisor + offset));
            g = min(0xFF, max(0, g / divisor + offset));
            b = min(0xFF, max(0, b / divisor + offset));
            newPixelData[y * layer->width + x] = (0xFF000000 | (r << 16) | (g << 8) | b);
        }
    }

    // set new buffer and update position and dimensions
    layer->setNewBuffer(newPixelData, layer->posX, layer->posY, layer->width, layer->height);

    if (writeHistory) {
        history->addCommand(
                Util::String::format("filterKernel %d %d %d %d %d %d %d %d %d %d %d %d", index, kernel[0], kernel[1], kernel[2], kernel[3],
                                     kernel[4], kernel[5], kernel[6], kernel[7], kernel[8], divisor, offset), &layers, &layerCount);
    }
}

/**
 * \brief Applies a convolution filter to the current layer using the given 3x3 kernel.
 *
 * This function calls filterKernel on the current layer with the specified parameters.
 *
 * \param kernel The 3x3 kernel matrix to use for the convolution filter.
 * \param divisor The divisor to normalize the kernel values.
 * \param offset The offset to add to the resulting pixel values.
 */
void Layers::filterKernelCurrent(int kernel[9], int divisor, int offset) {
    filterKernel(currentLayer, kernel, divisor, offset);
}
