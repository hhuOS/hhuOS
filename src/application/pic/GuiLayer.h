//
// Created by Rafael Reip on 06.10.24.
//

#ifndef HHUOS_GUI_LAYER_H
#define HHUOS_GUI_LAYER_H

class Button;

class GuiLayer {
public:
    GuiLayer();

    ~GuiLayer();

    void addButton(Button *button);

    void appear() const;

    Button **buttons;
    int buttonCount;
};

#endif // HHUOS_GUI_LAYER_H
