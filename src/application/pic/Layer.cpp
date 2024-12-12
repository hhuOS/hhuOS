#include "Layer.h"

/**
 * \brief Constructor for Layer class.
 *
 * Initializes a Layer with given dimensions, position, and visibility.
 * Allocates memory for pixel data and initializes it to zero.
 *
 * @param width Width of the layer.
 * @param height Height of the layer.
 * @param posX X position of the layer.
 * @param posY Y position of the layer.
 * @param visible Visibility of the layer.
 */
Layer::Layer(int width, int height, int posX, int posY, int visible) : width(width), height(height), posX(posX),
                                                                       posY(posY), isVisible(visible), tempPixelData(new uint32_t[0]) {
    int size = width * height;
    pixelData = new uint32_t[size];
    for (int i = 0; i < size; ++i) {
        pixelData[i] = 0;
    }
    prepareNextDrawing();
}

/**
 * \brief Constructor for Layer class.
 *
 * Initializes a Layer with given dimensions, position, visibility, and pixel data.
 * Allocates memory for pixel data and copies the provided pixel data.
 *
 * @param width Width of the layer.
 * @param height Height of the layer.
 * @param posX X position of the layer.
 * @param posY Y position of the layer.
 * @param visible Visibility of the layer.
 * @param pixelData Pointer to the pixel data to initialize the layer with.
 */
Layer::Layer(int width, int height, int posX, int posY, int visible, const uint32_t *pixelData)
        : width(width), height(height), posX(posX), posY(posY), isVisible(visible), tempPixelData(new uint32_t[0]) {
    int size = width * height;
    this->pixelData = new uint32_t[size];
    for (int i = 0; i < size; ++i) {
        this->pixelData[i] = pixelData[i];
    }
    prepareNextDrawing();
}

/**
 * \brief Copy constructor for Layer class.
 *
 * Initializes a Layer by copying another Layer.
 *
 * @param other The Layer object to copy from.
 */
Layer::Layer(const Layer &other)
        : width(other.width), height(other.height), posX(other.posX), posY(other.posY), isVisible(other.isVisible), tempPixelData(new uint32_t[0]) {
    int size = width * height;
    pixelData = new uint32_t[size];
    for (int i = 0; i < size; ++i) {
        pixelData[i] = other.pixelData[i];
    }
}

/**
 * \brief Copy assignment operator for Layer class.
 *
 * Assigns the values from another Layer to this Layer.
 *
 * @param other The Layer object to assign from.
 * @return Reference to this Layer.
 */
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

/**
 * \brief Destructor for Layer class.
 *
 * Frees the allocated memory for pixel data.
 */
Layer::~Layer() {
    delete[] pixelData;
    delete[] tempPixelData;
}

/**
 * Gets the pixel data of the layer.
 *
 * @return Pointer to the pixel data.
 */
uint32_t *Layer::getPixelData() const {
    return pixelData;
}

/**
 * Gets the temporary pixel data (for blending while drawing) of the layer.
 *
 * @return Pointer to the temporary pixel data.
 */
uint32_t *Layer::getTempPixelData() const {
    return tempPixelData;
}

/**
 * Gets the pixel value at the specified coordinates.
 *
 * @param x X coordinate.
 * @param y Y coordinate.
 * @return Pixel value at the specified coordinates.
 */
uint32_t Layer::getPixel(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return 0;
    }
    return pixelData[y * width + x];
}

/**
 * Gets the temporary pixel value (for blending while drawing) at the specified coordinates.
 *
 * @param x X coordinate.
 * @param y Y coordinate.
 * @return Temporary pixel value at the specified coordinates.
 */
uint32_t Layer::getTempPixel(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return 0;
    }
    return tempPixelData[y * width + x];
}

/**
 * Sets the pixel value at the specified coordinates.
 *
 * @param x X coordinate.
 * @param y Y coordinate.
 * @param color Color value to set.
 */
void Layer::setPixel(int x, int y, unsigned int color) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return; // Silent fail if out of bounds
    }
    int index = y * width + x;
    pixelData[index] = color;
}

/**
 * Sets a new buffer for the layer and updates its position and dimensions.
 *
 * @param newBuffer Pointer to the new pixel data buffer.
 * @param x New X position.
 * @param y New Y position.
 * @param w New width.
 * @param h New height.
 */
void Layer::setNewBuffer(uint32_t *newBuffer, int x, int y, int w, int h) {
    delete[] pixelData;
    pixelData = newBuffer;
    posX = x;
    posY = y;
    width = w;
    height = h;
}

/**
 * Prepares the temporary pixel data for the next drawing.
 * Copies the current pixel data to the temporary pixel data.
 */
void Layer::prepareNextDrawing() {
    delete[] tempPixelData;
    tempPixelData = new uint32_t[width * height];
    for (int i = 0; i < width * height; ++i) {
        tempPixelData[i] = pixelData[i];
    }
}