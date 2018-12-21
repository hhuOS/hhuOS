/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include <filesystem/FileSystem.h>
#include <kernel/Kernel.h>
#include <kernel/services/PortService.h>
#include "ParallelModule.h"
#include "ParallelNode.h"
#include "ParallelModeNode.h"

MODULE_PROVIDER {

    return new ParallelModule();
};

int32_t ParallelModule::initialize() {
    
    log = &Logger::get("PARALLEL");

    auto *portService = Kernel::getService<PortService>();
    auto *fileSystem = Kernel::getService<FileSystem>();

    if(Parallel::ParallelDriver<Parallel::LPT1>::checkPort()) {
        log->info("Detected LPT1");
        lpt1 = new Parallel::ParallelDriver<Parallel::LPT1>();

        portService->registerPort(lpt1);

        fileSystem->createDirectory("/dev/ports/parallel1");
        fileSystem->addVirtualNode("/dev/ports/parallel1", new Parallel::ParallelNode<Parallel::LPT1>(lpt1));
        fileSystem->addVirtualNode("/dev/ports/parallel1", new Parallel::ParallelModeNode<Parallel::LPT1>(lpt1));
    }

    if(Parallel::ParallelDriver<Parallel::LPT2>::checkPort()) {
        log->info("Detected LPT2");
        lpt2 = new Parallel::ParallelDriver<Parallel::LPT2>();

        portService->registerPort(lpt2);

        fileSystem->createDirectory("/dev/ports/parallel2");
        fileSystem->addVirtualNode("/dev/ports/paralle2", new Parallel::ParallelNode<Parallel::LPT2>(lpt2));
        fileSystem->addVirtualNode("/dev/ports/parallel1", new Parallel::ParallelModeNode<Parallel::LPT2>(lpt2));
    }


    if(Parallel::ParallelDriver<Parallel::LPT3>::checkPort()) {
        log->info("Detected LPT3");
        lpt3 = new Parallel::ParallelDriver<Parallel::LPT3>();

        portService->registerPort(lpt3);

        fileSystem->createDirectory("/dev/ports/parallel3");
        fileSystem->addVirtualNode("/dev/ports/parallel3", new Parallel::ParallelNode<Parallel::LPT3>(lpt3));
        fileSystem->addVirtualNode("/dev/ports/parallel1", new Parallel::ParallelModeNode<Parallel::LPT3>(lpt3));
    }

    return 0;
}

int32_t ParallelModule::finalize() {

    return 0;
}

String ParallelModule::getName() {

    return "parallel";
}

Util::Array<String> ParallelModule::getDependencies() {

    return Util::Array<String>(0);
}