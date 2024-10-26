//
// Created by Rafael Reip on 21.10.24.
//

#ifndef HHUOS_LAYERS_H
#define HHUOS_LAYERS_H


#include "Layer.h"

class Layers {
public:
    Layers(MessageHandler *mHandler);

    ~Layers() = default;

    int countNum() const;

    int currentNum() const;

    int maxNum() const;

    void setCurrent(int index);

    void setCurrentToNext();

    void deletetAt(int index);

    void swap(int index1, int index2);

    void combine(int index1, int index2);

    Layer *current();

    Layer *at(int index);

    void addPicture(const char *path, int posX, int posY);

    void addEmpty(int posX, int posY, int width, int height);

    void exportPicture(const char *path, int x, int y, int w, int h, bool png, bool jpg, bool bmp);

    // Layer operations:

    void move(int index, int x, int y);

    void moveCurrent(int x, int y);

    void scale(int index, double factor, ToolCorner kind);

    void scaleCurrent(double factor, ToolCorner kind);

    void crop(int index, int left, int right, int top, int bottom);

    void cropCurrent(int left, int right, int top, int bottom);

    void rotate(int index, int degree);

    void rotateCurrent(int degree);

    void drawCircle(int index, int x, int y, uint32_t color, int thickness);

    void drawCircleCurrent(int x, int y, uint32_t color, int thickness);

    void drawLine(int index, int x1, int y1, int x2, int y2, uint32_t color, int thickness);

    void drawLineCurrent(int x1, int y1, int x2, int y2, uint32_t color, int thickness);

    void prepareNextDrawing(int index);

    void prepareNextDrawingCurrent();

private:
    MessageHandler *mHandler;
    Layer **layers;
    int layerCount;
    int maxLayerCount;
    int currentLayer;
};


#endif //HHUOS_LAYERS_H
