#ifndef HHUOS_ARCHIVEDIRECTORYNODE_H
#define HHUOS_ARCHIVEDIRECTORYNODE_H

#include <cstdint>

#include "ArchiveNode.h"
#include "lib/util/collection/Array.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/base/String.h"
#include "lib/util/io/file/File.h"

namespace Util {
namespace Io {
namespace Tar {
class Archive;
}  // namespace Tar
}  // namespace File
}  // namespace Util

namespace Filesystem::Tar {

class ArchiveDirectoryNode : public ArchiveNode {

public:
    /**
     * Constructor.
     */
    ArchiveDirectoryNode(Util::Io::Tar::Archive &archive, const Util::String &path);

    /**
     * Copy Constructor.
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

    Util::String name;
    Util::ArrayList<Util::String> children;

};

}

#endif
