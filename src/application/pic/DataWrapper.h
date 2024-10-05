//
// Created by Rafael Reip on 04.10.24.
//

#ifndef HHUOS_DATAWRAPPER_H
#define HHUOS_DATAWRAPPER_H


#include "Layer.h"

class MouseInfo {
public:
    MouseInfo() : X(0), Y(0), leftButtonPressed(false) {}

    int X, Y;
    bool leftButtonPressed;
};

class RenderFlags {
public:
    RenderFlags() : anyChange(true), result(true), mouse(true), gui(true), workArea(true),
                    base(true), overlay(true), layers(true), layerOrder(true) {}

    bool anyChange;
    bool result;
    bool mouse;
    bool gui;
    bool workArea;
    bool base;
    bool overlay;
    bool layers;
    bool layerOrder;

    void mouseChanged() {
        anyChange = true;
        mouse = true;
    }

    void guiChanged() {
        anyChange = true;
        result = true;
        gui = true;
    }

    void baseChanged() {
        anyChange = true;
        result = true;
        base = true;
    }

    void overlayChanged() {
        anyChange = true;
        result = true;
        workArea = true;
        overlay = true;
    }

    void currentLayerChanged() {
        anyChange = true;
        result = true;
        workArea = true;
        layers = true;
    }

    void layerOrderChanged() {
        anyChange = true;
        result = true;
        workArea = true;
        layers = true;
        layerOrder = true;
    }

};

class RenderData {
public:
    explicit RenderData(MouseInfo *mouse, Layer **layers, int layerCount, int currentLayer, RenderFlags *flags)
            : mouse(mouse), layers(layers), layerCount(layerCount), currentLayer(currentLayer), flags(flags) {}

    MouseInfo *mouse;
    Layer **layers;
    int layerCount;
    int currentLayer;
    RenderFlags *flags;
};


#endif // HHUOS_DATAWRAPPER_H
