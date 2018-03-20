#include "Archive.h"

namespace Tar {

    uint32_t Archive::calculateSize(const uint8_t *size) {

        uint32_t ret = 0;

        uint32_t i = 0;

        uint32_t count = 1;

        for (i = 11; i > 0; i--, count *= 8) {

            ret += ((size[i - 1] - '0') * count);
        }

        return ret;
    }

    Archive &Archive::from(Address &address) {

        Archive *tar = new Archive();

        uint32_t i;

        uint32_t addr = (uint32_t) address;

        for (i = 0; ; i++) {

            Header *header = (Header*) addr;

            if (header->filename[0] == '\0') {

                break;
            }

            uint32_t size = calculateSize(header->size);

            tar->totalSize += size;

            tar->headers.add(*header);

            addr += ((size / BLOCKSIZE) + 1) * BLOCKSIZE;

            if (size % 512) {

                addr += 512;
            }
        }

        tar->fileCount = i;

        return *tar;
    }


    Archive &Archive::from(File &file) {

        DirEntry *entry = file.getInfo();

        char *buffer = new char[entry->length];

        file.readBytes(buffer, entry->length);

        Address address((uint32_t) buffer);

        return from(address);
    }
}



