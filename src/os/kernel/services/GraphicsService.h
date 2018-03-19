#ifndef __GraphicsService_include__
#define __GraphicsService_include__

#include <kernel/KernelService.h>
#include <devices/graphics/lfb/LinearFrameBuffer.h>
#include <devices/graphics/text/TextDriver.h>

class GraphicsService : public KernelService {

private:
    LinearFrameBuffer *lfb;
    TextDriver *text;

public:
    /**
     * Constructor.
     */
    GraphicsService() = default;

    /**
     * Copy-constructor.
     */
    GraphicsService(const GraphicsService &copy) = delete;

    /**
     * Destructor.
     */
    ~GraphicsService() = default;

    /**
     * Get current lfb.
     */
    LinearFrameBuffer *getLinearFrameBuffer();

    /**
     * Set current lfb.
     */
    void setLinearFrameBuffer(LinearFrameBuffer *lfb);

    /**
     * Get current text driver.
     */
    TextDriver *getTextDriver();

    /**
     * Set current text driver.
     */
    void setTextDriver(TextDriver *text);

    static constexpr char* SERVICE_NAME = "GraphicsService";
};


#endif