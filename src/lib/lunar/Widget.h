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

#ifndef HHUOS_LIB_LUNAR_WIDGET_H
#define HHUOS_LIB_LUNAR_WIDGET_H

#include <stddef.h>

#include "lunar/ActionListener.h"
#include "util/collection/ArrayList.h"
#include "util/graphic/LinearFrameBuffer.h"

namespace Lunar {
class Container;
} // namespace Lunar

namespace Lunar {

/// Base class for all widgets.
/// A widget is UI element that can be drawn on the screen and can interact with user input.
/// Examples for widgets are buttons, labels, text fields, etc.
/// Each widget can have action listeners that are notified when certain events occur,
/// such as mouse clicks or key presses.
/// Widgets are positioned and sized automatically by layouts within containers.
/// A `Container` is a special type of widget that can contain other widgets. This allows
/// for complex UI hierarchies, resulting in a tree structure of widgets.
class Widget {

public:
    /// Create a new widget instance.
    /// The parameter `redrawOnMouseStatusChange` specifies whether the widget should be redrawn
    /// when the mouse enters or exits the widget, or when a mouse button is pressed or released
    /// while the mouse is over the widget. Some widgets may want to change their appearance
    /// in these cases (e.g. buttons) and thus need to be redrawn.
    /// The parameter `redrawOnFocusChange` specifies whether the widget should be redrawn
    /// when the widget gains or loses keyboard focus. Some widgets may want to change their appearance
    /// in these cases (e.g. text fields) and thus need to be redrawn.
    explicit Widget(bool redrawOnMouseStatusChange = true, bool redrawOnFocusChange = true);

    /// The widget class is not copyable, as it manages action listeners on the heap.
    /// Thus, the copy constructor is deleted.
    Widget(const Widget &other) = delete;

    /// The widget class is not copyable, as it manages action listeners on the heap.
    /// Thus, the copy assignment operator is deleted.
    Widget& operator=(const Widget &other) = delete;

    /// Destroy the widget instance, deleting all registered action listeners.
    virtual ~Widget();

    /// Get the X position of the widget in absolute pixel coordinates.
    size_t getPosX() const {
        return posX;
    }

    /// Get the Y position of the widget in absolute pixel coordinates.
    size_t getPosY() const {
        return posY;
    }

    /// Get the width of the widget in pixels.
    size_t getWidth() const {
        return width;
    }

    /// Get the height of the widget in pixels.
    size_t getHeight() const {
        return height;
    }

    /// Check whether the given point (px, py) in absolute pixel coordinates
    /// is contained within the widget's area.
    bool containsPoint(const size_t px, const size_t py) const {
        const auto x = getPosX();
        const auto y = getPosY();
        const auto w = getWidth();
        const auto h = getHeight();

        return px >= x && px < x + w && py >= y && py < y + h;
    }

    /// Check whether the mouse is currently hovering over the widget.
    bool isHovered() const {
        return hovered;
    }

    /// Check whether a mouse button is currently pressed while the mouse is over the widget.
    bool isPressed() const {
        return pressed;
    }

    /// Check whether the widget currently has keyboard focus.
    bool isFocused() const {
        return focused;
    }

    /// Set whether the widget has keyboard focus.
    void setFocused(bool focused);

    /// Add an action listener to the widget.
    /// The listener will be notified when certain events occur on the widget (e.g. mouse clicks, key presses).
    void addActionListener(ActionListener *listener) {
        actionListeners.add(listener);
    }

    /// Notify the widget that it has been clicked with the mouse.
    /// This calls `onMouseClicked()` on all registered action listeners.
    /// A mouse click is defined as a mouse press followed by a mouse release while the mouse is over the widget.
    void mouseClicked() const;

    /// Notify the widget that a mouse button has been pressed while the mouse is over the widget.
    /// This calls `onMousePressed()` on all registered action listeners.
    void mousePressed() const;

    /// Notify the widget that a mouse button has been released while the mouse is over the widget.
    /// This calls `onMouseReleased()` on all registered action listeners.
    void mouseReleased() const;

    /// Notify the widget that the mouse has entered the widget's area.
    /// This calls `onMouseEntered()` on all registered action listeners.
    void mouseEntered() const;

    /// Notify the widget that the mouse has exited the widget's area.
    /// This calls `onMouseExited()` on all registered action listeners.
    void mouseExited() const;

    /// Notify the widget that a key has been pressed while the widget has keyboard focus.
    /// This calls `onKeyPressed()` on all registered action listeners.
    void keyPressed(const Util::Io::Key &key) const;

    /// Notify the widget that a key has been released while the widget has keyboard focus.
    /// This calls `onKeyReleased()` on all registered action listeners.
    void keyReleased(const Util::Io::Key &key) const;

    /// Notify the widget that a key has been typed while the widget has keyboard focus.
    /// This calls `onKeyTyped()` on all registered action listeners.
    /// A key is considered typed when it is pressed and then released.
    void keyTyped(const Util::Io::Key &key) const;

    /// Check whether the widget needs to be redrawn.
    virtual bool requiresRedraw() const {
        return needsRedraw;
    }

    /// Get the preferred width of the widget in pixels (i.e. the width that the widget would like to have).
    virtual size_t getPreferredWidth() const = 0;

    /// Get the preferred height of the widget in pixels (i.e. the height that the widget would like to have).
    virtual size_t getPreferredHeight() const = 0;

    /// Get the child widget at the given point (posX, posY) in absolute pixel coordinates.
    /// The default implementation returns `this` if the point is within the widget's area,
    /// and `nullptr` otherwise.
    /// The `Container` class overrides this method to return the appropriate child widget.
    virtual Widget* getChildAtPoint(size_t posX, size_t posY);

    /// Set the size of the widget.
    /// Implementations should override this method to react to size changes and must
    /// adhere strictly to size changes if the new size is smaller than the preferred size of the widget.
    /// However, if the new size is larger than the preferred size of the widget,
    /// the implementation may choose to clamp the size to the preferred size.
    virtual void setSize(size_t width, size_t height);

    /// Draw the widget on the given linear frame buffer.
    /// Implementations should override this method to perform custom drawing, making sure
    /// to call `Widget::draw()` to reset the `needsRedraw` flag.
    virtual void draw(const Util::Graphic::LinearFrameBuffer&) {
        needsRedraw = false;
    }

protected:

    /// Notify the widget that it needs to be redrawn.
    void requireRedraw() {
        needsRedraw = true;
    }

    /// Notify the parent container that the preferred size of the widget has changed.
    /// This can for example happen when the content of the widget changes (e.g. text in a label).
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

        explicit MouseListener(Widget &widget) : widget(widget) {}

        void onMouseEntered() override;

        void onMouseExited() override;

        void onMousePressed() override;

        void onMouseReleased() override;

    private:

        Widget &widget;
    };

    void setPosition(size_t x, size_t y);

    virtual void rearrangeChildren() {}

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

    Util::ArrayList<ActionListener*> actionListeners;
};

}

#endif