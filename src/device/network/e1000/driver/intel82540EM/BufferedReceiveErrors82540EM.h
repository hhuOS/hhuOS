#ifndef HHUOS_BUFFEREDRECEIVEERRORS82540EM_H
#define HHUOS_BUFFEREDRECEIVEERRORS82540EM_H

#include "device/network/e1000/receive/descriptor/field/ReBufferedDefault.h"

class BufferedReceiveErrors82540EM final : public ReBufferedDefault {
public:
    /**
      * Constructor. Same as in extended class.
      */
    explicit BufferedReceiveErrors82540EM(uint8_t *address);

    ~BufferedReceiveErrors82540EM() override = default;

private:
    /**
      * Inherited methods from ReBuffered.
      * This methods are meant to be overridden and
      * implemented by this class.
      */
    bool symbolError() final;
    bool sequenceError() final;
    bool carrierExtensionError() final;
    bool rxDataError() final;
    bool hasErrors() final;
};

#endif
