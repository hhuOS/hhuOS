#ifndef __Tar_Constants_include__
#define __Tar_Constants_include__

#include <cstdint>

namespace Tar {

    struct Header {
        uint8_t filename[100];
        uint8_t mode[8];
        uint8_t userId[8];
        uint8_t groupId[8];
        uint8_t size[12];
        uint8_t modificationTime[12];
        uint8_t checkSum[8];
        uint8_t typeFlag[1];
        uint8_t padding[355];

        bool operator!=(const Header &other) const;
    };
}

#endif
