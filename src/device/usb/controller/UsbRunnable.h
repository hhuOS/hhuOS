#ifndef UsbRunnable__include
#define UsbRunnable__include

#include "../../../lib/util/async/Runnable.h"
extern "C"{
#include "UsbController.h"
}

class UsbRunnable : public Util::Async::Runnable {

public:

    explicit UsbRunnable(UsbController* controller);

    UsbRunnable(const UsbRunnable& other) = delete;

    UsbRunnable &operator=(const UsbRunnable& other) = delete;

    void run() override;

private:
    UsbController* controller;
};

#endif