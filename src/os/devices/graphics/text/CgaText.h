#ifndef __CgaText_include__
#define __CgaText_include__

#include "kernel/IOport.h"
#include "kernel/memory/MemLayout.h"
#include "TextDriver.h"
#include "lib/Colors.h"

#include <cstdint>

/**
 * Implementation of TextDriver for graphics cards, that are compatible with the CGA-standard.
 */
class CgaText : public TextDriver {

private:
    const char *CGA_START = reinterpret_cast<const char *>(VIRT_CGA_START);

    IOport index_port;
    IOport data_port;

    uint16_t pos = 0;

    String vendorName = String("Unknown");
    String deviceName = String();
    uint32_t videoMemorySize = 0;

    Util::ArrayList<TextResolution> resolutions;

    /**
     * Overriding virtual function from TextDriver.
     */
    bool setResolution(TextResolution resolution) override;

    /**
     * Set the CGA-device to a given mode.
     *
     * @param modeNumber The mode
     */
    void setMode(uint16_t modeNumber);

public:
    /**
     * Constuctor.
     */
    CgaText();

    /**
     * Copy-constructor.
     */
    CgaText(const CgaText &copy) = delete;

    /**
     * Destructor.
     */
    ~CgaText() override = default;

    /**
     * Overriding virtual function from TextDriver.
     */
    bool isAvailable() override;

    /**
     * Overriding virtual function from TextDriver.
     */
    Util::Array<TextResolution> getTextResolutions() override;

    /**
     * Overriding virtual function from TextDriver.
     */
    String getVendorName() override;

    /**
     * Overriding virtual function from TextDriver.
     */
    String getDeviceName() override;

    /**
     * Overriding virtual function from TextDriver.
     */
    uint32_t getVideoMemorySize() override;

    /**
     * Overriding virtual function from TextDriver.
     */
    void setpos (uint16_t x, uint16_t y) override;

    /**
     * Overriding virtual function from TextDriver.
     */
    void getpos (uint16_t& x, uint16_t& y) override;

    /**
     * Overriding virtual function from TextDriver.
     */
    void show(uint16_t x, uint16_t y, char c, Color fgColor = Colors::LIGHT_GRAY, Color bgColor = Colors::INVISIBLE) override;

    /**
     * Overriding virtual function from TextDriver.
     */
    void putc(char c, Color fgColor = Colors::LIGHT_GRAY, Color bgColor = Colors::INVISIBLE) override;

    /**
     * Overriding virtual function from TextDriver.
     */
    void puts(const char *s, uint32_t n, Color fgColor = Colors::LIGHT_GRAY, Color bgColor = Colors::INVISIBLE) override;

    /**
     * Overriding virtual function from TextDriver.
     */
    void scrollup() override;

    /**
     * Overriding virtual function from TextDriver.
     */
    void clear() override;
};

#endif

