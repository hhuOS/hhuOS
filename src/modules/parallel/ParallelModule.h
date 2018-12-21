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

#ifndef HHUOS_PARALLELMODULE_H
#define HHUOS_PARALLELMODULE_H

#include <kernel/Module.h>
#include <kernel/log/Logger.h>
#include "ParallelDriver.h"

class ParallelModule : public Module {

public:

    ParallelModule() = default;

    int32_t initialize() override;

    int32_t finalize() override;

    String getName() override;

    Util::Array <String> getDependencies() override;

private:

    Logger *log = nullptr;

    Parallel::ParallelDriver<Parallel::LPT1> *lpt1 = nullptr;
    Parallel::ParallelDriver<Parallel::LPT2> *lpt2 = nullptr;
    Parallel::ParallelDriver<Parallel::LPT3> *lpt3 = nullptr;

};

#endif
