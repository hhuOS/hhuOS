#include "Label.h"

namespace Util::Graphic {
Label::Label(const String &text, const size_t maxWidth, const Font &font) :
    text(text), maxWidth(maxWidth), font(font) {}

Label::Label(const String &text, const size_t maxWidth, const Font &font,
    const Color &textColor, const Color &backgroundColor) : Label(text, maxWidth, font)
{
    style.textColor = textColor;
    style.backgroundColor = backgroundColor;
}

void Label::setText(const String &text) {
    Label::text = text;
    requireParentRedraw();
}

size_t Label::getWidth() const {
    return maxWidth;
}

size_t Label::getHeight() const {
    const auto charsPerLine = maxWidth / font.getCharWidth();
    const auto totalChars = text.length();
    const auto lines = (totalChars + charsPerLine - 1) / charsPerLine;

    return lines * font.getCharHeight() + 2 * style.paddingY;
}

void Label::draw(const LinearFrameBuffer &lfb) {
    const auto charWidth = font.getCharWidth();
    const auto lineHeight = font.getCharHeight();
    const auto innerWidth = maxWidth - 2 * style.paddingX;
    const auto maxChars = innerWidth >= charWidth ? innerWidth / charWidth : 1;

    const auto *lineStart = static_cast<const char*>(text);
    const auto startX = getPosX() + style.paddingX;
    auto currentYLine = getPosY();

    while (*lineStart) {
        // Calculate string segment
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

        // Draw string segment
        const auto segmentLength = segmentEnd - lineStart;
        const auto segment = String(reinterpret_cast<const uint8_t*>(lineStart), segmentLength);

        lfb.drawString(font, startX, currentYLine, static_cast<const char*>(segment),
            style.textColor, style.backgroundColor);
        currentYLine += lineHeight;

        // Set start point for next segment
        lineStart = segmentEnd;
        while (*lineStart == ' ' || *lineStart == '\n') {
            lineStart++;
        }
    }

    Widget::draw(lfb);
}

}
