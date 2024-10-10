#ifndef PIC_H
#define PIC_H

#include "Button.h"
#include "Layer.h"
#include "Renderer.h"
#include "DataWrapper.h"


class Pic {
public:
    explicit Pic();

    ~Pic() = default;

    void run();

private:
    DataWrapper *data;
    Renderer *renderer;

    void init_gui();

    void checkMouseInput();

    void checkKeyboardInput();

    void parseMouseToGui();
};

#endif // PIC_H
