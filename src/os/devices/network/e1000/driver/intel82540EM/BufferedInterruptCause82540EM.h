#ifndef HHUOS_BUFFEREDINTERRUPTCAUSE82540EM_H
#define HHUOS_BUFFEREDINTERRUPTCAUSE82540EM_H

#include <devices/network/e1000/interrupts/IcBufferedDefault.h>

class BufferedInterruptCause82540EM final : public IcBufferedDefault {
public:
    /**
      * Constructor. Same as in extended class.
      */
    explicit BufferedInterruptCause82540EM(uint32_t *address);
    ~BufferedInterruptCause82540EM() override = default;

private:
    /**
      * Inherited methods from IcBufferedDefault.
      * This methods are meant to be overridden and
      * implemented by this class.
      */

    uint8_t generalPurposeInterrupts_1() final;
    bool hasReceiveSequenceError() final;
    bool hasReceivingCOrderedSets() final;
    bool isPhyInterrupt() final;
    bool isGeneralPurposeInterruptOnSDP6_2() final;
    bool isGeneralPurposeInterruptOnSDP7_3() final;
    bool isTransmitDescriptorLowThresholdHit() final;
    bool hasSmallReceivePacketDetected() final;
};

#endif
