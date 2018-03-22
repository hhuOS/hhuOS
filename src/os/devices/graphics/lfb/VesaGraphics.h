#ifndef __VesaGraphics_include__
#define __VesaGraphics_include__

#include "LinearFrameBuffer.h"
#include "kernel/Kernel.h"
#include <cstdint>
#include "kernel/memory/manager/IOMemoryManager.h"

/**
 * Implementation of LinearFrameBuffer for graphics cards, that are compatible with the VBE-standard.
 * Compatible devices need to support at least VBE 2.0.
 */
class VesaGraphics : public LinearFrameBuffer {

public:
	/**
	 * Information about a VBE device.
	 * See http://wiki.osdev.org/VESA_Video_Modes for further reference.
	 */
    struct VbeInfo {
		char 	    signature[4] = {'V', 'B', 'E', '2'};
		uint16_t 	version{};			// VBE version; high byte is major version, low byte is minor version
		uint32_t 	oem{};				// segment:offset pointer to OEM
		uint32_t 	capabilities{};		// bitfield that describes card capabilities
		uint16_t 	video_modes[2]{};	// segment:offset pointer to list of supported video modes
		uint16_t 	video_memory{};		// amount of video memory in 64KB blocks
		uint16_t 	software_rev{};		// software revision
		uint16_t 	vendor[2]{};		// segment:offset to card vendor string
		uint16_t 	product_name[2]{};	// segment:offset to card model name
		uint16_t 	product_rev[2]{};	// segment:offset pointer to product revision
		char reserved[222]{};			// reserved for future expansion
		char oem_data[256]{};			// OEM BIOSes store their strings in this area
	} __attribute__((packed)) ;

    /**
     * Information about a VBE graphics mode.
     * See http://wiki.osdev.org/VESA_Video_Modes for further reference.
     */
	struct ModeInfo {
        uint16_t  attributes;
        uint8_t   winA,winB;
        uint16_t  granularity;
        uint16_t  winsize;
        uint16_t  segmentA, segmentB;
        uint16_t  realFctPtr[2];
        uint16_t  pitch;          // bytes per Scanline

        uint16_t  Xres, Yres;
        uint8_t   Wchar, Ychar, planes, bpp, banks;
        uint8_t   memory_model, bank_size, image_pages;
        uint8_t   reserved0;

        uint8_t   red_mask, red_position;
        uint8_t   green_mask, green_position;
        uint8_t   blue_mask, blue_position;
        uint8_t   rsv_mask, rsv_position;
        uint8_t   directcolor_attributes;

        uint32_t  physbase;       // address of the linear framebuffer
        uint32_t  reserved1;
        uint16_t  reserved2;

		bool operator!=(const ModeInfo &other) const;

    } __attribute__((packed));
	
	struct PaletteEntry {
		uint8_t blue;
		uint8_t green;
		uint8_t red;
		uint8_t alignment;       // unused;
	} __attribute__((packed));

private:

    static const uint16_t INVALID_MODE = 0xFFFF;

	String vendorName = String();
	String deviceName = String();
	uint32_t videoMemorySize = 0;

	Util::ArrayList<LinearFrameBuffer::LfbResolution> resolutions;

private:
	/**
     * Overriding virtual function from LinearFrameBuffer.
     */
	bool setResolution(LinearFrameBuffer::LfbResolution resolution) override;

	/**
	 * Set the VBE-device to a given mode.
	 *
	 * @param mode The mode
	 * @return true, on success
	 */
    bool setMode(uint16_t mode);

    /**
     * Get information about the VBE device from the BIOS.
     */
    VbeInfo* getVbeInfo();

    /**
     * Get information about a specific VBE graphics mode from the BIOS.
     * @param mode The mode
     */
    ModeInfo* getModeInfo(uint16_t mode);

	/**
     * Reallocate the buffer, that is used for double-buffering.
     */
	void reallocBuffer();

private:
    bool isDoubleBuffered = false;
	uint8_t *doubleBuffer = nullptr;

    IOMemInfo lfbMemInfo = (IOMemInfo) {0, 0, nullptr};

protected:
    uint8_t *hardwareBuffer = nullptr;
    uint16_t pitch = 0;

public:
    /**
     * Constructor.
     */
    VesaGraphics();

    /**
     * Copy-constructor.
     */
    VesaGraphics(const VesaGraphics &copy) = delete;

    /**
     * Destructor.
     */
    ~VesaGraphics() override = default;

    /**
     * Overriding virtual function from LinearFrameBuffer.
     */
    bool isAvailable() override;

    /**
     * Overriding virtual function from LinearFrameBuffer.
     */
    Util::ArrayList<LinearFrameBuffer::LfbResolution>& getLfbResolutions() override;

    /**
     * Overriding virtual function from LinearFrameBuffer.
     */
	String getVendorName() override;

    /**
     * Overriding virtual function from LinearFrameBuffer.
     */
	String getDeviceName() override;

    /**
     * Overriding virtual function from LinearFrameBuffer.
     */
	uint32_t getVideoMemorySize() override;

    /**
     * Overriding virtual function from LinearFrameBuffer.
     */
    void drawPixel(uint16_t x, uint16_t y, Color color) override;

    /**
     * Overriding virtual function from LinearFrameBuffer.
     */
    void readPixel(uint16_t x, uint16_t y, Color &color) override;

    /**
     * Overriding virtual function from LinearFrameBuffer.
     */
    void clear() override;

	/**
     * Overriding virtual function from LinearFrameBuffer.
     */
	void enableDoubleBuffering() override;

	/**
     * Overriding virtual function from LinearFrameBuffer.
     */
	void disableDoubleBuffering() override;

    /**
     * Overriding virtual function from LinearFrameBuffer.
     */
    void show() override;
};

#endif
