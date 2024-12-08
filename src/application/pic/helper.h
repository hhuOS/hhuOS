#ifndef HELPER_H
#define HELPER_H

#include "lib/util/io/stream/PrintStream.h"
#include "lib/util/base/System.h"

#define print(i) Util::System::out << i << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush

#define  PI 3.14159265358979323846

enum Tool {
    // tools
    NOTHING, MOVE, ROTATE, SCALE, CROP, PEN, ERASER, COLOR, SHAPE, REPLACE_COLOR,
    // layer tools
    EXPORT_PNG, EXPORT_JPG, EXPORT_BMP, NEW_EMPTY, COMBINE, DUPLICATE,
};

enum ToolCorner {
    TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT,
};

enum Shape {
    DEFAULT, RECTANGLE, SQUARE, CIRCLE, ELLIPSE,
};

class RenderFlags {
public:

    bool anyChange = true;
    bool result = true;
    bool mouse = true;
    bool gui = true;
    bool guiLayer = true;
    bool workArea = true;
    bool base = true;
    bool overlay = true;
    bool messages = true;
    bool layers = true;
    bool layerOrder = true;

    void mouseChanged() { anyChange = true, mouse = true; }

    void guiButtonChanged() { anyChange = true, result = true, gui = true; }

    void guiLayerChanged() { anyChange = true, result = true, gui = true, guiLayer = true; }

    void baseChanged() { anyChange = true, result = true, base = true; }

    void overlayChanged() { anyChange = true, result = true, workArea = true, overlay = true; }

    void messagesChanged() { anyChange = true, result = true, workArea = true, messages = true; }

    void currentLayerChanged() { anyChange = true, result = true, workArea = true, layers = true; }

    void layerOrderChanged() { anyChange = true, result = true, workArea = true, overlay = true, layers = true, layerOrder = true; }

    void allChanged() {
        anyChange = true, result = true, mouse = true, gui = true, guiLayer = true, workArea = true, base = true, overlay = true, messages = true, layers = true, layerOrder = true;
    }

};

uint32_t blendPixels(uint32_t lower, uint32_t upper);

void blendBuffers(uint32_t *lower, const uint32_t *upper, int size);

void blendBuffers(uint32_t *lower, const uint32_t *upper, int lx, int ly, int ux, int uy, int px, int py);

const char *int_to_string(int value);

const char *double_to_string(double value, int decimal_places);

int min(int a, int b);

double min(double a, double b);

int max(int a, int b);

double max(double a, double b);

namespace Bitmaps {

    extern uint8_t arrow_up[];
    extern uint8_t arrow_down[];
    extern uint8_t arrow_right[];
    extern uint8_t arrow_left[];
    extern uint8_t trashcan[];
    extern uint8_t eye[];
    extern uint8_t arrow_back[];
    extern uint8_t arrow_forward[];
    extern uint8_t cross[];
    extern uint8_t checkmark[];
    extern uint8_t tool[];
    extern uint8_t play[];
    extern uint8_t brackets[];
    extern uint8_t mouse[];

} // namespace Bitmaps

#endif // HELPER_H

