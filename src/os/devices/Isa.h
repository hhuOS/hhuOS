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

    static void selectChannel(uint8_t channel);

    static void deselectChannel(uint8_t channel);
};

#endif
