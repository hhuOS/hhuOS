#include "GuiLayer.h"

#include "Button.h"

/**
 * Constructor for GuiLayer.
 * Initializes the button count to 0 and allocates memory for 20 buttons.
 */
GuiLayer::GuiLayer() {
    this->buttonCount = 0;
    this->buttons = new Button *[20]; // fits onto minimum screen resolution of 800x600
}

/**
 * Destructor for GuiLayer.
 */
GuiLayer::~GuiLayer() {
    for (int i = 0; i < buttonCount; i++) {
        delete buttons[i];
    }
    delete[] buttons;
}

/**
 * Adds a button to the GuiLayer if there is space.
 * @param button Pointer to the Button to be added.
 */
void GuiLayer::addButton(Button *button) {
    if (buttonCount >= 20) return;
    buttons[buttonCount] = button;
    buttonCount++;
}
 /**
 * Makes all buttons in the GuiLayer appear by removing their interaction and therefore making them rerender.
 */
void GuiLayer::appear() const {
    for (int i = 0; i < buttonCount; i++) {
        buttons[i]->removeInteraction();
    }
}
