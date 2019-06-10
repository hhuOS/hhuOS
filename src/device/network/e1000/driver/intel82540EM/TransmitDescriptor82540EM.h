#ifndef HHUOS_TRANSMITDESCRIPTOR82540EM_H
#define HHUOS_TRANSMITDESCRIPTOR82540EM_H

#include "device/network/e1000/transmit/descriptor/legacy/TdDefault.h"

class TransmitDescriptor82540EM final : public TdDefault {
public:
    /**
      * Constructor. Same as in extended class.
      */
    explicit TransmitDescriptor82540EM(uint8_t *address, LegacyCommand *command, TransmitStatus *status);

    ~TransmitDescriptor82540EM() override = default;

protected:
    /**
     * Inherited methods from TdDefault.
     * This methods are meant to be overridden and
     * implemented by this class.
     */

    void writeCommand() final;
    bool isDone() final;
};

#endif
