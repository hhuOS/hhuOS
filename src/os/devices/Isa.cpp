#include "Isa.h"

IOport Isa::startAddressRegisters[8] = {
        IOport(0x00),
        IOport(0x02),
        IOport(0x04),
        IOport(0x06),
        IOport(0xc0),
        IOport(0xc4),
        IOport(0xc8),
        IOport(0xcc)
};

IOport Isa::pageAddressRegisters[8] = {
        IOport(0x87),
        IOport(0x83),
        IOport(0x81),
        IOport(0x82),
        IOport(0x8f),
        IOport(0x8b),
        IOport(0x89),
        IOport(0x8a)
};

IOport Isa::countRegisters[8] = {
        IOport(0x01),
        IOport(0x03),
        IOport(0x05),
        IOport(0x07),
        IOport(0xc2),
        IOport(0xc6),
        IOport(0xca),
        IOport(0xce)
};

IOport Isa::statusRegisters[2] = {
        IOport(0x08),
        IOport(0xd0)
};

IOport Isa::commandRegisters[2] = {
        IOport(0x08),
        IOport(0xd0)
};

IOport Isa::requestRegisters[2] = {
        IOport(0x09),
        IOport(0xd2)
};

IOport Isa::singleChannelMaskRegisters[2] = {
        IOport(0x0a),
        IOport(0xd4)
};

IOport Isa::modeRegisters[2] = {
        IOport(0x0b),
        IOport(0xd6)
};

IOport Isa::flipFlopResetRegisters[2] = {
        IOport(0x0c),
        IOport(0xd8)
};

IOport Isa::intermediateRegisters[2] = {
        IOport(0x0d),
        IOport(0xda)
};

IOport Isa::masterResetRegisters[2] = {
        IOport(0x0d),
        IOport(0xda)
};

IOport Isa::maskResetRegisters[2] = {
        IOport(0x0e),
        IOport(0xdc)
};

IOport Isa::multiChannelMaskRegisters[2] = {
        IOport(0x0f),
        IOport(0xde)
};

void Isa::selectChannel(uint8_t channel) {
    if(channel > 7) {
        return;
    }

    auto mask = static_cast<uint8_t>((channel > 3u ? channel - 4u : channel) | 0x04u);

    singleChannelMaskRegisters[channel / 4].outb(mask);
}

void Isa::deselectChannel(uint8_t channel) {
    if(channel > 7) {
        return;
    }

    auto mask = static_cast<uint8_t>(channel > 3u ? channel - 4u : channel);

    singleChannelMaskRegisters[channel / 4].outb(mask);
}

void Isa::setCount(uint8_t channel, uint16_t count) {
    if(channel > 7) {
        return;
    }

    countRegisters[channel].outb(static_cast<uint8_t>(count & 0x0fu));
    countRegisters[channel].outb(static_cast<uint8_t>(count >> 8u));
}

void Isa::setMode(uint8_t channel, Isa::TransferMode transferMode, bool autoReset, bool reverseMemoryOrder,
                  Isa::DmaMode dmaMode) {
    if(channel > 7) {
        return;
    }

    auto mask = static_cast<uint8_t>((channel > 3u ? channel - 4u : channel));

    mask |= transferMode;

    if(autoReset) {
        mask |= 0x10;
    }

    if(reverseMemoryOrder) {
        mask |= 0x20;
    }

    mask |= dmaMode;

    modeRegisters[channel / 4].outb(mask);
}

void Isa::resetFlipFlop(uint8_t channel) {
    if(channel > 7) {
        return;
    }

    flipFlopResetRegisters[channel / 4].outb(0xff);
}

void Isa::resetMask(uint8_t channel) {
    if(channel > 7) {
        return;
    }

    maskResetRegisters[channel / 4].outb(0xff);
}

void Isa::resetAll(uint8_t channel) {
    if(channel > 7) {
        return;
    }

    masterResetRegisters[channel / 4].outb(0xff);
}
