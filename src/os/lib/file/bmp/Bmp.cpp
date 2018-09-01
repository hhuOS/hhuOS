#include "Bmp.h"

Bmp::Bmp(File *file) : deleteRawData(true) {
    *file >> rawData;

    processData();
}

Bmp::Bmp(char *data) : rawData(data), deleteRawData(false) {
    processData();
}

Bmp::~Bmp() {
    if(deleteRawData) {
        delete rawData;
    }

    if(processedPixels != nullptr) {
        delete[] processedPixels;
    }
}

void Bmp::processData() {
    fileHeader = *((BitmapFileHeader*) &rawData[0]);
    infoHeader = *((BitmapInfoHeader*) &rawData[14]);

    if(infoHeader.compression == BI_BITFIELDS) {
        bitMask = *((BitMask*) &rawData[sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader)]);
    } else {
        if(infoHeader.bitmapDepth > 16) {
            bitMask = {0x00ff0000, 0x0000ff00, 0x000000ff};
        } else {
            bitMask = {0x00007C00, 0x000003E0, 0x0000001F};
        }
    }

    auto *pixelData = reinterpret_cast<uint8_t *>(&rawData[fileHeader.dataOffset]);

    colorPalette = (ColorPaletteEntry*) &rawData[sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader)];

    // Search for color palette identifier
    while(colorPalette[0].blue != 'B' && colorPalette[0].green != 'G' &&
            colorPalette[0].red != 'R' && colorPalette[0].reserved != 's' &&
            (uint32_t) colorPalette < (uint32_t) pixelData) {
        colorPalette = reinterpret_cast<ColorPaletteEntry *>(((char*) colorPalette) + 1);
    }

    colorPalette++;
    
    processedPixels = new Color[infoHeader.bitmapWidth * infoHeader.bitmapHeight];

    switch(infoHeader.bitmapDepth) {
        case 32:
            read32BitImage(pixelData);
            break;
        case 24:
            read24BitImage(pixelData);
            break;
        case 16:
            read16BitImage(pixelData);
            break;
        case 8:
            read8BitImage(pixelData);
            break;
        case 4:
            read4BitImage(pixelData);
            break;
        case 2:
            read2BitImage(pixelData);
            break;
        case 1:
            read1BitImage(pixelData);
            break;
        default:
            read24BitImage(pixelData);
            break;
    }
}

void Bmp::read32BitImage(uint8_t *rawPixelData) {
    uint8_t redShift = countTrailingZeros(bitMask.red);
    uint8_t greenShift = countTrailingZeros(bitMask.green);
    uint8_t blueShift = countTrailingZeros(bitMask.blue);
    uint8_t alphaShift = 24;

    // If height > 0 -> bottom-up, else -> top-down
    auto currentRow = static_cast<uint32_t>(infoHeader.bitmapHeight >= 0 ? infoHeader.bitmapHeight - 1 : 0);

    for(uint32_t i = 0; i < getHeight(); i++) {
        for(uint32_t j = 0; j < getWidth(); j++) {
            uint32_t currentPixel = ((uint32_t *) rawPixelData)[i * infoHeader.bitmapWidth + j];

            auto red = static_cast<uint8_t>((currentPixel & bitMask.red) >> redShift);
            auto green = static_cast<uint8_t>((currentPixel & bitMask.green) >> greenShift);
            auto blue = static_cast<uint8_t>((currentPixel & bitMask.blue) >> blueShift);
            auto alpha = static_cast<uint8_t>((currentPixel & 0xff000000) >> alphaShift);

            if (ignoreAlpha && alpha != 0) {
                ignoreAlpha = false;
            }

            processedPixels[currentRow * infoHeader.bitmapWidth + j] = Color(red, green, blue, alpha);
        }

        if(infoHeader.bitmapHeight >= 0) {
            // bottom-up
            currentRow--;
        } else {
            // top-down
            currentRow++;
        }
    }

    if(ignoreAlpha) {
        for(int32_t i = 0; i < infoHeader.bitmapHeight * infoHeader.bitmapWidth; i++) {
            processedPixels[i].setAlpha(255);
        }
    }
}

void Bmp::read24BitImage(uint8_t *rawPixelData) {
    uint8_t redShift = countTrailingZeros(bitMask.red);
    uint8_t greenShift = countTrailingZeros(bitMask.green);
    uint8_t blueShift = countTrailingZeros(bitMask.blue);

    // If height > 0 -> bottom-up, else -> top-down
    auto currentRow = static_cast<uint32_t>(infoHeader.bitmapHeight >= 0 ? infoHeader.bitmapHeight - 1 : 0);

    for(uint32_t i = 0; i < getHeight(); i++) {
        for(uint32_t j = 0; j < getWidth(); j++) {
            uint32_t index = 3 * (i * infoHeader.bitmapWidth + j);
            uint32_t currentPixel = rawPixelData[index] | rawPixelData[index + 1] << 8 | rawPixelData[index + 2] << 16;

            auto red = static_cast<uint8_t>((currentPixel & bitMask.red) >> redShift);
            auto green = static_cast<uint8_t>((currentPixel & bitMask.green) >> greenShift);
            auto blue = static_cast<uint8_t>((currentPixel & bitMask.blue) >> blueShift);

            processedPixels[currentRow * infoHeader.bitmapWidth + j] = Color(red, green, blue);
        }

        if(infoHeader.bitmapHeight >= 0) {
            // bottom-up
            currentRow--;
        } else {
            // top-down
            currentRow++;
        }
    }
}

void Bmp::read16BitImage(uint8_t *rawPixelData) {
    uint8_t redShift = countTrailingZeros(bitMask.red);
    uint8_t greenShift = countTrailingZeros(bitMask.green);
    uint8_t blueShift = countTrailingZeros(bitMask.blue);

    // If height > 0 -> bottom-up, else -> top-down
    auto currentRow = static_cast<uint32_t>(infoHeader.bitmapHeight >= 0 ? infoHeader.bitmapHeight - 1 : 0);

    for(uint32_t i = 0; i < getHeight(); i++) {
        for(uint32_t j = 0; j < getWidth(); j++) {
            uint16_t currentPixel = ((uint16_t *) rawPixelData)[i * infoHeader.bitmapWidth + j];

            auto red = static_cast<uint8_t>((currentPixel & bitMask.red) >> redShift);
            auto green = static_cast<uint8_t>((currentPixel & bitMask.green) >> greenShift);
            auto blue = static_cast<uint8_t>((currentPixel & bitMask.blue) >> blueShift);

            // Normalize to 8-bit colors
            red = static_cast<uint8_t>(red * (256 / 32));
            green = static_cast<uint8_t>(green * (256 / ((countOnes(bitMask.green) == 6) ? 64 : 32)));
            blue = static_cast<uint8_t>(blue * (256 / 32));

            processedPixels[currentRow * infoHeader.bitmapWidth + j] = Color(red, green, blue);
        }

        if(infoHeader.bitmapHeight >= 0) {
            // bottom-up
            currentRow--;
        } else {
            // top-down
            currentRow++;
        }
    }
}

void Bmp::read8BitImage(uint8_t *rawPixelData) {
    // If height > 0 -> bottom-up, else -> top-down
    auto currentRow = static_cast<uint32_t>(infoHeader.bitmapHeight >= 0 ? infoHeader.bitmapHeight - 1 : 0);

    for(uint32_t i = 0; i < getHeight(); i++) {
        for(uint32_t j = 0; j < getWidth(); j++) {
            uint8_t currentPixel = rawPixelData[i * infoHeader.bitmapWidth + j];

            auto red = colorPalette[currentPixel].red;
            auto green = colorPalette[currentPixel].green;
            auto blue = colorPalette[currentPixel].blue;

            processedPixels[currentRow * infoHeader.bitmapWidth + j] = Color(red, green, blue);
        }

        if(infoHeader.bitmapHeight >= 0) {
            // bottom-up
            currentRow--;
        } else {
            // top-down
            currentRow++;
        }
    }
}

void Bmp::read4BitImage(uint8_t *rawPixelData) {
    // If height > 0 -> bottom-up, else -> top-down
    auto currentRow = static_cast<uint32_t>(infoHeader.bitmapHeight >= 0 ? infoHeader.bitmapHeight - 1 : 0);

    for(uint32_t i = 0; i < getHeight(); i++) {
        for(uint32_t j = 0; j < getWidth() / 2; j++) {
            uint8_t pixels[2];
            pixels[0] = static_cast<uint8_t>(rawPixelData[i * infoHeader.bitmapWidth + j] & 0x0fu);
            pixels[1] = static_cast<uint8_t>((rawPixelData[i * infoHeader.bitmapWidth + j] & 0xf0u) >> 4);

            for(uint8_t k = 0; k < 2; k++) {
                auto red = colorPalette[pixels[k]].red;
                auto green = colorPalette[pixels[k]].green;
                auto blue = colorPalette[pixels[k]].blue;

                processedPixels[currentRow * infoHeader.bitmapWidth + j * (k + 1)] = Color(red, green, blue);
            }
        }

        if(infoHeader.bitmapHeight >= 0) {
            // bottom-up
            currentRow--;
        } else {
            // top-down
            currentRow++;
        }
    }
}

void Bmp::read2BitImage(uint8_t *rawPixelData) {
    // If height > 0 -> bottom-up, else -> top-down
    auto currentRow = static_cast<uint32_t>(infoHeader.bitmapHeight >= 0 ? infoHeader.bitmapHeight - 1 : 0);

    for(uint32_t i = 0; i < getHeight(); i++) {
        for(uint32_t j = 0; j < getWidth() / 8; j++) {
            uint8_t pixels[4];
            pixels[0] = static_cast<uint8_t>(rawPixelData[i * infoHeader.bitmapWidth + j] & 0x03u);
            pixels[1] = static_cast<uint8_t>((rawPixelData[i * infoHeader.bitmapWidth + j] & 0x12u) >> 2);
            pixels[2] = static_cast<uint8_t>((rawPixelData[i * infoHeader.bitmapWidth + j] & 0x48u) >> 4);
            pixels[3] = static_cast<uint8_t>((rawPixelData[i * infoHeader.bitmapWidth + j] & 0x192u) >> 6);

            for(uint8_t k = 0; k < 4; k++) {
                auto red = colorPalette[pixels[k]].red;
                auto green = colorPalette[pixels[k]].green;
                auto blue = colorPalette[pixels[k]].blue;

                processedPixels[currentRow * infoHeader.bitmapWidth + j * (k + 1)] = Color(red, green, blue);
            }
        }

        if(infoHeader.bitmapHeight >= 0) {
            // bottom-up
            currentRow--;
        } else {
            // top-down
            currentRow++;
        }
    }
}

void Bmp::read1BitImage(uint8_t *rawPixelData) {
    // If height > 0 -> bottom-up, else -> top-down
    auto currentRow = static_cast<uint32_t>(infoHeader.bitmapHeight >= 0 ? infoHeader.bitmapHeight - 1 : 0);

    for(uint32_t i = 0; i < getHeight(); i++) {
        for(uint32_t j = 0; j < getWidth() / 8; j++) {
            uint8_t pixels[8];
            pixels[0] = static_cast<uint8_t>(rawPixelData[i * infoHeader.bitmapWidth + j] & 0x01u);
            pixels[1] = static_cast<uint8_t>((rawPixelData[i * infoHeader.bitmapWidth + j] & 0x02u) >> 1);
            pixels[2] = static_cast<uint8_t>((rawPixelData[i * infoHeader.bitmapWidth + j] & 0x04u) >> 2);
            pixels[3] = static_cast<uint8_t>((rawPixelData[i * infoHeader.bitmapWidth + j] & 0x08u) >> 3);
            pixels[4] = static_cast<uint8_t>((rawPixelData[i * infoHeader.bitmapWidth + j] & 0x16u) >> 4);
            pixels[5] = static_cast<uint8_t>((rawPixelData[i * infoHeader.bitmapWidth + j] & 0x32u) >> 5);
            pixels[6] = static_cast<uint8_t>((rawPixelData[i * infoHeader.bitmapWidth + j] & 0x64u) >> 6);
            pixels[7] = static_cast<uint8_t>((rawPixelData[i * infoHeader.bitmapWidth + j] & 0x128u) >> 7);

            for(uint8_t k = 0; k < 8; k++) {
                auto red = colorPalette[pixels[k]].red;
                auto green = colorPalette[pixels[k]].green;
                auto blue = colorPalette[pixels[k]].blue;

                processedPixels[currentRow * infoHeader.bitmapWidth + j * (k + 1)] = Color(red, green, blue);
            }
        }

        if(infoHeader.bitmapHeight >= 0) {
            // bottom-up
            currentRow--;
        } else {
            // top-down
            currentRow++;
        }
    }
}

uint8_t Bmp::countTrailingZeros(uint32_t number) {
    if(number == 0) {
        return 0;
    }

    uint8_t ret = 0;

    while((number & 0x1) == 0) {
        number >>= 1;
        ret++;
    }

    return ret;
}

uint8_t Bmp::countOnes(uint32_t number) {
    uint8_t ret = 0;

    for(uint8_t i = 0; i < 32; i++) {
        if((number & 0x1) == 1) {
            ret++;
        }

        number >>= 1;
    }

    return ret;
}

uint32_t Bmp::getHeight() {
    if(infoHeader.bitmapHeight >= 0) {
        return static_cast<uint32_t>(infoHeader.bitmapHeight);
    }

    return static_cast<uint32_t>(infoHeader.bitmapHeight * -1);
}

uint32_t Bmp::getWidth() {
    if(infoHeader.bitmapHeight >= 0) {
        return static_cast<uint32_t>(infoHeader.bitmapWidth);
    }

    return static_cast<uint32_t>(infoHeader.bitmapWidth * -1);
}

void Bmp::draw(uint16_t x, uint16_t y) {
    auto *lfb = Kernel::getService<GraphicsService>()->getLinearFrameBuffer();

    for(uint16_t i = 0; i < infoHeader.bitmapHeight; i++) {
        for(uint16_t j = 0; j < infoHeader.bitmapWidth; j++) {
            lfb->drawPixel(x + j, y + i, processedPixels[i * infoHeader.bitmapWidth + j]);
        }
    }
}
