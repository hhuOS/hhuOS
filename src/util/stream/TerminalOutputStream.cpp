#include "TerminalOutputStream.h"

namespace Util::Stream  {

TerminalOutputStream::TerminalOutputStream(Graphic::Terminal &terminal) : terminal(terminal) {

}

void TerminalOutputStream::write(uint8_t c) {
    terminal.putChar(c);
}

}