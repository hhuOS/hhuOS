#ifndef __StdStreamService_include__
#define __StdStreamService_include__

#include "kernel/KernelService.h"
#include "lib/OutputStream.h"

#include <stdint.h>

class StdStreamService : public KernelService {

public:

    StdStreamService() = default;

    OutputStream *getStdout();

    void setStdout(OutputStream *stream);


    OutputStream *getStderr();

    void setStderr(OutputStream *stream);


    static constexpr char* SERVICE_NAME = "StdStreamService";

private:

    OutputStream *stdout = nullptr;

    OutputStream *stderr = nullptr;

};


#endif
