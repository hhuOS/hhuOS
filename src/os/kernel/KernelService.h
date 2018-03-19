/**
 * KernelService - Base class for all Kernel services.
 *
 * @author Michael Schoettner, Filip Krakowski, Christian Gesse, Fabian Ruhland, Burak Akguel
 * @date HHU, 2018
 */

#ifndef __KernelService_include__
#define __KernelService_include__


#include <cstdint>

/**
 * @author Filip Krakowski
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
