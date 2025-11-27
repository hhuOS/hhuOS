#include "Label.h"

#include "lunar/Theme.h"

namespace Lunar {

Label::Label(const Util::String &text, const size_t maxWidth, const Util::Graphic::Font &font) :
    text(text), maxWidth(maxWidth), font(font), preferredLines(calculateLines(text, maxWidth, font)) {}

Label::Label(const Util::String &text, const Util::Graphic::Font &font) : Label(text, 0, font) {}

void Label::setText(const Util::String &text) {
    const auto oldPreferredWidth = getPreferredWidth();
    const auto oldPreferredHeight = getPreferredHeight();

    Label::text = text;
    lines = calculateLines(text, getWidth(), font);
    preferredLines = calculateLines(text, maxWidth, font);
    requireRedraw();

    if (oldPreferredWidth != getPreferredWidth() || oldPreferredHeight != getPreferredHeight()) {
        reportPreferredSizeChange();
    }
}

Util::String Label::getText() const {
    return text;
}

size_t Label::getPreferredWidth() const {
    size_t width = 0;
    for (const auto &line : preferredLines) {
        const auto lineWidth = line.length() * font.getCharWidth();
        if (lineWidth > width) {
            width = lineWidth;
        }
    }

    return width;
}

size_t Label::getPreferredHeight() const {
    return preferredLines.length() * font.getCharHeight();
}

void Label::setSize(size_t width, size_t height) {
    if (getPreferredWidth() == 0) {
        lines = calculateLines(text, width, font);
    }

    const auto preferredWidth = getPreferredWidth();
    const auto preferredHeight = getPreferredHeight();

    if (width > preferredWidth) {
        width = preferredWidth;
    }
    if (height > preferredHeight) {
        height = preferredHeight;
    }

    if (width != getWidth() || height != getHeight()) {
        lines = calculateLines(text, width, font);
        Widget::setSize(width, height);
    }
}

void Label::draw(const Util::Graphic::LinearFrameBuffer &lfb) {
    const auto &style = Theme::CURRENT_THEME.label().getStyle(*this);

    const auto posX = getPosX();
    const auto posY = getPosY();
    const auto width = getWidth();
    const auto height = getHeight();

    // Not enough space to draw text
    if (width < font.getCharWidth()) {
        Widget::draw(lfb);
        return;
    }

    auto linePosition = posY;
    for (const auto &line : lines) {
        if (linePosition + font.getCharHeight() > posY + height) {
            break;
        }

        lfb.drawString(font, posX, linePosition, static_cast<const char*>(line),
            style.textColor, style.textBackgroundColor);

        linePosition += font.getCharHeight();
    }

    Widget::draw(lfb);
}

Util::Array<Util::String> Label::calculateLines(const Util::String &text, const size_t maxWidth, const Util::Graphic::Font &font) {
    Util::ArrayList<Util::String> lines;
    const auto charWidth = font.getCharWidth();
    const auto maxChars = maxWidth >= charWidth ? maxWidth / charWidth : 1;

    const auto *lineStart = static_cast<const char*>(text);
    while (*lineStart) {
        // Calculate line
        const char *segmentEnd = lineStart;
        size_t charCount = 0;
        while ((charCount < maxChars || maxWidth == 0) && *segmentEnd != '\0' && *segmentEnd != '\n') {
            segmentEnd++;
            charCount++;
        }

        if (*segmentEnd != '\0' && *segmentEnd != '\n' && charCount == maxChars) {
            const char *lastSpace = segmentEnd;
            while (lastSpace > lineStart && *lastSpace != ' ') {
                lastSpace--;
            }

            if (lastSpace > lineStart) {
                segmentEnd = lastSpace;
            }
        }

        // Add current line
        const auto segmentLength = segmentEnd - lineStart;
        const auto segment = Util::String(reinterpret_cast<const uint8_t*>(lineStart), segmentLength);
        lines.add(segment);

        // Set start point for next line
        lineStart = segmentEnd;
        while (*lineStart == ' ' || *lineStart == '\n') {
            lineStart++;
        }
    }

    return lines.toArray();
}

}
