#ifndef HHUOS_ARCHIVEFILENODE_H
#define HHUOS_ARCHIVEFILENODE_H

#include <stdint.h>

#include "ArchiveNode.h"
#include "lib/util/collection/Array.h"
#include "lib/util/io/file/tar/Archive.h"
#include "lib/util/base/Address.h"
#include "lib/util/base/String.h"
#include "lib/util/io/file/File.h"

namespace Filesystem::Tar {

class ArchiveFileNode : public ArchiveNode {

public:
    /**
     * File Constructor.
     */
    ArchiveFileNode(Util::Io::Tar::Archive &archive, Util::Io::Tar::Archive::Header fileHeader);

    /**
     * Copy Constructor.
     */
    ArchiveFileNode(const ArchiveFileNode &copy) = delete;

    /**
     * Assignment operator.
     */
    ArchiveFileNode& operator=(const ArchiveFileNode &other) = delete;

    /**
     * Destructor.
     */
    ~ArchiveFileNode() override = default;

    /**
     * Overriding function from Node.
     */
    Util::String getName() override;

    /**
     * Overriding function from Node.
     */
    Util::Io::File::Type getType() override;

    /**
     * Overriding function from Node.
     */
    uint64_t getLength() override;

    /**
     * Overriding function from Node.
     */
    Util::Array<Util::String> getChildren() override;

    /**
     * Overriding function from Node.
     */
    uint64_t readData(uint8_t *targetBuffer, uint64_t pos, uint64_t numBytes) override;

    /**
     * Overriding function from Node.
     */
    uint64_t writeData(const uint8_t *sourceBuffer, uint64_t pos, uint64_t numBytes) override;

private:

    uint32_t length = 0;
    Util::Address<uint32_t> dataAddress;
    Util::String name;
};

}

#endif
