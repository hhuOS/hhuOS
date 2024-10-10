//
// Created by Rafael Reip on 06.10.24.
//

#ifndef HHUOS_GUI_LAYER_H
#define HHUOS_GUI_LAYER_H

#include "Button.h"

class GuiLayer {
public:
    GuiLayer(){
        this->buttonCount = 0;
        this->buttons = new Button*[20];
    }
    void addButton(Button *button){
        buttons[buttonCount] = button;
        buttonCount++;
    }
    Button ** buttons;
    int buttonCount;
};

#endif // HHUOS_GUI_LAYER_H
