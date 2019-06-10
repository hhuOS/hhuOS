#ifndef HHUOS_RECEIVECONTROL82540EM_H
#define HHUOS_RECEIVECONTROL82540EM_H

#include "device/network/e1000/receive/RcDefault.h"

class ReceiveControl82540EM final : public RcDefault {
public:
    /**
      * Constructor. Same as in extended class.
      */
    explicit ReceiveControl82540EM(Register *request);
    ~ReceiveControl82540EM() override = default;

private:
    /**
     * Inherited methods from RcDefault.
     * This methods are meant to be overridden and
     * implemented by this class.
     */

    void loopbackMode(uint8_t value) final;
    void vlanFilter(bool enable) final;
};

#endif
