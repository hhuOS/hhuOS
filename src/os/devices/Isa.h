#ifndef HHUOS_ISA_H
#define HHUOS_ISA_H

#include <kernel/IOport.h>

class Isa {

private:

    static IOport startAddressRegisters[8];
    static IOport pageAddressRegisters[8];
    static IOport countRegisters[8];
    static IOport statusRegisters[2];
    static IOport commandRegisters[2];
    static IOport requestRegisters[2];
    static IOport singleChannelMaskRegisters[2];
    static IOport modeRegisters[2];
    static IOport flipFlopResetRegisters[2];
    static IOport intermediateRegisters[2];
    static IOport masterResetRegisters[2];
    static IOport maskResetRegisters[2];
    static IOport multiChannelMaskRegisters[2];

public:

    enum TransferMode : uint8_t {
        TRANSFER_MODE_SELF_TEST = 0x00,
        TRANSFER_MODE_WRITE = 0x04,
        TRANSFER_MODE_READ = 0x08
    };

    enum DmaMode : uint8_t {
        DMA_MODE_TRANSFER_ON_DEMAND = 0x00,
        DMA_MODE_SINGLE_TRANSFER = 0x40,
        DMA_MODE_BLOCK_TRANSFER = 0x80,
        DMA_MODE_CASCADE = 0xc0
    };

    static void selectChannel(uint8_t channel);

    static void deselectChannel(uint8_t channel);

    static void setCount(uint8_t channel, uint16_t count);

    static void setMode(uint8_t channel, TransferMode transferMode, bool autoReset, bool reverseMemoryOrder,
                        DmaMode dmaMode);

    static void resetFlipFlop(uint8_t channel);

    static void resetMask(uint8_t channel);

    static void resetAll(uint8_t channel);
};

#endif
