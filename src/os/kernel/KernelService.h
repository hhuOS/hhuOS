/*
 * Copyright (C) 2018  Filip Krakowski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __KernelService_include__
#define __KernelService_include__


#include <cstdint>

/**
 * KernelService - Base class for all Kernel services.
 *
 * @author Michael Schoettner, Filip Krakowski, Christian Gesse, Fabian Ruhland, Burak Akguel
 * @date HHU, 2018
 */
class KernelService {

public:

    KernelService() = default;

    static const uint32_t   TIME                    = 0x00000001;
    static const uint32_t   FILE_SYSTEM             = 0x00000002;
    static const uint32_t   DEBUG_SERVICE           = 0x00000003;
    static const uint32_t   EVENT_BUS               = 0x00000004;
    static const uint32_t   TEXT_STREAM             = 0x00000005;
    static const uint32_t   LINEAR_FRAME_BUFFER     = 0x00000006;
};


#endif
