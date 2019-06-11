#ifndef HHUOS_BOOTSEQUENCE_H
#define HHUOS_BOOTSEQUENCE_H

#include "kernel/thread/Thread.h"
#include "lib/async/Spinlock.h"
#include "lib/util/ArrayList.h"
#include "lib/util/SmartPointer.h"
#include "BootComponent.h"

namespace Kernel {

class BootCoordinator : public Thread {

public:

    explicit BootCoordinator(Util::Array<BootComponent *> components, void (*onFinish)());

    BootCoordinator(const BootCoordinator &copy) = delete;

    BootCoordinator &operator=(const BootCoordinator &other) = delete;

    ~BootCoordinator() override = default;

    void run() override;

    void addComponent(BootComponent *component);

    Util::Array<BootComponent *> getComponents();

private:

    void buildComponentLayers();

private:

    Atomic<bool> hasStarted;

    Util::ArrayList<BootComponent *> components;

    Util::ArrayList<Util::List<BootComponent *> *> layeredComponents;

    void (*onFinish)();

};

}

#endif
