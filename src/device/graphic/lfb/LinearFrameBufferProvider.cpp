#include "lib/util/data/ArrayList.h"
#include "LinearFrameBufferProvider.h"
#include "LinearFrameBufferNode.h"
#include "kernel/service/FilesystemService.h"
#include "kernel/system/System.h"
#include "filesystem/core/Filesystem.h"
#include "filesystem/memory/MemoryDriver.h"
#include "lib/util/Exception.h"
#include "lib/util/data/Collection.h"
#include "lib/util/data/Iterator.h"

namespace Device::Graphic {

LinearFrameBufferProvider::ModeInfo LinearFrameBufferProvider::searchMode(uint16_t resolutionX, uint16_t resolutionY, uint8_t colorDepth) const {
    auto modes = getAvailableModes();
    auto candidates = Util::Data::ArrayList<ModeInfo>();

    if (modes.length() == 0) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "LinearFrameBufferProvider: No graphics mode available!");
    }

    uint32_t bestDiff = 0xffffffff;
    ModeInfo bestMode{};

    // Find the closest vertical resolution;
    for (const ModeInfo &mode : modes) {
        uint32_t diff = mode.resolutionY > resolutionY ? mode.resolutionY - resolutionY : resolutionY - mode.resolutionY;

        if (diff < bestDiff) {
            bestDiff = diff;
            bestMode = mode;
        }
    }

    // Mark all modes with the closest vertical resolution as candidates
    for (const ModeInfo &mode : modes) {
        if (mode.resolutionY == bestMode.resolutionY) {
            candidates.add(mode);
        }
    }

    bestDiff = 0xffffffff;

    // Search candidates for the closest horizontal resolution
    for (const ModeInfo &mode : candidates) {
        uint32_t diff = mode.resolutionX > resolutionX ? mode.resolutionX - resolutionX : resolutionX - mode.resolutionX;

        if (diff < bestDiff) {
            bestDiff = diff;
            bestMode = mode;
        }
    }

    // Remove all candidates, that don't have the closest horizontal resolution
    auto removeList = Util::Data::ArrayList<ModeInfo>();
    for (const ModeInfo &mode : candidates) {
        if (mode.resolutionX != bestMode.resolutionX) {
            removeList.remove(mode);
        }
    }
    candidates.removeAll(removeList);

    bestDiff = 0xffffffff;

    // Find the mode with closest color depth
    for (const ModeInfo &mode : candidates) {
        uint32_t diff = mode.colorDepth > colorDepth ? mode.colorDepth - colorDepth : colorDepth - mode.colorDepth;

        if (diff < bestDiff) {
            bestDiff = diff;
            bestMode = mode;
        }
    }

    return bestMode;
}

void LinearFrameBufferProvider::initializeLinearFrameBuffer(const LinearFrameBufferProvider::ModeInfo &modeInfo, const Util::Memory::String &filename) {
    auto *lfb = initializeLinearFrameBuffer(modeInfo);

    // Create filesystem node
    auto &filesystem = Kernel::System::getService<Kernel::FilesystemService>().getFilesystem();
    auto &driver = filesystem.getVirtualDriver("/device");
    auto *lfbNode = new LinearFrameBufferNode(filename, lfb);

    if (!driver.addNode("/", lfbNode)) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "LinearFrameBufferProvider: Failed to add node!");
    }
}

}