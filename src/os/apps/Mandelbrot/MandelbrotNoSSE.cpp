#include <lib/math/Complex.h>
#include "Mandelbrot.h"

void Mandelbrot::drawMandelbrotNoSSE(float offsetX, float offsetY, float zoom) {
    bool notInSet = false;
    uint32_t i;

    for (uint16_t y = 0; y < Mandelbrot::properties.height; y++) {
        for (uint16_t x = 0; x < Mandelbrot::properties.width; x++) {

            Complex c((-2.0F + realBase * x) / zoom + offsetX,
                      (-2.0F + imaginaryBase * y) / zoom + offsetY);

            Complex z(0.0, 0.0);

            for (i = 0; i < MAX_VALUE; i++) {

                z = z * z + c;

                if (z.squaredAbsolute() >= 4.0) {

                    notInSet = true;

                    break;
                }
            }

            if(notInSet) {
                lfb->drawPixel(x, y, gradient.getColor(i % MAX_COLORS));
            } else {
                lfb->drawPixel(x, y, Colors::BLACK);
            }

            notInSet = false;
        }
    }
}