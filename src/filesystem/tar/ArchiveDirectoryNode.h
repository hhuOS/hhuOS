#ifndef HHUOS_ARCHIVEDIRECTORYNODE_H
#define HHUOS_ARCHIVEDIRECTORYNODE_H

#include "ArchiveNode.h"

namespace Filesystem::Tar {

class ArchiveDirectoryNode : public ArchiveNode {

public:
    /**
     * Constructor.
     */
    ArchiveDirectoryNode(Util::File::Tar::Archive &archive, const Util::Memory::String &path);

    /**
     * Copy-constructor.
     */
    ArchiveDirectoryNode(const ArchiveDirectoryNode &copy) = delete;

    /**
     * Assignment operator.
     */
    ArchiveDirectoryNode& operator=(const ArchiveDirectoryNode &other) = delete;

    /**
     * Destructor.
     */
    ~ArchiveDirectoryNode() override = default;

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

    Util::Memory::String name;
    Util::Data::ArrayList<Util::Memory::String> children;

};

}

#endif
