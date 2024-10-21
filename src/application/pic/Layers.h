//
// Created by Rafael Reip on 21.10.24.
//

#ifndef HHUOS_LAYERS_H
#define HHUOS_LAYERS_H


#include "Layer.h"

class Layers {
public:
    Layers();

//    ~Layers() = default;

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

    void addEmpty(int width, int height, int posX, int posY);

private:
    Layer **layers;
    int layerCount;
    int maxLayerCount;
    int currentLayer;
};


#endif //HHUOS_LAYERS_H
