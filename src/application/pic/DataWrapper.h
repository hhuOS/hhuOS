//
// Created by Rafael Reip on 04.10.24.
//

#ifndef HHUOS_DATAWRAPPER_H
#define HHUOS_DATAWRAPPER_H


class MouseInfo {
public:
    MouseInfo() : X(0), Y(0), leftButtonPressed(false) {}

    int X, Y;
    bool leftButtonPressed;
};


#endif // HHUOS_DATAWRAPPER_H
