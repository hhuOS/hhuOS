#ifndef UsbRunnable__include
#define UsbRunnable__include

#include "../../../lib/util/async/Runnable.h"

class UsbRunnable : public Util::Async::Runnable {

public:

    explicit UsbRunnable(void* controller);

    UsbRunnable(const UsbRunnable& other) = delete;

    UsbRunnable &operator=(const UsbRunnable& other) = delete;

    void run() override;

private:

    void* controller;

};

#endif