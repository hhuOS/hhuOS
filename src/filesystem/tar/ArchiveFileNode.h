#ifndef HHUOS_ARCHIVEFILENODE_H
#define HHUOS_ARCHIVEFILENODE_H

#include <cstdint>

#include "ArchiveNode.h"
#include "lib/util/data/Array.h"
#include "lib/util/file/Type.h"
#include "lib/util/file/tar/Archive.h"
#include "lib/util/memory/Address.h"
#include "lib/util/memory/String.h"

namespace Filesystem::Tar {

class ArchiveFileNode : public ArchiveNode {

public:
    /**
     * File Constructor.
     */
    ArchiveFileNode(Util::File::Tar::Archive &archive, Util::File::Tar::Archive::Header fileHeader);

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
    Util::Memory::String getName() override;

    /**
     * Overriding function from Node.
     */
    Util::File::Type getFileType() override;

    /**
     * Overriding function from Node.
     */
    uint64_t getLength() override;

    /**
     * Overriding function from Node.
     */
    Util::Data::Array<Util::Memory::String> getChildren() override;

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
    Util::Memory::Address<uint32_t> dataAddress;
    Util::Memory::String name;
};

}

#endif
