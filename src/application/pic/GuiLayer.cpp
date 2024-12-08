#include "GuiLayer.h"

#include "Button.h"

GuiLayer::GuiLayer() {
    this->buttonCount = 0;
    this->buttons = new Button *[20];
}

GuiLayer::~GuiLayer() {
    for (int i = 0; i < buttonCount; i++) {
        delete buttons[i];
    }
    delete[] buttons;
}

void GuiLayer::addButton(Button *button) {
    buttons[buttonCount] = button;
    buttonCount++;
}

void GuiLayer::appear() const {
    for (int i = 0; i < buttonCount; i++) {
        buttons[i]->removeInteraction();
    }
}
