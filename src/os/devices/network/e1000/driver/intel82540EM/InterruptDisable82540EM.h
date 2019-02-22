#ifndef HHUOS_INTERRUPTDISABLE82540EM_H
#define HHUOS_INTERRUPTDISABLE82540EM_H

#include <devices/network/e1000/interrupts/IdDefault.h>

class InterruptDisable82540EM final : public IdDefault {
public:
    /**
      * Constructor. Same as in extended class.
      */
    explicit InterruptDisable82540EM(Register *request);
    ~InterruptDisable82540EM() override = default;

private:
    /**
     * Inherited methods from IdDefault.
     * This methods are meant to be overridden and
     * implemented by this class.
     */

    void receiveSequenceError() final;
    void receivingCOrderedSets() final;
    void phyInterrupt() final;
    void generalPurposeInterrupts2(uint8_t value) final;
    void transmitDescriptorLowThresholdHit() final;
    void smallReceivePacketDetection() final;
    void clearReserved() final;
};

#endif
