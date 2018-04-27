#ifndef __GraphicsDeviceNameNode_include__
#define __GraphicsDeviceNameNode_include__


#include <filesystem/RamFs/VirtualNode.h>
#include <kernel/services/GraphicsService.h>

/**
 * Implementation of VirtualNode, that read the currently used graphics card's name.
 *
 * @author Fabian Ruhland
 * @date 2018
 */
class GraphicsDeviceNameNode : public VirtualNode {

private:
    GraphicsService *graphicsService = nullptr;

    uint8_t mode;

public:

    /**
     * Possible graphics modes.
     *
     * @param mode TEXT: Use the current TextDriver.
     *             LINEAR_FRAME_BUFFER: Use the current LinearFrameBuffer.
     */
    enum MODES {
        TEXT = 0x00,
        LINEAR_FRAME_BUFFER = 0x01
    };

    /**
     * Constructor.
     */
    explicit GraphicsDeviceNameNode(uint8_t mode);

    /**
     * Copy-Constructor.
     */
    GraphicsDeviceNameNode(const GraphicsDeviceNameNode &copy) = delete;

    /**
     * Destructor.
     */
    ~GraphicsDeviceNameNode() override = default;

    /**
     * Overriding function from VirtualNode.
     */
    uint64_t getLength() override;

    /**
     * Overriding function from VirtualNode.
     */
    uint64_t readData(char *buf, uint64_t pos, uint64_t numBytes) override;

    /**
     * Overriding function from VirtualNode.
     */
    uint64_t writeData(char *buf, uint64_t pos, uint64_t numBytes) override;
};


#endif
