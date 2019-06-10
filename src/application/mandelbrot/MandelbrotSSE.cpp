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

#include "Mandelbrot.h"

#pragma GCC target("sse2")

#include "xmmintrin.h"

// Source : https://github.com/skeeto/mandel-simd
__attribute__((force_align_arg_pointer)) void drawMandelbrotSSE(float xlim[2], float ylim[2], uint16_t globalOffsetX, uint16_t globalOffsetY) {

    Mandelbrot::state.xmin = _mm_set_ps1(xlim[0]);
    Mandelbrot::state.ymin = _mm_set_ps1(ylim[0]);
    Mandelbrot::state.xscale = _mm_set_ps1((xlim[1] - xlim[0]) / Mandelbrot::properties.width);
    Mandelbrot::state.yscale = _mm_set_ps1((ylim[1] - ylim[0]) / Mandelbrot::properties.height);
    Mandelbrot::state.threshold = _mm_set_ps1(4);
    Mandelbrot::state.one = _mm_set_ps1(1);
    Mandelbrot::state.iter_scale = _mm_set_ps1(1.0F / Mandelbrot::properties.iterations);
    Mandelbrot::state.depth_scale = _mm_set_ps1(Mandelbrot::properties.depth - 1);

    for (uint16_t y = 0; y < Mandelbrot::properties.height; y++) {

        for (uint16_t x = 0; x < Mandelbrot::properties.width; x += 4) {

            Mandelbrot::state.mx = _mm_set_ps(x + 3, x + 2, x + 1, x + 0);
            Mandelbrot::state.my = _mm_set_ps1(y);
            Mandelbrot::state.cr = _mm_add_ps(_mm_mul_ps(Mandelbrot::state.mx, Mandelbrot::state.xscale), Mandelbrot::state.xmin);
            Mandelbrot::state.ci = _mm_add_ps(_mm_mul_ps(Mandelbrot::state.my, Mandelbrot::state.yscale), Mandelbrot::state.ymin);
            Mandelbrot::state.zr = Mandelbrot::state.cr;
            Mandelbrot::state.zi = Mandelbrot::state.ci;

            uint32_t k = 1;
            Mandelbrot::state.mk = _mm_set_ps1(k);

            while (++k < Mandelbrot::properties.iterations) {

                Mandelbrot::state.zr2 = _mm_mul_ps(Mandelbrot::state.zr, Mandelbrot::state.zr);
                Mandelbrot::state.zi2 = _mm_mul_ps(Mandelbrot::state.zi, Mandelbrot::state.zi);
                Mandelbrot::state.zrzi = _mm_mul_ps(Mandelbrot::state.zr, Mandelbrot::state.zi);
                Mandelbrot::state.zr = _mm_add_ps(_mm_sub_ps(Mandelbrot::state.zr2, Mandelbrot::state.zi2), Mandelbrot::state.cr);
                Mandelbrot::state.zi = _mm_add_ps(_mm_add_ps(Mandelbrot::state.zrzi, Mandelbrot::state.zrzi), Mandelbrot::state.ci);

                Mandelbrot::state.zr2 = _mm_mul_ps(Mandelbrot::state.zr, Mandelbrot::state.zr);
                Mandelbrot::state.zi2 = _mm_mul_ps(Mandelbrot::state.zi, Mandelbrot::state.zi);
                Mandelbrot::state.mag2 = _mm_add_ps(Mandelbrot::state.zr2, Mandelbrot::state.zi2);
                Mandelbrot::state.mask = _mm_cmplt_ps(Mandelbrot::state.mag2, Mandelbrot::state.threshold);
                Mandelbrot::state.mk = _mm_add_ps(_mm_and_ps(Mandelbrot::state.mask, Mandelbrot::state.one), Mandelbrot::state.mk);

                if (_mm_movemask_ps(Mandelbrot::state.mask) == 0) {

                    break;
                }
            }

            Mandelbrot::state.mk = _mm_mul_ps(Mandelbrot::state.mk, Mandelbrot::state.iter_scale);
            Mandelbrot::state.mk = _mm_sqrt_ps(Mandelbrot::state.mk);
            Mandelbrot::state.mk = _mm_mul_ps(Mandelbrot::state.mk, Mandelbrot::state.depth_scale);
            Mandelbrot::state.pixels = _mm_cvtps_epi32(Mandelbrot::state.mk);

            auto *src = reinterpret_cast<uint32_t *>(&Mandelbrot::state.pixels);

            for (uint8_t i = 0; i < 4; i++) {

                Mandelbrot::lfb->drawPixel(x + globalOffsetX + i, y + globalOffsetY, Mandelbrot::gradient.getColor(src[i] % Mandelbrot::MAX_COLORS));
            }
        }
    }
}