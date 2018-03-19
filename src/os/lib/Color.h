#ifndef __Color_include__
#define __Color_include__

#include <cstdint>

class Color {

private:
    void invalidate();
    uint32_t calcRGB32();
    uint32_t calcRGB24();
    uint16_t calcRGB16();
    uint16_t calcRGB15();
    uint8_t calcRGB8();
    uint8_t calcRGB4();
    uint8_t calcRGB2();
    uint8_t calcRGB1();

    uint8_t red, green, blue, alpha;
    int32_t rgb32 = -1;
    int32_t rgb24 = -1;
    int16_t rgb16 = -1;
    int16_t rgb15 = -1;
    int8_t rgb8 = -1;
    int8_t rgb4 = -1;
    int8_t rgb2 = -1;
    int8_t rgb1 = -1;

public:
    Color() : red(0), green(0), blue(0), alpha(255) {}
    Color(uint8_t arg_red, uint8_t arg_green, uint8_t arg_blue) : red(arg_red), green(arg_green), blue(arg_blue), alpha(255) {}
    Color(uint8_t arg_red, uint8_t arg_green, uint8_t arg_blue, uint8_t arg_alpha) : red(arg_red), green(arg_green), blue(arg_blue), alpha(arg_alpha) {}

    Color(uint32_t rgb, uint8_t depth) {
        switch(depth) {
            case 32:
                setRGB32(rgb);
                break;
            case 24:
                setRGB24(rgb);
                break;
            case 16:
                setRGB16(rgb);
                break;
            case 15:
                setRGB15(rgb);
                break;
            case 8:
                setRGB8(rgb);
                break;
            case 4:
                setRGB4(rgb);
                break;
            case 2:
                setRGB2(rgb);
                break;
            case 1:
                setRGB1(rgb);
                break;
            default:
                setRGB32(rgb);
                break;
        }
    }

    void setRed(uint8_t red);
    void setGreen(uint8_t green);
    void setBlue(uint8_t blue);
    void setAlpha(uint8_t alpha);

    void setRGB(uint8_t red, uint8_t green, uint8_t blue);
    void setRGBA(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
    void setRGB(uint32_t rgb, uint8_t depth);
    void setRGB32(uint32_t rgba);
    void setRGB24(uint32_t rgb);
    void setRGB16(uint16_t rgb);
    void setRGB15(uint16_t rgb);
    void setRGB8(uint8_t rgb);
    void setRGB4(uint8_t rgb);
    void setRGB2(uint8_t rgb);
    void setRGB1(uint8_t rgb);

    uint8_t getRed();
    uint8_t getGreen();
    uint8_t getBlue();
    uint8_t getAlpha();
    uint32_t getRGB32();
    uint32_t getRGB24();
    uint16_t getRGB16();
    uint16_t getRGB15();
    uint8_t getRGB8();
    uint8_t getRGB4();
    uint8_t getRGB2();
    uint8_t getRGB1();
    uint32_t getColorForDepth(uint8_t depth);

    void blendWith(Color color);
};

#endif