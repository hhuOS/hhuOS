#ifndef __GraphicsService_include__
#define __GraphicsService_include__

#include <kernel/KernelService.h>
#include <devices/graphics/lfb/LinearFrameBuffer.h>
#include <devices/graphics/text/TextDriver.h>
#include <lib/util/RingBuffer.h>
#include <kernel/events/graphics/TextDriverChangedEvent.h>
#include <kernel/events/graphics/LfbDriverChangedEvent.h>

class GraphicsService : public KernelService {

private:
    LinearFrameBuffer *lfb = nullptr;
    TextDriver *text = nullptr;

    Util::RingBuffer<TextDriverChangedEvent> textEventBuffer;
    Util::RingBuffer<LfbDriverChangedEvent> lfbEventBuffer;

public:
    /**
     * Constructor.
     */
    GraphicsService();

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

    static const constexpr char* SERVICE_NAME = "GraphicsService";
};


#endif