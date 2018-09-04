extern "C" {
#include <lib/libc/math.h>
}
#include "Bmp.h"

Bmp::Bmp(File *file) {
    *file >> rawData;

    processData();

    delete rawData;
}

Bmp::Bmp(char *data) : rawData(data) {
    processData();
}

Bmp::~Bmp() {
    delete processedPixels;
}

void Bmp::processData() {
    fileHeader = *((BitmapFileHeader*) &rawData[0]);

    BitmapHeaderVersion headerVersion = *((BitmapHeaderVersion*) &rawData[14]);

    if(headerVersion == BITMAPCOREHEADER) {
        BitmapCoreHeaderV1 legacyHeader = *((BitmapCoreHeaderV1*) &rawData[14]);
        infoHeader = convertLegacyHeader(legacyHeader);
    } else if(headerVersion == BITMAPCOREHEADER2_LONG || headerVersion == BITMAPCOREHEADER2_SHORT){
        BitmapCoreHeaderV2 legacyHeader = *((BitmapCoreHeaderV2*) &rawData[14]);
        infoHeader = convertLegacyHeader(legacyHeader);
    } else {
        infoHeader = *((BitmapInformationHeader *) &rawData[14]);
    }

    if(infoHeader.compression == BI_BITFIELDS) {
        bitMask = *((BitMask*) &rawData[sizeof(BitmapFileHeader) + sizeof(BitmapInformationHeader)]);
        colorPalette = ColorPalette(headerVersion, &rawData[sizeof(BitmapFileHeader) + infoHeader.headerSize + sizeof(BitMask)]);
    } else {
        if(infoHeader.bitmapDepth > 16) {
            bitMask = {0x00ff0000, 0x0000ff00, 0x000000ff};
        } else {
            bitMask = {0x00007C00, 0x000003E0, 0x0000001F};
        }

        colorPalette = ColorPalette(headerVersion, &rawData[sizeof(BitmapFileHeader) + infoHeader.headerSize]);
    }

    auto *pixelData = reinterpret_cast<uint8_t *>(&rawData[fileHeader.dataOffset]);

    if(infoHeader.compression == BI_RLE8) {
        pixelData = decodeRLE8(pixelData);
    } else if(infoHeader.compression == BI_RLE4) {
        pixelData = decodeRLE4(pixelData);
    }

    // If height > 0 -> bottom-up, else -> top-down
    if(infoHeader.bitmapHeight < 0) {
        bottomUpImage = false;
    }
    
    processedPixels = new Color[getWidth() * getHeight()];

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

    if(infoHeader.compression == BI_RLE8 || infoHeader.compression == BI_RLE4) {
        delete pixelData;
    }
}

Bmp::BitmapInformationHeader Bmp::convertLegacyHeader(const Bmp::BitmapCoreHeaderV1 &legacyHeader) {
    BitmapInformationHeader ret{0};

    ret.headerSize = legacyHeader.headerSize;
    ret.bitmapWidth = legacyHeader.bitmapWidth;
    ret.bitmapHeight = legacyHeader.bitmapHeight;
    ret.numColorPlanes = legacyHeader.numColorPlanes;
    ret.bitmapDepth = legacyHeader.bitmapDepth;
    ret.compression = CompressionMethod::BI_RGB;

    return ret;
}

Bmp::BitmapInformationHeader Bmp::convertLegacyHeader(const Bmp::BitmapCoreHeaderV2 &legacyHeader) {
    BitmapInformationHeader ret{0};

    ret.headerSize = legacyHeader.headerSize;
    ret.bitmapWidth = legacyHeader.bitmapWidth;
    ret.bitmapHeight = legacyHeader.bitmapHeight;
    ret.numColorPlanes = legacyHeader.numColorPlanes;
    ret.bitmapDepth = legacyHeader.bitmapDepth;
    ret.compression = CompressionMethod::BI_RGB;

    return ret;
}

uint8_t *Bmp::decodeRLE4(uint8_t *encodedData) {
    // TODO: Implement decoding of 4-Bit RLE
    auto *decodedData = new uint8_t[getHeight() * getWidth() / 2 * (8 / infoHeader.bitmapDepth)];

    memset(decodedData, 0, getHeight() * getWidth() / 2 * (8 / infoHeader.bitmapDepth));

    return decodedData;
}

uint8_t* Bmp::decodeRLE8(uint8_t *encodedData) {
    auto *decodedData = new uint8_t[getHeight() * getWidth() * (8 / infoHeader.bitmapDepth)];

    uint32_t encodedIndex = 0;
    uint32_t decodedIndex = 0;
    bool endOfBitmap = false;

    while(!endOfBitmap) {
        uint8_t firstByte = encodedData[encodedIndex];
        uint8_t secondByte = encodedData[encodedIndex + 1];

        if(firstByte == 0) {
            switch(secondByte) {
                case 0:
                    // End of line reached (can be ignored)
                    encodedIndex += 2;
                    break;
                case 1:
                    // End of bitmap reached
                    endOfBitmap = true;
                    break;
                case 2:
                    // Index is changed -> new index = (oldX + secondByte, oldY + thirdByte)
                    encodedIndex += encodedData[encodedIndex + 2] + getWidth() * encodedData[encodedIndex + 3];
                    break;
                default:
                    // Transfer secondByte pixels from encodedData to decodedData
                    memcpy(&decodedData[decodedIndex], &encodedData[encodedIndex + 2], secondByte);

                    encodedIndex += 2 + secondByte;
                    decodedIndex += secondByte;

                    if(encodedIndex % 2 != 0) {
                        encodedIndex++;
                    }

                    break;
            }
        } else {
            // secondByte is repeated firstByte times.
            memset(&decodedData[decodedIndex], secondByte, firstByte);
            decodedIndex += firstByte;

            encodedIndex += 2;
        }
    }

    return decodedData;
}

void Bmp::read32BitImage(uint8_t *rawPixelData) {
    uint8_t redShift = countTrailingZeros(bitMask.red);
    uint8_t greenShift = countTrailingZeros(bitMask.green);
    uint8_t blueShift = countTrailingZeros(bitMask.blue);

    uint32_t alphaMask = ~(bitMask.red | bitMask.green  | bitMask.blue);
    uint8_t alphaShift = countTrailingZeros(alphaMask);

    uint32_t currentRow = bottomUpImage ? getHeight() - 1 : 0;

    for(uint32_t i = 0; i < getHeight(); i++) {
        for(uint32_t j = 0; j < getWidth(); j++) {
            uint32_t currentPixel = ((uint32_t *) rawPixelData)[i * getWidth() + j];

            auto red = static_cast<uint32_t>((currentPixel & bitMask.red) >> redShift);
            auto green = static_cast<uint32_t>((currentPixel & bitMask.green) >> greenShift);
            auto blue = static_cast<uint32_t>((currentPixel & bitMask.blue) >> blueShift);
            auto alpha = static_cast<uint32_t>((currentPixel & alphaMask) >> alphaShift);

            double redPerc = red / pow(2, countOnes(bitMask.red));
            double greenPerc = green / pow(2, countOnes(bitMask.green));
            double bluePerc = blue / pow(2, countOnes(bitMask.blue));
            double alphaPerc = alpha / pow(2, countOnes(alphaMask));

            if (ignoreAlpha && alpha != 0) {
                ignoreAlpha = false;
            }

            processedPixels[currentRow * getWidth() + j] = Color(static_cast<uint8_t>(redPerc * 255),
                                                                 static_cast<uint8_t>(greenPerc * 255),
                                                                 static_cast<uint8_t>(bluePerc * 255),
                                                                 static_cast<uint8_t>(alphaPerc * 255));
        }

        currentRow += bottomUpImage ? -1 : 1;
    }

    if(ignoreAlpha) {
        for(uint32_t i = 0; i < getWidth() * getHeight(); i++) {
            processedPixels[i].setAlpha(255);
        }
    }
}

void Bmp::read24BitImage(uint8_t *rawPixelData) {
    uint8_t redShift = countTrailingZeros(bitMask.red);
    uint8_t greenShift = countTrailingZeros(bitMask.green);
    uint8_t blueShift = countTrailingZeros(bitMask.blue);

    uint32_t currentRow = bottomUpImage ? getHeight() - 1 : 0;

    for(uint32_t i = 0; i < getHeight(); i++) {
        for(uint32_t j = 0; j < getWidth(); j++) {
            uint32_t index = 3 * (i * getPaddedWidth() + j);
            uint32_t currentPixel = rawPixelData[index] | rawPixelData[index + 1] << 8 | rawPixelData[index + 2] << 16;

            auto red = static_cast<uint32_t>((currentPixel & bitMask.red) >> redShift);
            auto green = static_cast<uint32_t>((currentPixel & bitMask.green) >> greenShift);
            auto blue = static_cast<uint32_t>((currentPixel & bitMask.blue) >> blueShift);

            double redPerc = red / pow(2, countOnes(bitMask.red));
            double greenPerc = green / pow(2, countOnes(bitMask.green));
            double bluePerc = blue / pow(2, countOnes(bitMask.blue));

            processedPixels[currentRow * getWidth() + j] = Color(static_cast<uint8_t>(redPerc * 255),
                                                                 static_cast<uint8_t>(greenPerc * 255),
                                                                 static_cast<uint8_t>(bluePerc * 255));
        }

        currentRow += bottomUpImage ? -1 : 1;
    }
}

void Bmp::read16BitImage(uint8_t *rawPixelData) {
    uint8_t redShift = countTrailingZeros(bitMask.red);
    uint8_t greenShift = countTrailingZeros(bitMask.green);
    uint8_t blueShift = countTrailingZeros(bitMask.blue);

    uint32_t currentRow = bottomUpImage ? getHeight() - 1 : 0;

    for(uint32_t i = 0; i < getHeight(); i++) {
        for(uint32_t j = 0; j < getWidth(); j++) {
            uint16_t currentPixel = ((uint16_t *) rawPixelData)[i * getPaddedWidth() + j];

            auto red = static_cast<uint16_t>((currentPixel & bitMask.red) >> redShift);
            auto green = static_cast<uint16_t>((currentPixel & bitMask.green) >> greenShift);
            auto blue = static_cast<uint16_t>((currentPixel & bitMask.blue) >> blueShift);

            double redPerc = red / pow(2, countOnes(bitMask.red));
            double greenPerc = green / pow(2, countOnes(bitMask.green));
            double bluePerc = blue / pow(2, countOnes(bitMask.blue));

            processedPixels[currentRow * getWidth() + j] = Color(static_cast<uint8_t>(redPerc * 255),
                                                                 static_cast<uint8_t>(greenPerc * 255),
                                                                 static_cast<uint8_t>(bluePerc * 255));
        }

        currentRow += bottomUpImage ? -1 : 1;
    }
}

void Bmp::read8BitImage(uint8_t *rawPixelData) {
    uint32_t currentRow = bottomUpImage ? getHeight() - 1 : 0;

    for(uint32_t i = 0; i < getHeight(); i++) {
        for(uint32_t j = 0; j < getWidth(); j++) {
            uint8_t currentPixel = rawPixelData[(i * getPaddedWidth() + j)];

            auto red = colorPalette.getColor(currentPixel).red;
            auto green = colorPalette.getColor(currentPixel).green;
            auto blue = colorPalette.getColor(currentPixel).blue;

            processedPixels[currentRow * getWidth() + j] = Color(red, green, blue);
        }

        currentRow += bottomUpImage ? -1 : 1;
    }
}

void Bmp::read4BitImage(uint8_t *rawPixelData) {
    // TODO: Implement processing of 4-Bit images

    for(uint32_t i = 0; i < getWidth() * getHeight(); i++) {
        processedPixels[i] = Colors::BLACK;
    }
}

void Bmp::read2BitImage(uint8_t *rawPixelData) {
    // TODO: Implement processing of 2-Bit images

    for(uint32_t i = 0; i < getWidth() * getHeight(); i++) {
        processedPixels[i] = Colors::BLACK;
    }
}

void Bmp::read1BitImage(uint8_t *rawPixelData) {
    // TODO: Implement processing of 1-Bit images

    for(uint32_t i = 0; i < getWidth() * getHeight(); i++) {
        processedPixels[i] = Colors::BLACK;
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
    if(infoHeader.bitmapWidth >= 0) {
        return static_cast<uint32_t>(infoHeader.bitmapWidth);
    }

    return static_cast<uint32_t>(infoHeader.bitmapWidth * -1);
}

uint8_t Bmp::getColorDepth() {
    return static_cast<uint8_t>(infoHeader.bitmapDepth);
}

uint32_t Bmp::getPaddedWidth() {
    uint32_t width = getWidth();

    if(width % 4 != 0) {
        width += 4 - width % 4;
    }

    return width;
}

void Bmp::draw(uint16_t x, uint16_t y) {
    auto *lfb = Kernel::getService<GraphicsService>()->getLinearFrameBuffer();

    for(uint16_t i = 0; i < getHeight(); i++) {
        for(uint16_t j = 0; j < getWidth(); j++) {
            lfb->drawPixel(x + j, y + i, processedPixels[i * getWidth() + j]);
        }
    }
}
