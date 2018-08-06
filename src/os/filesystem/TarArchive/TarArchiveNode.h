#ifndef HHUOS_TARARCHIVENODE_H
#define HHUOS_TARARCHIVENODE_H

#include <filesystem/FsNode.h>
#include <lib/file/tar/Archive.h>

class TarArchiveNode : public FsNode {

private:

    FileType fileType;

    String fileName;

    uint32_t fileSize = 0;

    uint8_t *fileBuffer = nullptr;

    Util::ArrayList<String> children;

public:
    /**
     * Constructor.
     */
    TarArchiveNode(Tar::Archive *archive, Tar::Header *fileHeader);

    TarArchiveNode(Tar::Archive *archive, const String &path);

    /**
     * Copy-constructor.
     */
    TarArchiveNode(const TarArchiveNode &copy) = delete;

    /**
     * Destructor.
     */
    ~TarArchiveNode() override = default;

    /**
     * Overriding function from FsNode.
     */
    String getName() override;

    /**
     * Overriding function from FsNode.
     */
    uint8_t getFileType() override;

    /**
     * Overriding function from FsNode.
     */
    uint64_t getLength() override;

    /**
     * Overriding function from FsNode.
     */
    Util::Array<String> getChildren() override;

    /**
     * Overriding function from FsNode.
     */
    uint64_t readData(char *buf, uint64_t pos, uint64_t numBytes) override;

    /**
     * Overriding function from FsNode.
     */
    uint64_t writeData(char *buf, uint64_t pos, uint64_t numBytes) override;

private:

    uint32_t calculateFileSize(const uint8_t octalSize[12]);
};

#endif
