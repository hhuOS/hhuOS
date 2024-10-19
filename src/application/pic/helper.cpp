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