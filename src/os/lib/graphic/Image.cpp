#include <kernel/services/GraphicsService.h>
#include <kernel/Kernel.h>
#include "Image.h"

extern "C" {
#include <lib/libc/math.h>
}

void Image::draw(uint16_t x, uint16_t y) {
    auto *lfb = Kernel::getService<GraphicsService>()->getLinearFrameBuffer();

    for(uint16_t i = 0; i < height; i++) {
        for(uint16_t j = 0; j < width; j++) {
            lfb->drawPixel(x + j, y + i, pixelBuf[i * width + j]);
        }
    }
}

uint32_t Image::getHeight() {
    return height;
}

uint32_t Image::getWidth() {
    return width;
}

uint8_t Image::getColorDepth() {
    return depth;
}

void Image::scaleUp(uint8_t factor) {
    if(factor <= 1) {
        return;
    }

    uint32_t newHeight = height * factor;
    uint32_t newWidth = width * factor;

    auto *scaledPixelBuf = new Color[newHeight * newWidth];

    for(uint32_t i = 0; i < height; i++) {
        for(uint32_t j = 0; j < width; j++) {
            const Color &currentPixel = pixelBuf[i * width + j];

            for(uint8_t k = 0; k < factor; k++) {
                for(uint8_t l = 0; l < factor; l++) {
                    scaledPixelBuf[(i * factor + k) * newWidth + j * factor + l] = Color(currentPixel);
                }
            }
        }
    }

    delete pixelBuf;

    height = newHeight;
    width = newWidth;
    pixelBuf = scaledPixelBuf;
}

void Image::scaleDown(uint8_t factor) {
    if(factor <= 1) {
        return;
    }

    uint32_t newHeight = height / factor;
    uint32_t newWidth = width / factor;

    auto *scaledPixelBuf = new Color[newHeight * newWidth];

    for(uint32_t i = 0; i < newHeight; i++) {
        for(uint32_t j = 0; j < newWidth; j++) {
            float red = 0;
            float blue = 0;
            float green = 0;
            float alpha = 0;

            for(uint8_t k = 0; k < factor; k++) {
                for(uint8_t l = 0; l < factor; l++) {
                    const Color &currentPixel = pixelBuf[(i * factor + k) * width + j * factor + l];

                    red += currentPixel.getRed();
                    blue += currentPixel.getBlue();
                    green += currentPixel.getGreen();
                    alpha += currentPixel.getAlpha();
                }
            }

            scaledPixelBuf[i * newWidth + j] = Color(static_cast<uint8_t>(red / pow(factor, 2)),
                                                  static_cast<uint8_t>(green / pow(factor, 2)),
                                                  static_cast<uint8_t>(blue / pow(factor, 2)),
                                                  static_cast<uint8_t>(alpha / pow(factor, 2)));
        }
    }

    delete pixelBuf;

    height = newHeight;
    width = newWidth;
    pixelBuf = scaledPixelBuf;
}
