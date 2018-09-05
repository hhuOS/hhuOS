#ifndef HHUOS_PARALLELSERVICE_H
#define HHUOS_PARALLELSERVICE_H

#include <kernel/KernelService.h>
#include <devices/Parallel.h>

class ParallelService : public KernelService {

private:

    static Logger &log;

    Parallel *lpt1 = nullptr;
    Parallel *lpt2 = nullptr;
    Parallel *lpt3 = nullptr;

public:

    ParallelService();

    static constexpr const char* SERVICE_NAME = "ParallelService";

    Parallel *getParallelPort(Parallel::LptPort port);

    bool isPortAvailable(Parallel::LptPort port);
};

#endif
