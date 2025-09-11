#include "Label.h"

#include "graphic/widget/Theme.h"

namespace Util::Graphic {

Label::Label(const String &text, const size_t maxWidth, const Font &font) : maxWidth(maxWidth), font(font) {
    calculateLines(text);
}

void Label::setText(const String &text) {
    const auto oldWidth = getWidth();
    const auto oldHeight = getHeight();

    calculateLines(text);
    requireRedraw();

    if (oldWidth != getWidth() || oldHeight != getHeight()) {
        reportSizeChange();
    }
}

String Label::getText() const {
    String text;
    for (const auto &line : lines) {
        text += line + "\n";
    }

    return text;
}

size_t Label::getWidth() const {
    size_t width = 0;
    for (const auto &line : lines) {
        const auto lineWidth = line.length() * font.getCharWidth();
        if (lineWidth > width) {
            width = lineWidth;
        }
    }

    return width;
}

size_t Label::getHeight() const {
    return lines.length() * font.getCharHeight() + 2 * PADDING_Y;
}

void Label::draw(const LinearFrameBuffer &lfb) {
    const auto &style = Theme::CURRENT_THEME.label().getStyle(*this);
    
    const auto startX = getPosX() + PADDING_X;
    auto currentYLine = getPosY();

    for (const auto &line : lines) {
        lfb.drawString(font, startX, currentYLine, static_cast<const char*>(line),
            style.textColor, style.textBackgroundColor);

        currentYLine += font.getCharHeight();
    }

    Widget::draw(lfb);
}

void Label::calculateLines(const String &text) {
    ArrayList<String> lines;
    const auto charWidth = font.getCharWidth();
    const auto innerWidth = maxWidth - 2 * PADDING_X;
    const auto maxChars = innerWidth >= charWidth ? innerWidth / charWidth : 1;

    const auto *lineStart = static_cast<const char*>(text);
    while (*lineStart) {
        // Calculate line
        const char *segmentEnd = lineStart;
        size_t charCount = 0;
        while (charCount < maxChars && *segmentEnd != '\0' && *segmentEnd != '\n') {
            segmentEnd++;
            charCount++;
        }

        if (*segmentEnd != '\0' && *segmentEnd != '\n' && charCount == maxChars) {
            const char *lastSpace = segmentEnd;
            while (lastSpace > lineStart && *lastSpace != ' ') {
                lastSpace--;
            }

            if (lastSpace > lineStart)
                segmentEnd = lastSpace;
        }

        // Add current line
        const auto segmentLength = segmentEnd - lineStart;
        const auto segment = String(reinterpret_cast<const uint8_t*>(lineStart), segmentLength);
        lines.add(segment);

        // Set start point for next line
        lineStart = segmentEnd;
        while (*lineStart == ' ' || *lineStart == '\n') {
            lineStart++;
        }
    }

    Label::lines = lines.toArray();
}

}
