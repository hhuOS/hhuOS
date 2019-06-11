#ifndef HHUOS_ANSIOUTPUTSTREAM_H
#define HHUOS_ANSIOUTPUTSTREAM_H

#include "kernel/service/GraphicsService.h"
#include "OutputStream.h"

class AnsiOutputStream : public OutputStream {

public:

    AnsiOutputStream();

    AnsiOutputStream(const AnsiOutputStream &copy) = delete;

    AnsiOutputStream& operator=(const AnsiOutputStream &copy) = delete;

    ~AnsiOutputStream() override = default;

    void flush() override;

private:

    Color getColor(uint32_t colorCode, bool bright);

private:

    Kernel::GraphicsService *graphicsService = nullptr;

    Color fgColor = Colors::TERM_WHITE;

    Color bgColor = Colors::TERM_BLACK;

    bool isEscapeActive = false;

    char currentEscapeCode[16];

    uint8_t escapeCodeIndex = 0;
};

#endif
