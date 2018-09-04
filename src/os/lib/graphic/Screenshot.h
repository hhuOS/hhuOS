#ifndef HHUOS_SCREENSHOT_H
#define HHUOS_SCREENSHOT_H

#include <lib/String.h>
#include "Image.h"

class Screenshot : public Image {

public:

    Screenshot() = default;

    ~Screenshot();

    void take();

    void saveToBmp(const String &path);

};

#endif
