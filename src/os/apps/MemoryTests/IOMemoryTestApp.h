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

#ifndef HHUOS_IOMEMORYTESTAPP_H
#define HHUOS_IOMEMORYTESTAPP_H


#include <lib/Random.h>
#include <kernel/memory/manager/IOMemoryManager.h>
#include <kernel/threads/Thread.h>

/**
 * IOMemoryTestApp - tests the io memory area with a certain number of allocations and frees
 *
 * @author Burak Akguel, Christian Gesse, Filip Krakowski, Fabian Ruhland, Michael Schoettner
 * @date 2018
 */
class IOMemoryTestApp : public Thread {

private:

    Random *random;
    void **objects;
    unsigned int *stats;

    void runTest();
    void shuffle();

public:

    IOMemoryTestApp();
    IOMemoryTestApp(const IOMemoryTestApp &other) = delete;
    ~IOMemoryTestApp() override ;

    void run() override;
};


#endif
