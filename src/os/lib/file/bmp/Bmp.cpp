/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include "Bmp.h"

extern "C" {
#include <lib/libc/math.h>
}

Bmp::Bmp(File *file) {
    *file >> rawData;

    processData();

    delete rawData;
}

Bmp::Bmp(const Bmp &other) {
    this->width = other.width;
    this->height = other.height;
    this->depth = 32;

    this->pixelBuf = new Color[height * width];
    memcpy(this->pixelBuf, other.pixelBuf, height * width * sizeof(Color));
}

Bmp::Bmp(Color *pixelBuf, uint32_t width, uint32_t height) {
    this->width = width;
    this->height = height;
    this->depth = 32;

    this->pixelBuf = new Color[height * width];
    memcpy(this->pixelBuf, pixelBuf, height * width * sizeof(Color));
}

Bmp::~Bmp() {
    delete[] pixelBuf;
}

Bmp *Bmp::load(const String &path) {
    File *file = File::open(path, "r");

    if(file == nullptr) {
        return nullptr;
    }

    auto ret = new Bmp(file);

    delete file;

    return ret;
}

void Bmp::processData() {
    fileHeader = *((BitmapFileHeader*) &rawData[0]);
    infoHeader = BitmapHeader(&rawData[14]);
    
    if(infoHeader.bitmapHeight < 0) {
        height = static_cast<uint32_t>(infoHeader.bitmapHeight * -1);
        bottomUpImage = false;
    } else {
        height = static_cast<uint32_t>(infoHeader.bitmapHeight);
    }

    if(infoHeader.bitmapWidth < 0) {
        width = static_cast<uint32_t>(infoHeader.bitmapWidth * -1);
    } else {
        width = static_cast<uint32_t>(infoHeader.bitmapWidth);
    }
    
    depth = static_cast<uint8_t>(infoHeader.bitmapDepth);
    
    if(infoHeader.compression == BI_BITFIELDS) {
        bitMask = *((BitMask*) &rawData[sizeof(BitmapFileHeader) + sizeof(BitmapInformationHeader)]);
        colorPalette = ColorPalette(infoHeader.version, &rawData[sizeof(BitmapFileHeader) + infoHeader.headerSize + sizeof(BitMask)]);
    } else {
        if(infoHeader.bitmapDepth > 16) {
            bitMask = {0x00ff0000, 0x0000ff00, 0x000000ff};
        } else {
            bitMask = {0x00007C00, 0x000003E0, 0x0000001F};
        }

        colorPalette = ColorPalette(infoHeader.version, &rawData[sizeof(BitmapFileHeader) + infoHeader.headerSize]);
    }

    auto *pixelData = reinterpret_cast<uint8_t *>(&rawData[fileHeader.dataOffset]);

    if(infoHeader.compression == BI_RLE8) {
        pixelData = decodeRLE8(pixelData);
    } else if(infoHeader.compression == BI_RLE4) {
        pixelData = decodeRLE4(pixelData);
    }
    
    pixelBuf = new Color[width * height];

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

uint8_t *Bmp::decodeRLE4(uint8_t *encodedData) {
    auto *decodedData = new uint8_t[height * width * (8 / infoHeader.bitmapDepth)];
    memset(decodedData, 0, height * width * (8 / infoHeader.bitmapDepth));

    uint32_t encodedIndex = 0;
    uint32_t decodedIndex = 0;
    uint8_t currentNibble = 0;
    bool endOfBitmap = false;

    while(!endOfBitmap) {
        uint8_t firstByte = encodedData[encodedIndex];
        uint8_t secondByte = encodedData[encodedIndex + 1];

        if(firstByte == 0) {
            switch(secondByte) {
                case 0:
                    // End of line reached
                    encodedIndex += 2;

                    if(decodedIndex % 4 != 0) {
                        decodedIndex += 4 - decodedIndex % 4;
                        currentNibble = 0;
                    }

                    break;
                case 1:
                    // End of bitmap reached
                    endOfBitmap = true;
                    break;
                case 2: {
                    // Index is changed -> new index = (oldX + thirdByte, oldY + fourthByte)
                    uint32_t offset = encodedData[encodedIndex + 2] + width * encodedData[encodedIndex + 3];

                    encodedIndex += offset / 2;
                    decodedIndex += offset / 2;

                    break;
                    }
                default:
                    // Transfer secondByte pixels from encodedData to decodedData
                    encodedIndex += 2;

                    for(uint32_t i = 0; i < secondByte; i++) {
                        if(currentNibble == 0) {
                            decodedData[decodedIndex] |= (encodedData[encodedIndex] & 0x0fu);

                            currentNibble = 1;
                        } else {
                            decodedData[decodedIndex] |= (encodedData[encodedIndex] & 0xf0u);

                            currentNibble = 0;
                            encodedIndex++;
                            decodedIndex++;
                        }
                    }

                    if(encodedIndex % 2 != 0) {
                        encodedIndex++;
                    }

                    break;
            }
        } else {
            // secondByte is repeated firstByte times.
            for(uint32_t i = 0; i < firstByte; i++) {
                if(currentNibble == 0) {
                    decodedData[decodedIndex] |= (secondByte & 0x0fu);

                    currentNibble = 1;
                } else {
                    decodedData[decodedIndex] |= (secondByte & 0xf0u);

                    currentNibble = 0;
                    decodedIndex++;
                }
            }

            encodedIndex += 2;
        }
    }

    return decodedData;
}

uint8_t* Bmp::decodeRLE8(uint8_t *encodedData) {
    auto *decodedData = new uint8_t[height * getPaddedWidth() * (8 / infoHeader.bitmapDepth)];
    memset(decodedData, 0, height * getPaddedWidth() * (8 / infoHeader.bitmapDepth));

    uint32_t encodedIndex = 0;
    uint32_t decodedIndex = 0;
    bool endOfBitmap = false;

    while(!endOfBitmap) {
        uint8_t firstByte = encodedData[encodedIndex];
        uint8_t secondByte = encodedData[encodedIndex + 1];

        if(firstByte == 0) {
            switch(secondByte) {
                case 0:
                    // End of line reached
                    encodedIndex += 2;

                    if(decodedIndex % 4 != 0) {
                        decodedIndex += 4 - decodedIndex % 4;
                    }

                    break;
                case 1:
                    // End of bitmap reached
                    endOfBitmap = true;
                    break;
                case 2: {
                    // Index is changed -> new index = (oldX + thirdByte, oldY + fourthByte)
                    uint32_t offset = encodedData[encodedIndex + 2] + width * encodedData[encodedIndex + 3];

                    encodedIndex += offset;
                    decodedIndex += offset;

                    break;
                    }
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

    uint32_t currentRow = bottomUpImage ? height - 1 : 0;

    for(uint32_t i = 0; i < height; i++) {
        for(uint32_t j = 0; j < width; j++) {
            uint32_t currentPixel = ((uint32_t *) rawPixelData)[i * width + j];

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

            pixelBuf[currentRow * width + j] = Color(static_cast<uint8_t>(redPerc * 255),
                                                                 static_cast<uint8_t>(greenPerc * 255),
                                                                 static_cast<uint8_t>(bluePerc * 255),
                                                                 static_cast<uint8_t>(alphaPerc * 255));
        }

        currentRow += bottomUpImage ? -1 : 1;
    }

    if(ignoreAlpha) {
        for(uint32_t i = 0; i < width * height; i++) {
            pixelBuf[i].setAlpha(255);
        }
    }
}

void Bmp::read24BitImage(uint8_t *rawPixelData) {
    uint8_t redShift = countTrailingZeros(bitMask.red);
    uint8_t greenShift = countTrailingZeros(bitMask.green);
    uint8_t blueShift = countTrailingZeros(bitMask.blue);

    uint32_t currentRow = bottomUpImage ? height - 1 : 0;

    for(uint32_t i = 0; i < height; i++) {
        for(uint32_t j = 0; j < width; j++) {
            uint32_t index = 3 * (i * getPaddedWidth() + j);
            uint32_t currentPixel = rawPixelData[index] | rawPixelData[index + 1] << 8 | rawPixelData[index + 2] << 16;

            auto red = static_cast<uint32_t>((currentPixel & bitMask.red) >> redShift);
            auto green = static_cast<uint32_t>((currentPixel & bitMask.green) >> greenShift);
            auto blue = static_cast<uint32_t>((currentPixel & bitMask.blue) >> blueShift);

            double redPerc = red / pow(2, countOnes(bitMask.red));
            double greenPerc = green / pow(2, countOnes(bitMask.green));
            double bluePerc = blue / pow(2, countOnes(bitMask.blue));

            pixelBuf[currentRow * width + j] = Color(static_cast<uint8_t>(redPerc * 255),
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

    uint32_t currentRow = bottomUpImage ? height - 1 : 0;

    for(uint32_t i = 0; i < height; i++) {
        for(uint32_t j = 0; j < width; j++) {
            uint16_t currentPixel = ((uint16_t *) rawPixelData)[i * getPaddedWidth() + j];

            auto red = static_cast<uint16_t>((currentPixel & bitMask.red) >> redShift);
            auto green = static_cast<uint16_t>((currentPixel & bitMask.green) >> greenShift);
            auto blue = static_cast<uint16_t>((currentPixel & bitMask.blue) >> blueShift);

            double redPerc = red / pow(2, countOnes(bitMask.red));
            double greenPerc = green / pow(2, countOnes(bitMask.green));
            double bluePerc = blue / pow(2, countOnes(bitMask.blue));

            pixelBuf[currentRow * width + j] = Color(static_cast<uint8_t>(redPerc * 255),
                                                                 static_cast<uint8_t>(greenPerc * 255),
                                                                 static_cast<uint8_t>(bluePerc * 255));
        }

        currentRow += bottomUpImage ? -1 : 1;
    }
}

void Bmp::read8BitImage(uint8_t *rawPixelData) {
    uint32_t currentRow = bottomUpImage ? height - 1 : 0;

    for(uint32_t i = 0; i < height; i++) {
        for(uint32_t j = 0; j < width; j++) {
            uint8_t currentPixel = rawPixelData[(i * getPaddedWidth() + j)];

            auto red = colorPalette.getColor(currentPixel).red;
            auto green = colorPalette.getColor(currentPixel).green;
            auto blue = colorPalette.getColor(currentPixel).blue;

            pixelBuf[currentRow * width + j] = Color(red, green, blue);
        }

        currentRow += bottomUpImage ? -1 : 1;
    }
}

void Bmp::read4BitImage(uint8_t *rawPixelData) {
    uint32_t currentRow = bottomUpImage ? height - 1 : 0;

    for(uint32_t i = 0; i < height; i++) {
        for(uint32_t j = 0; j < width; j++) {
            uint32_t index = (i * getPaddedWidth() + j) / 2 + (i * getPaddedWidth() + j) % 2;
            uint8_t currentPixel = rawPixelData[index];

            if(j % 2 == 0) {
                currentPixel = static_cast<uint8_t>(currentPixel & 0x0fu);
            } else {
                currentPixel = static_cast<uint8_t>((currentPixel & 0xf0u) >> 4);
            }

            auto red = colorPalette.getColor(currentPixel).red;
            auto green = colorPalette.getColor(currentPixel).green;
            auto blue = colorPalette.getColor(currentPixel).blue;

            pixelBuf[currentRow * width + j] = Color(red, green, blue);
        }

        currentRow += bottomUpImage ? -1 : 1;
    }
}

void Bmp::read1BitImage(uint8_t *rawPixelData) {
    uint32_t currentRow = bottomUpImage ? height - 1 : 0;

    for(uint32_t i = 0; i < height; i++) {
        for(uint32_t j = 0; j < width; j++) {
            uint32_t index = (i * getPaddedWidth() + j) / 8;
            uint8_t currentPixel = rawPixelData[index];

            switch(j % 8) {
                case 0:
                    currentPixel = static_cast<uint8_t>(currentPixel & 0x01u);
                    break;
                case 1:
                    currentPixel = static_cast<uint8_t>((currentPixel & 0x02u) >> 1);
                    break;
                case 2:
                    currentPixel = static_cast<uint8_t>((currentPixel & 0x04u) >> 2);
                    break;
                case 3:
                    currentPixel = static_cast<uint8_t>((currentPixel & 0x08u) >> 3);
                    break;
                case 4:
                    currentPixel = static_cast<uint8_t>((currentPixel & 0x10u) >> 4);
                    break;
                case 5:
                    currentPixel = static_cast<uint8_t>((currentPixel & 0x20u) >> 5);
                    break;
                case 6:
                    currentPixel = static_cast<uint8_t>((currentPixel & 0x40u) >> 6);
                    break;
                case 7:
                    currentPixel = static_cast<uint8_t>((currentPixel & 0x80u) >> 7);
                    break;
                default:
                    currentPixel = 0;
                    break;
            }

            auto red = colorPalette.getColor(currentPixel).red;
            auto green = colorPalette.getColor(currentPixel).green;
            auto blue = colorPalette.getColor(currentPixel).blue;

            pixelBuf[currentRow * width + j] = Color(red, green, blue);
        }

        currentRow += bottomUpImage ? -1 : 1;
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

uint32_t Bmp::getPaddedWidth() {
    if((width % 4) != 0) {
        return (width + 4) - (width % 4);
    }

    return width;
}

void Bmp::saveToFile(const String &path) {
    File *file = File::open(path, "w+");

    if(file == nullptr) {
        return;
    }

    BitmapFileHeader fileHeader{0};
    fileHeader.identifier[0] = 'B';
    fileHeader.identifier[1] = 'M';
    fileHeader.fileSize = sizeof(BitmapFileHeader) + sizeof(BitmapInformationHeader) + sizeof(BitMask) + width * height * 4;
    fileHeader.reserved = 'B' | 'C' << 8 | 'F' << 16 | 'F' << 24;
    fileHeader.dataOffset = sizeof(BitmapFileHeader) + sizeof(BitmapInformationHeader) + sizeof(BitMask);

    BitmapInformationHeader infoHeader{0};
    infoHeader.headerSize = 40;
    infoHeader.bitmapWidth = width;
    infoHeader.bitmapHeight = -height;
    infoHeader.numColorPlanes = 1;
    infoHeader.bitmapDepth = 32;
    infoHeader.compression = BI_BITFIELDS;
    infoHeader.bitmapSize = width * height * 4;

    BitMask bitMask{0x00ff0000, 0x0000ff00, 0x000000ff};

    file->writeBytes(reinterpret_cast<char *>(&fileHeader), sizeof(BitmapFileHeader));
    file->writeBytes(reinterpret_cast<char *>(&infoHeader), sizeof(BitmapInformationHeader));
    file->writeBytes(reinterpret_cast<char *>(&bitMask), sizeof(BitMask));

    auto *pixelData = new uint32_t[infoHeader.bitmapSize];

    for(uint32_t i = 0; i < infoHeader.bitmapSize; i++) {
        pixelData[i] = pixelBuf[i].getRGB32();
    }

    file->writeBytes(reinterpret_cast<char *>(pixelData), infoHeader.bitmapSize);

    delete file;
}
