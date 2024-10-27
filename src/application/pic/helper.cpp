#include "helper.h"

uint32_t blendPixels(uint32_t lower, uint32_t upper) {
    uint8_t upperAlpha = (upper >> 24) & 0xFF;
    uint8_t lowerAlpha = (lower >> 24) & 0xFF;
    if (upperAlpha == 0xFF || lowerAlpha == 0x00) return upper;
    else if (upperAlpha == 0x00) return lower;
    uint8_t inverseAlpha = 255 - upperAlpha;
    uint8_t r = ((upper >> 16) & 0xFF) * upperAlpha / 255 + ((lower >> 16) & 0xFF) * inverseAlpha / 255;
    uint8_t g = ((upper >> 8) & 0xFF) * upperAlpha / 255 + ((lower >> 8) & 0xFF) * inverseAlpha / 255;
    uint8_t b = (upper & 0xFF) * upperAlpha / 255 + (lower & 0xFF) * inverseAlpha / 255;
    uint8_t a = upperAlpha + (lowerAlpha * inverseAlpha) / 255;
    return (a << 24) | (r << 16) | (g << 8) | b;
}

void blendBuffers(uint32_t *lower, const uint32_t *upper, int size) {
    for (int i = 0; i < size; i++) {
        lower[i] = blendPixels(lower[i], upper[i]);
    }
}

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

const char *double_to_string(double value, int decimal_places) {
    static char buffer[32];  // Adjust size as needed
    char *p = buffer;
    int integer_part;
    double fractional_part;
    int i, digit;

    // Handle negative numbers
    if (value < 0) {
        *p++ = '-';
        value = -value;
    }

    // Split into integer and fractional parts
    integer_part = (int) value;
    fractional_part = value - integer_part;

    // Convert integer part
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

    // Add decimal point if needed
    if (decimal_places > 0) {
        *p++ = '.';
    }

    // Convert fractional part
    for (i = 0; i < decimal_places; i++) {
        fractional_part *= 10;
        digit = (int) fractional_part;
        *p++ = '0' + digit;
        fractional_part -= digit;
    }

    // Null terminate
    *p = '\0';

    return buffer;
}

int min(int a, int b) {
    return a < b ? a : b;
}

double min(double a, double b) {
    return a < b ? a : b;
}

int max(int a, int b) {
    return a > b ? a : b;
}

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

} // namespace Bitmaps