#ifndef HHUOS_BMP_H
#define HHUOS_BMP_H

#include "lib/file/File.h"

class Bmp {

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

private:

    BitmapFileHeader fileHeader;
    BitmapInformationHeader infoHeader;
    BitMask bitMask;
    ColorPaletteEntry *colorPalette = nullptr;

    char *rawData = nullptr;
    Color *processedPixels = nullptr;

    bool bottomUpImage = true;
    bool ignoreAlpha = true;

private:

    uint8_t countTrailingZeros(uint32_t number);

    uint8_t countOnes(uint32_t number);

    BitmapInformationHeader convertLegacyHeader(const BitmapCoreHeaderV1 &legacyHeader);

    BitmapInformationHeader convertLegacyHeader(const BitmapCoreHeaderV2 &legacyHeader);

    void processData();

    uint8_t * decodeRLE4(uint8_t *encodedData);

    uint8_t * decodeRLE8(uint8_t *encodedData);

    void read32BitImage(uint8_t *rawPixelData);

    void read24BitImage(uint8_t *rawPixelData);

    void read16BitImage(uint8_t *rawPixelData);

    void read8BitImage(uint8_t *rawPixelData);

    void read4BitImage(uint8_t *rawPixelData);

    void read2BitImage(uint8_t *rawPixelData);

    void read1BitImage(uint8_t *rawPixelData);

public:

    explicit Bmp(File *file);

    explicit Bmp(char *data);

    Bmp(const Bmp &copy) = delete;

    ~Bmp();

    uint32_t getHeight();

    uint32_t getWidth();

    uint8_t getColorDepth();

    void draw(uint16_t x, uint16_t y);
};

#endif
