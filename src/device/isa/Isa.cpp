/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include "kernel/core/SystemManagement.h"
#include "kernel/memory/MemLayout.h"
#include "Isa.h"

static Spinlock isaLock;

IsaDmaMemoryManager Isa::dmaMemoryManager = IsaDmaMemoryManager();

IoPort Isa::startAddressRegisters[8] = {
        IoPort(0x00),
        IoPort(0x02),
        IoPort(0x04),
        IoPort(0x06),
        IoPort(0xc0),
        IoPort(0xc4),
        IoPort(0xc8),
        IoPort(0xcc)
};

IoPort Isa::pageAddressRegisters[8] = {
        IoPort(0x87),
        IoPort(0x83),
        IoPort(0x81),
        IoPort(0x82),
        IoPort(0x8f),
        IoPort(0x8b),
        IoPort(0x89),
        IoPort(0x8a)
};

IoPort Isa::countRegisters[8] = {
        IoPort(0x01),
        IoPort(0x03),
        IoPort(0x05),
        IoPort(0x07),
        IoPort(0xc2),
        IoPort(0xc6),
        IoPort(0xca),
        IoPort(0xce)
};

IoPort Isa::statusRegisters[2] = {
        IoPort(0x08),
        IoPort(0xd0)
};

IoPort Isa::commandRegisters[2] = {
        IoPort(0x08),
        IoPort(0xd0)
};

IoPort Isa::requestRegisters[2] = {
        IoPort(0x09),
        IoPort(0xd2)
};

IoPort Isa::singleChannelMaskRegisters[2] = {
        IoPort(0x0a),
        IoPort(0xd4)
};

IoPort Isa::modeRegisters[2] = {
        IoPort(0x0b),
        IoPort(0xd6)
};

IoPort Isa::flipFlopResetRegisters[2] = {
        IoPort(0x0c),
        IoPort(0xd8)
};

IoPort Isa::intermediateRegisters[2] = {
        IoPort(0x0d),
        IoPort(0xda)
};

IoPort Isa::masterResetRegisters[2] = {
        IoPort(0x0d),
        IoPort(0xda)
};

IoPort Isa::maskResetRegisters[2] = {
        IoPort(0x0e),
        IoPort(0xdc)
};

IoPort Isa::multiChannelMaskRegisters[2] = {
        IoPort(0x0f),
        IoPort(0xde)
};

void *Isa::allocDmaBuffer() {
    isaLock.acquire();

    void *physAddress = dmaMemoryManager.alloc(IsaDmaMemoryManager::ISA_DMA_BUF_SIZE);
    void *ret = SystemManagement::getInstance().mapIO((uint32_t) physAddress, IsaDmaMemoryManager::ISA_DMA_BUF_SIZE);

    isaLock.release();

    return ret;
}

void Isa::freeDmaBuffer(void *ptr) {
    isaLock.acquire();

    dmaMemoryManager.free(SystemManagement::getInstance().getPhysicalAddress(ptr));
    SystemManagement::getInstance().freeIO(ptr);

    isaLock.release();
}

void Isa::selectChannel(uint8_t channel) {
    if(channel > 7) {
        return;
    }

    auto mask = static_cast<uint8_t>((channel > 3u ? channel - 4u : channel) | 0x04u);

    isaLock.acquire();

    singleChannelMaskRegisters[channel / 4].outb(mask);

    isaLock.release();
}

void Isa::deselectChannel(uint8_t channel) {
    if(channel > 7) {
        return;
    }

    auto mask = static_cast<uint8_t>(channel > 3u ? channel - 4u : channel);

    isaLock.acquire();

    singleChannelMaskRegisters[channel / 4].outb(mask);

    isaLock.release();
}

void Isa::setAddress(uint8_t channel, uint32_t address) {
    if(channel > 7) {
        return;
    }

    if(address < ISA_DMA_START_ADDRESS || address > ISA_DMA_END_ADDRESS) {
        return;
    }

    isaLock.acquire();

    resetFlipFlop(channel);

    startAddressRegisters[channel].outb(static_cast<uint8_t>(address & 0x000000ffu));
    startAddressRegisters[channel].outb(static_cast<uint8_t>((address >> 8u) & 0x000000ffu));
    pageAddressRegisters[channel].outb(static_cast<uint8_t>((address >> 16u) & 0x0000000fu));

    isaLock.release();
}

void Isa::setCount(uint8_t channel, uint16_t count) {
    if(channel > 7) {
        return;
    }

    isaLock.acquire();

    resetFlipFlop(channel);

    countRegisters[channel].outb(static_cast<uint8_t>((count) & 0x00ffu));
    countRegisters[channel].outb(static_cast<uint8_t>(((count) >> 8u) & 0x00ffu));

    isaLock.release();
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

    isaLock.acquire();

    modeRegisters[channel / 4].outb(mask);

    isaLock.release();
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

    isaLock.acquire();

    maskResetRegisters[channel / 4].outb(0xff);

    isaLock.release();
}

void Isa::resetAll(uint8_t channel) {
    if(channel > 7) {
        return;
    }

    isaLock.acquire();

    masterResetRegisters[channel / 4].outb(0xff);

    isaLock.release();
}
