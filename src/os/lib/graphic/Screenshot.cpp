#include <lib/file/bmp/Bmp.h>
#include "Screenshot.h"

Screenshot::~Screenshot() {
    if(pixelBuf != nullptr) {
        delete pixelBuf;
    }
}

void Screenshot::take() {
    LinearFrameBuffer *lfb = Kernel::getService<GraphicsService>()->getLinearFrameBuffer();

    width = lfb->getResX();
    height = lfb->getResY();
    depth = 32;

    if(pixelBuf != nullptr) {
        delete pixelBuf;
    }

    pixelBuf = new Color[width * height];

    for(uint16_t i = 0; i < height; i++) {
        for(uint16_t j = 0; j < width; j++) {
            lfb->readPixel(j, i, pixelBuf[i * width + j]);
        }
    }
}

void Screenshot::saveToBmp(const String &path) {
    if(pixelBuf == nullptr) {
        return;
    }

    Bmp bmp(pixelBuf, width, height);
    bmp.saveToFile(path);
}
