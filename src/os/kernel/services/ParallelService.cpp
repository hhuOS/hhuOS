//
// Created by fabian on 11.06.18.
//

#include "ParallelService.h"

ParallelService::ParallelService() {
    if(Parallel::checkPort(Parallel::LPT1)) {
        lpt1 = new Parallel(Parallel::LPT1);
    }

    if(Parallel::checkPort(Parallel::LPT2)) {
        lpt2 = new Parallel(Parallel::LPT2);
    }

    if(Parallel::checkPort(Parallel::LPT3)) {
        lpt3 = new Parallel(Parallel::LPT3);
    }
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
    switch(port) {
        case Parallel::LPT1 :
            return lpt1 != nullptr;
        case Parallel::LPT2 :
            return lpt2 != nullptr;
        case Parallel::LPT3 :
            return lpt3 != nullptr;
        default:
            return false;
    }
}
