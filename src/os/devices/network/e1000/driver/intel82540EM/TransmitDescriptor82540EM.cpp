#include "TransmitDescriptor82540EM.h"

TransmitDescriptor82540EM::TransmitDescriptor82540EM(uint8_t *address, LegacyCommand *command, TransmitStatus *status) :
        TdDefault(address, command, status) {

}

void TransmitDescriptor82540EM::writeCommand() {
    command->isEndOfPacket(true);
    command->insertFrameCheckSequence(true);
    command->reportStatus(true);
    command->legacyMode(true);
    command->manage();
}

bool TransmitDescriptor82540EM::isDone() {
    return status->descriptorDone() || status->lateCollision() || status->excessCollisions();
}
