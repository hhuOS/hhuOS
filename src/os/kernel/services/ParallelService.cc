//
// Created by fabian on 11.06.18.
//

#include "ParallelService.h"

ParallelService::ParallelService() {
    lpt1 = new Parallel(Parallel::LPT1);
    lpt2 = new Parallel(Parallel::LPT2);
    lpt3 = new Parallel(Parallel::LPT3);
}

Parallel *ParallelService::getParallelPort(Parallel::LptPort port) {
    switch(port) {
        case Parallel::LPT1 :
            return lpt1;
        case Parallel::LPT2 :
            return lpt2;
        case Parallel::LPT3 :
            return lpt3;
        default:
            return nullptr;
    }
}

bool ParallelService::isPortAvailable(Parallel::LptPort port) {
    return true;
}
