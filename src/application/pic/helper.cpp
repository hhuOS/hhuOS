#include "helper.h"

/**
 * Blends two pixels together using alpha blending.
 *
 * @param lower The lower pixel value in ARGB format.
 * @param upper The upper pixel value in ARGB format.
 * @return The blended pixel value in ARGB format.
 */
uint32_t blendPixels(uint32_t lower, uint32_t upper) {
    uint8_t upperAlpha = (upper >> 24) & 0xFF;
    uint8_t lowerAlpha = (lower >> 24) & 0xFF;
    if (upperAlpha == 0xFF || lowerAlpha == 0x00) return upper;
    if (upperAlpha == 0x00) return lower;
    uint8_t inverseAlpha = 255 - upperAlpha;

#define DIV255(color, alpha) (((color) * (alpha) + 128) * 257 >> 16) // approx. division by 255
    uint8_t r = DIV255((upper >> 16) & 0xFF, upperAlpha) + DIV255((lower >> 16) & 0xFF, inverseAlpha);
    uint8_t g = DIV255((upper >> 8) & 0xFF, upperAlpha) + DIV255((lower >> 8) & 0xFF, inverseAlpha);
    uint8_t b = DIV255(upper & 0xFF, upperAlpha) + DIV255(lower & 0xFF, inverseAlpha);
    uint8_t a = upperAlpha + DIV255(lowerAlpha, inverseAlpha);
    return (a << 24) | (r << 16) | (g << 8) | b;
}

/**
 * Blends two buffers together using alpha blending.
 *
 * @param lower The lower buffer in ARGB format.
 * @param upper The upper buffer in ARGB format.
 * @param size The size of the buffers.
 */
void blendBuffers(uint32_t *lower, const uint32_t *upper, int size) {
    for (int i = 0; i < size; i++) {
        lower[i] = blendPixels(lower[i], upper[i]);
    }
}

/**
 * Blends two buffers together using alpha blending with specified offsets.
 *
 * @param lower The lower buffer in ARGB format.
 * @param upper The upper buffer in ARGB format.
 * @param lx The width of the lower buffer.
 * @param ly The height of the lower buffer.
 * @param ux The width of the upper buffer.
 * @param uy The height of the upper buffer.
 * @param px The x offset for the upper buffer.
 * @param py The y offset for the upper buffer.
 */
void blendBuffers(uint32_t *lower, const uint32_t *upper, int lx, int ly, int ux, int uy, int px, int py) {
    for (int y = 0; y < uy; y++) {
        int lowerY = y + py;
        if (lowerY < 0 || lowerY >= ly) continue;
        for (int x = 0; x < ux; x++) {
            int lowerX = x + px;
            if (lowerX < 0 || lowerX >= lx) continue;
            int upperIndex = y * ux + x;
            int lowerIndex = lowerY * lx + lowerX;
            lower[lowerIndex] = blendPixels(lower[lowerIndex], upper[upperIndex]);
        }
    }
}

/**
 * Converts an integer to a string.
 *
 * @param value The integer value to convert.
 * @return A pointer to the resulting null-terminated string.
 */
const char *int_to_string(int value) {
    static char buffer[12];
    char *p = buffer + 11;
    int is_negative = 0;
    *p = '\0';
    if (value < 0) {
        is_negative = 1;
        value = -value;
    } else if (value == 0) {
        *--p = '0';
        return p;
    }
    while (value > 0) {
        *--p = (value % 10) + '0';
        value /= 10;
    }
    if (is_negative) {
        *--p = '-';
    }
    return p;
}

/**
 * Converts a double to a string with a specified number of decimal places.
 *
 * @param value The double value to convert.
 * @param decimal_places The number of decimal places to include in the string.
 * @return A pointer to the resulting null-terminated string.
 */
const char *double_to_string(double value, int decimal_places) {
    static char buffer[32];
    char *p = buffer;
    int integer_part;
    double fractional_part;
    int i, digit;

    if (value < 0) {
        *p++ = '-';
        value = -value;
    }

    integer_part = (int) value;
    fractional_part = value - integer_part;

    if (integer_part == 0) {
        *p++ = '0';
    } else {
        char temp[16];
        int temp_index = 0;
        while (integer_part > 0) {
            temp[temp_index++] = '0' + (integer_part % 10);
            integer_part /= 10;
        }
        while (temp_index > 0) {
            *p++ = temp[--temp_index];
        }
    }

    if (decimal_places > 0) {
        *p++ = '.';
    }

    for (i = 0; i < decimal_places; i++) {
        fractional_part *= 10;
        digit = (int) fractional_part;
        *p++ = '0' + digit;
        fractional_part -= digit;
    }

    *p = '\0';

    return buffer;
}

/**
 * Returns the minimum of two integer values.
 */
int min(int a, int b) {
    return a < b ? a : b;
}

/**
 * Returns the minimum of two double values.
 */
double min(double a, double b) {
    return a < b ? a : b;
}

/**
 * Returns the maximum of two integer values.
 */
int max(int a, int b) {
    return a > b ? a : b;
}

/**
 * Returns the maximum of two double values.
 */
double max(double a, double b) {
    return a > b ? a : b;
}

namespace Bitmaps {

    uint8_t arrow_up[] = {
            0x01, 0x80,
            0x03, 0xC0,
            0x07, 0xE0,
            0x0F, 0xF0,
            0x1F, 0xF8,
            0x3F, 0xFC,
            0x7F, 0xFE,
            0xFF, 0xFF,
            0x07, 0xE0,
            0x07, 0xE0,
            0x07, 0xE0,
            0x07, 0xE0,
            0x07, 0xE0,
            0x07, 0xE0,
            0x07, 0xE0,
            0x07, 0xE0,
    };

    uint8_t arrow_down[] = {
            0x07, 0xE0,
            0x07, 0xE0,
            0x07, 0xE0,
            0x07, 0xE0,
            0x07, 0xE0,
            0x07, 0xE0,
            0x07, 0xE0,
            0x07, 0xE0,
            0xFF, 0xFF,
            0x7F, 0xFE,
            0x3F, 0xFC,
            0x1F, 0xF8,
            0x0F, 0xF0,
            0x07, 0xE0,
            0x03, 0xC0,
            0x01, 0x80,
    };

    uint8_t trashcan[] = {
            0x07, 0xE0,
            0x0F, 0xF0,
            0xFF, 0xFF,
            0xFF, 0xFF,
            0x20, 0x04,
            0x2D, 0xB4,
            0x2D, 0xB4,
            0x2D, 0xB4,
            0x2D, 0xB4,
            0x2D, 0xB4,
            0x2D, 0xB4,
            0x2D, 0xB4,
            0x2D, 0xB4,
            0x2D, 0xB4,
            0x20, 0x04,
            0x1F, 0xF8,
    };

    uint8_t eye[] = {
            0x00, 0x00,
            0x00, 0x00,
            0x00, 0x00,
            0x0F, 0xF0,
            0x3B, 0xDC,
            0x67, 0xE6,
            0xCF, 0xF3,
            0x8F, 0xF1,
            0x8F, 0xF1,
            0xCF, 0xF3,
            0x67, 0xE6,
            0x3B, 0xDC,
            0x0F, 0xF0,
            0x00, 0x00,
            0x00, 0x00,
            0x00, 0x00,
    };

    uint8_t arrow_back[] = {
            0x02, 0x00,
            0x06, 0x00,
            0x0E, 0x00,
            0x1E, 0x00,
            0x3F, 0xE0,
            0x7F, 0xF8,
            0xFF, 0xFC,
            0x7F, 0xFE,
            0x3F, 0xFE,
            0x1E, 0x7F,
            0x0E, 0x1F,
            0x06, 0x0F,
            0x02, 0x07,
            0x00, 0x03,
            0x00, 0x03,
            0x00, 0x01,
    };

    uint8_t arrow_forward[] = {
            0x00, 0x40,
            0x00, 0x60,
            0x00, 0x70,
            0x00, 0x78,
            0x07, 0xFC,
            0x1F, 0xFE,
            0x3F, 0xFF,
            0x7F, 0xFE,
            0x7F, 0xFC,
            0xFE, 0x78,
            0xF8, 0x70,
            0xF0, 0x60,
            0xE0, 0x40,
            0xC0, 0x00,
            0xC0, 0x00,
            0x80, 0x00,
    };

    uint8_t cross[] = {
            0x10, 0x08,
            0x38, 0x1C,
            0x7C, 0x3E,
            0xFE, 0x7F,
            0x7F, 0xFE,
            0x3F, 0xFC,
            0x1F, 0xF8,
            0x0F, 0xF0,
            0x0F, 0xF0,
            0x1F, 0xF8,
            0x3F, 0xFC,
            0x7F, 0xFE,
            0xFE, 0x7F,
            0x7C, 0x3E,
            0x38, 0x1C,
            0x10, 0x08,
    };

    uint8_t checkmark[] = {
            0x00, 0x03,
            0x00, 0x07,
            0x00, 0x0F,
            0x00, 0x1F,
            0x00, 0x3F,
            0x00, 0x7F,
            0xE0, 0xFF,
            0xF1, 0xFE,
            0xFB, 0xFC,
            0xFF, 0xF8,
            0xFF, 0xF0,
            0x7F, 0xE0,
            0x3F, 0xC0,
            0x1F, 0x80,
            0x0F, 0x00,
            0x06, 0x00,
    };

    uint8_t tool[] = {
            0x30, 0xF0,
            0x18, 0x78,
            0x98, 0x3C,
            0xF8, 0x3E,
            0x7C, 0x3F,
            0x0E, 0x7F,
            0x07, 0xE3,
            0x03, 0xC1,
            0x03, 0xC0,
            0x07, 0xE0,
            0x0E, 0x70,
            0x1C, 0x3E,
            0x38, 0x1F,
            0x70, 0x19,
            0xE0, 0x18,
            0x40, 0x0C,
    };

    uint8_t play[] = {
            0xC0, 0x00,
            0xF0, 0x00,
            0xFC, 0x00,
            0xFF, 0x00,
            0xFF, 0xC0,
            0xFF, 0xF0,
            0xFF, 0xFC,
            0xFF, 0xFF,
            0xFF, 0xFF,
            0xFF, 0xFC,
            0xFF, 0xF0,
            0xFF, 0xC0,
            0xFF, 0x00,
            0xFC, 0x00,
            0xF0, 0x00,
            0xC0, 0x00,
    };

    uint8_t brackets[] = {
            0x3F, 0xFC,
            0x70, 0x0E,
            0xE0, 0x07,
            0xC0, 0x03,
            0xC0, 0x03,
            0xC0, 0x03,
            0xC0, 0x03,
            0xC0, 0x03,
            0xC0, 0x03,
            0xC0, 0x03,
            0xC0, 0x03,
            0xC0, 0x03,
            0xC0, 0x03,
            0xE0, 0x07,
            0x7F, 0xFE,
            0x3F, 0xFC,
    };

    uint8_t mouse[] = {
            0x0F, 0xF0,
            0x19, 0x98,
            0x31, 0x8C,
            0x61, 0x86,
            0x61, 0x86,
            0x61, 0x86,
            0x61, 0x86,
            0x7F, 0xFE,
            0x60, 0x06,
            0x60, 0x06,
            0x60, 0x06,
            0x60, 0x06,
            0x60, 0x06,
            0x60, 0x06,
            0x7F, 0xFE,
            0x3F, 0xFC,
    };

    uint8_t arrow_right[] = {
            0x00, 0x80,
            0x00, 0xC0,
            0x00, 0xE0,
            0x00, 0xF0,
            0x00, 0xF8,
            0xFF, 0xFC,
            0xFF, 0xFE,
            0xFF, 0xFF,
            0xFF, 0xFF,
            0xFF, 0xFE,
            0xFF, 0xFC,
            0x00, 0xF8,
            0x00, 0xF0,
            0x00, 0xE0,
            0x00, 0xC0,
            0x00, 0x80,
    };

    uint8_t arrow_left[] = {
            0x01, 0x00,
            0x03, 0x00,
            0x07, 0x00,
            0x0F, 0x00,
            0x1F, 0x00,
            0x3F, 0xFF,
            0x7F, 0xFF,
            0xFF, 0xFF,
            0xFF, 0xFF,
            0x7F, 0xFF,
            0x3F, 0xFF,
            0x1F, 0x00,
            0x0F, 0x00,
            0x07, 0x00,
            0x03, 0x00,
            0x01, 0x00,
    };

} // namespace Bitmaps