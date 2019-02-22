#ifndef HHUOS_INTERRUPTENABLE82540EM_H
#define HHUOS_INTERRUPTENABLE82540EM_H

#include <devices/network/e1000/interrupts/IeDefault.h>

class InterruptEnable82540EM final : public IeDefault {
public:
    /**
      * Constructor. Same as in extended class.
      */
    explicit InterruptEnable82540EM(Register *request);
    ~InterruptEnable82540EM() override = default;

private:
    /**
     * Inherited methods from IeDefault.
     * This methods are meant to be overridden and
     * implemented by this class.
     */

    void receiveSequenceError() final;
    void receivingCOrderedSets() final;
    void phyInterrupt() final;
    void generalPurposeInterrupts2(uint8_t value) final;
    void transmitDescriptorLowThresholdHit() final;
    void smallReceivePacketDetection() final;
};

#endif
