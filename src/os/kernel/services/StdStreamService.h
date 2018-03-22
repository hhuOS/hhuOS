#ifndef __StdStreamService_include__
#define __StdStreamService_include__

#include "kernel/KernelService.h"
#include "lib/OutputStream.h"

#include <stdint.h>
#include <lib/InputStream.h>

class StdStreamService : public KernelService {

public:

    StdStreamService() = default;

    OutputStream *getStdout();

    void setStdout(OutputStream *stream);


    OutputStream *getStderr();

    void setStderr(OutputStream *stream);


    InputStream *getStdin();

    void setStdin(InputStream *stream);


    static constexpr const char* SERVICE_NAME = "StdStreamService";

private:

    OutputStream *stdout = nullptr;

    OutputStream *stderr = nullptr;

    InputStream *stdin = nullptr;
};


#endif
