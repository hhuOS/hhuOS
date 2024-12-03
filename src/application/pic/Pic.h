#ifndef PIC_H
#define PIC_H

#include "Button.h"
#include "Layers.h"
#include "GuiLayer.h"
#include "Renderer.h"
#include "DataWrapper.h"
#include "lib/util/base/ArgumentParser.h"


class Pic {
public:
    explicit Pic(Util::Io::File *lfbFile);

    ~Pic();

    void run();

private:
    DataWrapper *data;
    Renderer *renderer;

    void init_gui();

    void checkMouseInput();

    void checkKeyboardInput();

    void parseMouse(bool clicked);
};

#endif // PIC_H
