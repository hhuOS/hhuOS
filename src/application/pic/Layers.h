//
// Created by Rafael Reip on 21.10.24.
//

#ifndef HHUOS_LAYERS_H
#define HHUOS_LAYERS_H


#include "Layer.h"
#include "History.h"

class History;

class Layers {
public:
    Layers(MessageHandler *mHandler, History *history);

    ~Layers() = default;

    int countNum() const;

    int currentNum() const;

    int maxNum() const;

    void setCurrent(int index);

    void setCurrentToNext();

    void reset();

    void deletetAt(int index, bool writeHistory = true);

    void swap(int index1, int index2, bool writeHistory = true);

    void changeVisibleAt(int index, bool writeHistory = true);

    void combine(int index1, int index2, bool writeHistory = true);

    void duplicate(int index, bool writeHistory = true);

    Layer *current();

    Layer *at(int index);

    void addPicture(const char *path, int posX, int posY, bool writeHistory = true);

    void addEmpty(int posX, int posY, int width, int height, bool writeHistory = true);

    void exportPicture(const char *path, int x, int y, int w, int h, bool png, bool jpg, bool bmp);

    // Layer operations:

    void move(int index, int x, int y, bool writeHistory = true);

    void moveCurrent(int x, int y);

    void scale(int index, double factor, ToolCorner kind, bool writeHistory = true);

    void scaleCurrent(double factor, ToolCorner kind);

    void crop(int index, int left, int right, int top, int bottom, bool writeHistory = true);

    void cropCurrent(int left, int right, int top, int bottom);

    void autoCrop(int index, bool writeHistory = true);

    void autoCropCurrent();

    void rotate(int index, int degree, bool writewriteHistory = true);

    void rotateCurrent(int degree);

    void drawCircle(int index, int x, int y, uint32_t color, int thickness);

    void drawCircleCurrent(int x, int y, uint32_t color, int thickness);

    void drawLine(int index, int x1, int y1, int x2, int y2, uint32_t color, int thickness, bool writeHistory = true);

    void drawLineCurrent(int x1, int y1, int x2, int y2, uint32_t color, int thickness);

    void drawShape(int index, Shape shape, int x, int y, int w, int h, uint32_t color, bool writeHistory = true);

    void drawShapeCurrent(Shape shape, int x, int y, int w, int h, uint32_t color);

    void prepareNextDrawing(int index, bool writeHistory = true);

    void prepareNextDrawingCurrent();

    History *history;
    Layer **layers;
    int layerCount;
    int currentLayer;

private:
    MessageHandler *mHandler;
    int maxLayerCount;
};


#endif //HHUOS_LAYERS_H
