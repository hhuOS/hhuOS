/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 * The widget and layout system is based on a bachelor's thesis, written by Michael Zuchniewski.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-mizuc100
 */

#ifndef HHUOS_LIB_UTIL_GRAPHIC_WIDGET_WIDGET_H
#define HHUOS_LIB_UTIL_GRAPHIC_WIDGET_WIDGET_H

#include <stddef.h>

#include "ActionListener.h"
#include "collection/ArrayList.h"
#include "graphic/LinearFrameBuffer.h"

namespace Util::Graphic {

    class Container;

class Widget {

public:

    explicit Widget(bool redrawOnMouseStatusChange = true, bool redrawOnFocusChange = true);

    Widget(const Widget &other) = delete;

    Widget& operator=(const Widget &other) = delete;

    virtual ~Widget();

    [[nodiscard]] size_t getPosX() const;

    [[nodiscard]] size_t getPosY() const;

    [[nodiscard]] size_t getWidth() const;

    [[nodiscard]] size_t getHeight() const;

    [[nodiscard]] bool containsPoint(size_t px, size_t py) const;

    [[nodiscard]] bool isHovered() const;

    [[nodiscard]] bool isPressed() const;

    [[nodiscard]] bool isFocused() const;

    void setFocused(bool focused);

    void addActionListener(ActionListener *listener);

    void mouseClicked() const;

    void mousePressed() const;

    void mouseReleased() const;

    void mouseEntered() const;

    void mouseExited() const;

    void keyPressed(const Io::Key &key) const;

    void keyReleased(const Io::Key &key) const;

    void keyTyped(const Io::Key &key) const;

    virtual void setSize(size_t width, size_t height);

    [[nodiscard]] virtual bool requiresRedraw() const;

    [[nodiscard]] virtual size_t getPreferredWidth() const = 0;

    [[nodiscard]] virtual size_t getPreferredHeight() const = 0;

    [[nodiscard]] virtual Widget* getChildAtPoint(size_t posX, size_t posY);

    virtual void draw(const LinearFrameBuffer &lfb);

protected:

    void requireRedraw();

    void reportPreferredSizeChange() const;

private:

    friend class Container;
    friend class FreeLayout;
    friend class VerticalLayout;
    friend class HorizontalLayout;
    friend class GridLayout;
    friend class BorderLayout;

    class MouseListener final : public ActionListener {

    public:

        explicit MouseListener(Widget &widget);

        void onMouseEntered() override;

        void onMouseExited() override;

        void onMousePressed() override;

        void onMouseReleased() override;

    private:

        Widget &widget;
    };

    void setPosition(size_t x, size_t y);

    virtual void rearrangeChildren();

    size_t posX = 0;
    size_t posY = 0;

    size_t width = 0;
    size_t height = 0;

    bool hovered = false;
    bool pressed = false;
    bool focused = false;

    const bool redrawOnMouseStatusChange;
    const bool redrawOnFocusChange;

    bool needsRedraw = true;

    Container *parent = nullptr;

    ArrayList<ActionListener*> actionListeners;
};

}

#endif