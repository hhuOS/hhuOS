#include <kernel/Kernel.h>
#include <lib/graphic/Ansi.h>
#include "AnsiOutputStream.h"

AnsiOutputStream::AnsiOutputStream() {
    graphicsService = Kernel::getService<GraphicsService>();
}

void AnsiOutputStream::flush() {
    uint16_t x, y;

    bool bright = false;

    for (uint32_t i = 0; i < StringBuffer::pos; i++) {

        if (StringBuffer::buffer[i] == '\u001b') {
            isEscapeActive = true;
        }

        if (isEscapeActive && StringBuffer::buffer[i] == Ansi::ESCAPE_END) {
            char color[3] {currentEscapeCode[2], static_cast<char>(escapeCodeIndex > 3 ? currentEscapeCode[3] : '\0'), '\0'};
            auto colorCode = static_cast<uint32_t>(strtoint(color));

            if (currentEscapeCode[4] == ';') {
                bright = true;
            }

            if (colorCode == 0) {
                fgColor = Colors::TERM_WHITE;
                bgColor = Colors::TERM_BLACK;
            } else if(colorCode < 40) {
                fgColor = getColor(colorCode, bright);
            } else {
                bgColor = getColor(colorCode, bright);
            }

            isEscapeActive = false;
            escapeCodeIndex = 0;

            continue;
        }

        if (isEscapeActive) {
            currentEscapeCode[escapeCodeIndex++] = StringBuffer::buffer[i];

            continue;
        }

        TextDriver *textDriver = graphicsService->getTextDriver();

        textDriver->getpos(x, y);

        if(StringBuffer::buffer[i] == '\n') {
            graphicsService->getTextDriver()->show(x, y, ' ', fgColor, bgColor);
        }

        textDriver->putc(StringBuffer::buffer[i], fgColor, bgColor);
    }

    StringBuffer::pos = 0;
}

Color AnsiOutputStream::getColor(uint32_t colorCode, bool bright) {
    switch (colorCode) {
        case 30:
        case 40:
            return bright ? Colors::TERM_BRIGHT_BLACK : Colors::TERM_BLACK;
        case 31:
        case 41:
            return bright ? Colors::TERM_BRIGHT_RED : Colors::TERM_RED;
        case 32:
        case 42:
            return bright ? Colors::TERM_BRIGHT_GREEN : Colors::TERM_GREEN;
        case 33:
        case 43:
            return bright ? Colors::TERM_BRIGHT_YELLOW : Colors::TERM_YELLOW;
        case 34:
        case 44:
            return bright ? Colors::TERM_BRIGHT_BLUE : Colors::TERM_BLUE;
        case 35:
        case 45:
            return bright ? Colors::TERM_BRIGHT_MAGENTA : Colors::TERM_MAGENTA;
        case 36:
        case 46:
            return bright ? Colors::TERM_BRIGHT_CYAN : Colors::TERM_CYAN;
        case 37:
        case 47:
            return bright ? Colors::TERM_BRIGHT_WHITE : Colors::TERM_WHITE;
        case 0:
            return Colors::TERM_WHITE;
        default:
            if(colorCode < 40) {
                return Colors::TERM_WHITE;
            } else {
                return Colors::TERM_BLACK;
            }
    }
}
