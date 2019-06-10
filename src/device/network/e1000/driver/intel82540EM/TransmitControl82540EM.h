#ifndef HHUOS_TRANSMITCONTROL82540EM_H
#define HHUOS_TRANSMITCONTROL82540EM_H

#include "device/network/e1000/transmit/TcDefault.h"

class TransmitControl82540EM final : public TcDefault {
public:
    /**
      * Constructor. Same as in extended class.
      */
    explicit TransmitControl82540EM(Register *request);
    ~TransmitControl82540EM() override = default;

private:
    /**
     * Inherited method from TcDefault.
     * This method is meant to be overridden and
     * implemented by this class.
     */

    void noRetransmitOnUnderrun(bool enable) final ;
};

#endif
