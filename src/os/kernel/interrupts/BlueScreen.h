#ifndef __BlueScreen_include__
#define __BlueScreen_include__

#include <kernel/threads/ThreadState.h>
#include <lib/OutputStream.h>
#include "kernel/memory/MemLayout.h"

class BlueScreen : public OutputStream {

public:

    BlueScreen() = default;

    BlueScreen(const BlueScreen &other) = delete;

    BlueScreen &operator=(const BlueScreen &other) = delete;

    /**
     * Initializes the bluescreen.
     */
    void initialize();

    /**
     * Prints out the bluescreen containing debug information.
     *
     * @param frame The interrupt frame.
     */
    void print(InterruptFrame &frame);

    void flush() override;

    static void setErrorMessage(const char *message);

private:

    int x = 0;

    int y = 0;

    static const char *errorMessage;

    void show(uint16_t x, uint16_t y, char c);

    const uint8_t ROWS = 25;

    const uint8_t COLUMNS = 80;

    const uint8_t ATTRIBUTE = 0x1F;

    const char *CGA_START = reinterpret_cast<const char *>(VIRT_CGA_START);

    void puts(const char *s, uint32_t n);

    void putc(char c);
};

#endif
