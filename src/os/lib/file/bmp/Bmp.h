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

#ifndef HHUOS_BMP_H
#define HHUOS_BMP_H

#include <lib/graphic/Image.h>
#include "lib/file/File.h"

class Bmp : public Image {

private:

    enum BitmapHeaderVersion : uint32_t {
        BITMAPCOREHEADER = 0x0c,
        BITMAPCOREHEADER2_LONG = 0x40,
        BITMAPCOREHEADER2_SHORT = 0x10,
        BITMAPINFOHEADER = 0x28,
        BITMAPV2INFOHEADER = 0x34,
        BITMAPV3INFOHEADER = 0x38,
        BITMAPV4HEADER = 0x6c,
        BITMAPV5HEADER = 0x7c
    };

    enum CompressionMethod : uint32_t {
        BI_RGB = 0x00,
        BI_RLE8 = 0x01,
        BI_RLE4 = 0x02,
        BI_BITFIELDS = 0x03,
        BI_JPEG = 0x04,
        BI_PNG = 0x05,
        BI_ALPHABITFIELDS = 0x06,
        BI_CMYK = 0x0B,
        BI_CMYKRLE8 = 0x0C,
        BI_CMYKRLE4 = 0x0D
    };

    struct BitMask {
        uint32_t red;
        uint32_t green;
        uint32_t blue;
    } __attribute__ ((packed));

    struct ColorPaletteEntry {
        uint8_t blue;
        uint8_t green;
        uint8_t red;
        uint8_t reserved;
    } __attribute__ ((packed));

    struct LegacyColorPaletteEntry {
        uint8_t blue;
        uint8_t green;
        uint8_t red;
    } __attribute__ ((packed));

    struct ColorPalette {
    private:

        ColorPaletteEntry *colorPalette = nullptr;
        LegacyColorPaletteEntry *legacyColorPalette = nullptr;

        bool useLegacyPalette = false;

    public:

        ColorPalette() = default;

        ColorPalette(BitmapHeaderVersion headerVersion, void *paletteStart) {
            colorPalette = (ColorPaletteEntry*) paletteStart;
            legacyColorPalette = (LegacyColorPaletteEntry*) paletteStart;

            if(headerVersion == BITMAPCOREHEADER) {
                useLegacyPalette = true;
            }
        }

        ColorPaletteEntry getColor(uint32_t index) {
            if(useLegacyPalette) {
                ColorPaletteEntry ret{0};

                ret.red = legacyColorPalette[index].red;
                ret.green = legacyColorPalette[index].green;
                ret.blue = legacyColorPalette[index].blue;

                return ret;
            }

            return colorPalette[index];
        }
    };

    struct BitmapFileHeader {
        char identifier[2];
        uint32_t fileSize;
        uint32_t reserved;
        uint32_t dataOffset;
    } __attribute__ ((packed));

    struct BitmapCoreHeaderV1 {
        uint32_t headerSize;
        uint16_t bitmapWidth;
        uint16_t bitmapHeight;
        uint16_t numColorPlanes;
        uint16_t bitmapDepth;
    };

    struct BitmapCoreHeaderV2 {
        uint32_t headerSize;
        uint32_t bitmapWidth;
        uint32_t bitmapHeight;
        uint16_t numColorPlanes;
        uint16_t bitmapDepth;
    } __attribute__ ((packed));

    struct BitmapInformationHeader {
        uint32_t headerSize;
        int32_t bitmapWidth;
        int32_t bitmapHeight;
        uint16_t numColorPlanes;
        uint16_t bitmapDepth;
        CompressionMethod compression;
        uint32_t bitmapSize;
        int32_t horizontalResolution;
        int32_t verticalResolution;
        uint32_t numColorsInPalette;
        uint32_t numImportantColors;
    } __attribute__ ((packed));

    struct BitmapHeader {

        BitmapHeaderVersion version = BITMAPINFOHEADER;
        uint32_t headerSize = 0;
        int32_t bitmapWidth = 0;
        int32_t bitmapHeight = 0;
        uint16_t numColorPlanes = 0;
        uint16_t bitmapDepth = 0;
        CompressionMethod compression = BI_RGB;
        uint32_t bitmapSize = 0;
        int32_t horizontalResolution = 0;
        int32_t verticalResolution = 0;
        uint32_t numColorsInPalette = 0;
        uint32_t numImportantColors = 0;

    public:

        BitmapHeader() = default;

        explicit BitmapHeader(void *headerStart) {
            version = *((BitmapHeaderVersion *) headerStart);

            if(version == BITMAPCOREHEADER) {
                BitmapCoreHeaderV1 header = *((BitmapCoreHeaderV1*) headerStart);

                headerSize = header.headerSize;
                bitmapWidth = header.bitmapWidth;
                bitmapHeight = header.bitmapHeight;
                numColorPlanes = header.numColorPlanes;
                bitmapDepth = header.bitmapDepth;
                compression = CompressionMethod::BI_RGB;
            } else if(version == BITMAPCOREHEADER2_SHORT || version == BITMAPCOREHEADER2_LONG) {
                BitmapCoreHeaderV2 header = *((BitmapCoreHeaderV2*) headerStart);

                headerSize = header.headerSize;
                bitmapWidth = header.bitmapWidth;
                bitmapHeight = header.bitmapHeight;
                numColorPlanes = header.numColorPlanes;
                bitmapDepth = header.bitmapDepth;
                compression = CompressionMethod::BI_RGB;
            } else {
                BitmapInformationHeader header = *((BitmapInformationHeader*) headerStart);

                headerSize = header.headerSize;
                bitmapWidth = header.bitmapWidth;
                bitmapHeight = header.bitmapHeight;
                numColorPlanes = header.numColorPlanes;
                bitmapDepth = header.bitmapDepth;
                compression = header.compression;
                bitmapSize = header.bitmapSize;
                horizontalResolution = header.horizontalResolution;
                verticalResolution = header.verticalResolution;
                numColorsInPalette = header.numColorsInPalette;
                numImportantColors = header.numImportantColors;
            }
        }
    };

private:

    BitmapFileHeader fileHeader;
    BitmapHeader infoHeader;
    BitMask bitMask;
    ColorPalette colorPalette;

    char *rawData = nullptr;

    bool bottomUpImage = true;
    bool ignoreAlpha = true;

private:

    uint8_t countTrailingZeros(uint32_t number);

    uint8_t countOnes(uint32_t number);

    uint32_t getPaddedWidth();

    void processData();

    uint8_t * decodeRLE4(uint8_t *encodedData);

    uint8_t * decodeRLE8(uint8_t *encodedData);

    void read32BitImage(uint8_t *rawPixelData);

    void read24BitImage(uint8_t *rawPixelData);

    void read16BitImage(uint8_t *rawPixelData);

    void read8BitImage(uint8_t *rawPixelData);

    void read4BitImage(uint8_t *rawPixelData);

    void read1BitImage(uint8_t *rawPixelData);

public:

    explicit Bmp(File *file);

    explicit Bmp(const Bmp &other);

    Bmp(Color *pixelBuf, uint32_t width, uint32_t height);

    ~Bmp();

    void saveToFile(const String &path);
};

#endif
