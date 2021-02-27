#ifndef HHUOS_FONTS_H
#define HHUOS_FONTS_H

#include "TerminalFont.h"
// #include "TerminalFontSmall.h"
#include "Font.h"

namespace Util::Graphic::Fonts {

static Font TERMINAL_FONT = Font(8, 16, TERMINAL_FONT_DATA);
// static Font TERMINAL_FONT_SMALL = Font(8, 8, TERMINAL_FONT_SMALL_DATA);

}

#endif
