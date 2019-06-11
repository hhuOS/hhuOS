#ifndef HHUOS_BOOTCOMPONENT_H
#define HHUOS_BOOTCOMPONENT_H

#include "kernel/thread/Thread.h"
#include "lib/util/Array.h"
#include "lib/util/ArrayList.h"

namespace Kernel {

class BootComponent : public Thread {

public:

    BootComponent(const String &name, Util::Array<BootComponent *> dependencies, void (*function)());

    BootComponent(const BootComponent &copy) = delete;

    BootComponent &operator=(const BootComponent &other) = delete;

    ~BootComponent() override = default;

    void run() override;

    bool isWaiting();

    bool hasFinished();

    void addDependency(BootComponent *dependency);

    bool hasDependencies();

    void removeDependency(BootComponent *dependency);

private:

    bool waiting;

    bool finished;

    Util::ArrayList<BootComponent *> dependencies;

    void (*function)();
};

}

#endif
