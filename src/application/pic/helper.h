#ifndef HELPER_H
#define HELPER_H

#include <cstdint>

uint32_t blendPixels(uint32_t lower, uint32_t upper);

const char *int_to_string(int value);

const char *double_to_string(double value, int decimal_places);

#endif // HELPER_H
