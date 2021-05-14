#include "Terminal.h"

Util::Graphic::Terminal::Terminal(uint16_t columns, uint16_t rows) : columns(columns), rows(rows) {

}

void Util::Graphic::Terminal::putString(const char *string) {
    for (uint32_t i = 0; string[i] != 0; i++) {
        putChar(string[i]);
    }
}

uint16_t Util::Graphic::Terminal::getColumns() const {
    return columns;
}

uint16_t Util::Graphic::Terminal::getRows() const {
    return rows;
}
