#ifndef __Tar_Archive_include__
#define __Tar_Archive_include__

#include "lib/File.h"
#include "lib/Address.h"
#include "lib/util/ArrayList.h"
#include "lib/file/tar/Constants.h"

namespace Tar {

    /**
     * @author Filip Krakowski
     */
    class Archive {

    public:

        ~Archive() = default;

        Archive(const Archive &other) = delete;

        Archive& operator=(const Archive &other) = delete;

        /**
         * Reads a tar archive from a specific file.
         *
         * @param file The file pointing to the tar archive
         * @return The tar archive
         */
        static Archive& from(File &file);

        /**
         * Reads a tar archive from a specific address in memory.
         *
         * @param address The address at which the tar archive starts
         * @return The tar archive
         */
        static Archive& from(Address &address);

    private:

        Archive() = default;

        uint32_t fileCount = 0;

        uint32_t totalSize = 0;

        Util::ArrayList<Header> headers;

        /**
         * Converts the size (base8) to the decimal system.
         *
         * @param size The size in base8
         * @return The size in base10
         */
        static uint32_t calculateSize(const uint8_t* size);

        static const uint32_t BLOCKSIZE = 0x200;
    };
}


#endif
