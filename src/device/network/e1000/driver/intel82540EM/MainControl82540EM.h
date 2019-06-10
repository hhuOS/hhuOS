#ifndef HHUOS_MAINCONTROL82540EM_H
#define HHUOS_MAINCONTROL82540EM_H

#include "device/network/e1000/general/DcDefault.h"

class MainControl82540EM : public DcDefault {
public:
    /**
     * Constructor. Same as in extended class.
     */
    explicit MainControl82540EM(Register *request);
    ~MainControl82540EM() override;

private:
    /**
     * Inherited methods from DcDefault.
     * This methods are meant to be overridden and
     * implemented by this class.
     */

    void linkReset(bool enable) final;
    void invertLossOfSignal(bool enable) final;
    void wakeupAdvertisement(bool enable) final;
    void vlanMode(bool enable) final;
    void resetInternalPhy(bool enable) final;
};

#endif
