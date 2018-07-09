
#ifndef __libBmp_include__
#define __libBmp_include__ 1

#include <cstdint>
#include "lib/file/File.h"

class Bmp {
    
private:

    struct Pixel {
        uint8_t blue;
        uint8_t green;
        uint8_t red;
    };

    int pos(int x, int y) {
        if (x<0) x=0;
        if (x>=width) x=width-1;
        if (y<0) y=0;
        if (y>=height) y=height-1;
        return ((height-y-1)*width +x);
    }
    
    Pixel * data;

public:
    
    int width;
    int height;

    Pixel & get(int x, int y) {
        return data[pos(x, y)];
    }

    uint8_t & B(int x, int y) {
        return *((uint8_t *) &(data[pos(x, y)]));
    }
    uint8_t & G(int x, int y) {
        // get address from 3byte Color, cast to "bytes" address, add +1
        // to Address (= 1byte step size) and get value of it
        return *( ((uint8_t *) &(data[pos(x, y)]))+1 );
    }
    uint8_t & R(int x, int y) {
        return *( ((uint8_t *) &(data[pos(x, y)]))+2 );
    }

    Bmp (File* filename);
    
    virtual ~Bmp();

    void print (int xpos, int ypos);

    struct InfoHeader {
        uint32_t    size;
        uint32_t    width;
        uint32_t    height;
        uint16_t    planes;
        uint16_t    bitCount;
        uint32_t    compression;
        uint32_t    imageSize;
        uint32_t    xpm;
        uint32_t    ypm;
        uint32_t    colorsUsed;
        uint32_t    importantColors;
    } __attribute__ ((packed));

    struct FileHeader {
        uint16_t    type;
        uint32_t    size;
        uint32_t    reserved;
        uint32_t    offset;
        InfoHeader  info;
    } __attribute__ ((packed));

    static const uint32_t   COMPRESSION_BI_RGB          = 0x0;
    static const uint32_t   COMPRESSION_BI_RLE8         = 0x1;
    static const uint32_t   COMPRESSION_BI_RLE4         = 0x2;
    static const uint32_t   COMPRESSION_BI_BITFIELDS    = 0x3;

};

#endif
