#ifndef HHUOS_COLORGRADIENT_H
#define HHUOS_COLORGRADIENT_H

#include <cstdint>
#include "Color.h"

class ColorGradient {

private:

    uint32_t numColors;

    Color *colors;

private:

    Color calculateColor(double value);

public:

    explicit ColorGradient(uint32_t numColors);

    ColorGradient(const ColorGradient &copy) = delete;

    ~ColorGradient();

    Color &getColor(uint32_t num);

};

#endif
