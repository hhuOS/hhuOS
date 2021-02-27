#include "TerminalOutputStream.h"

namespace Util::Stream  {

TerminalOutputStream::TerminalOutputStream(Graphic::Terminal &terminal) : terminal(terminal) {

}

void TerminalOutputStream::write(uint8_t c) {
    terminal.putChar(c);
}

void TerminalOutputStream::write(const uint8_t *source, uint32_t offset, uint32_t length) {
    OutputStream::write(source, offset, length);
}

}