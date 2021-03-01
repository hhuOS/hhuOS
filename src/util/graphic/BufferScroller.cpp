#include <util/memory/Address.h>
#include "BufferScroller.h"

namespace Util::Graphic {

BufferScroller::BufferScroller(LinearFrameBuffer &lfb) : lfb(lfb) {

}

void BufferScroller::scrollUp(uint16_t lineCount) {
    // Move screen buffer upwards by the given amount of lines
    auto source = Util::Memory::Address<uint32_t>(lfb.getBuffer() + lfb.getPitch() * lineCount);
    auto destination = Util::Memory::Address<uint32_t>(lfb.getBuffer());
    destination.copyRange(source, lfb.getPitch() * (lfb.getResolutionY() - lineCount));

    // Clear lower part of the screen
    auto clear = Util::Memory::Address<uint32_t>(lfb.getBuffer() + lfb.getPitch() * (lfb.getResolutionY() - lineCount));
    clear.setRange(0, lfb.getPitch() * lineCount);
}

void BufferScroller::scrollDown(uint16_t lineCount) {
    // Move the screen buffer downwards line by line, starting at the bottom
    for (int32_t i = 1; i < lfb.getResolutionY() - lineCount; i++) {
        auto source = Util::Memory::Address<uint32_t>(lfb.getBuffer() + lfb.getPitch() * (lfb.getResolutionY() - lineCount - i));
        auto destination = Util::Memory::Address<uint32_t>(lfb.getBuffer() + lfb.getPitch() * (lfb.getResolutionY() - i - 1));
        destination.copyRange(source, lfb.getPitch());
    }

    // Clear upper part of the screen
    auto clear = Util::Memory::Address<uint32_t>(lfb.getBuffer());
    clear.setRange(0, lfb.getPitch() * lineCount);
}

}