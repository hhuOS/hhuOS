//
// Created by Rafael Reip on 06.10.24.
//

#ifndef HHUOS_BUTTON_H
#define HHUOS_BUTTON_H

#include <cstdint>

// Forward declaration since otherwise circular dependency
class DataWrapper;

enum ButtonType {
    NAVIGATION,
    VALUE,
    CONFIRM,
    COLOR,
    LAYER,
    INPUT
};

class Button {
public:

    explicit Button(void (*clickMethod)(DataWrapper *), DataWrapper *data);

    ~Button() = default;

    void processClick(int relX, int relY);

    void showClick(int relX, int relY);

    void showHover(int relX, int relY);

    void removeInteraction();

    uint32_t *getBuffer();

    bool bufferChanged;

private:
    void (*clickMethod)(DataWrapper *data);

    DataWrapper *data;
    bool click;
    bool hover;


protected:
    void render();

    uint32_t *buffer;
};

#endif // HHUOS_BUTTON_H
