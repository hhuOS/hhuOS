#ifndef __FatNode_include__
#define __FatNode_include__

#include "../FsNode.h"
#include "FatFsLib/FatFs.h"
#include <stdint.h>

extern "C" {
#include "lib/libc/string.h"
}

/**
 * An implementation of FsNode for the FatFs-library.
 *
 * @author Fabian Ruhland
 * @date 2018
 */
class FatNode : public FsNode {

private:
    union FatObject {
        FIL file;
        DIR dir;
    };

    FatFs *fatInstance;
    FatObject fatObject{};
    FILINFO info{};
    String path{};

    /**
     * Constructor.
     *
     * @param fatInstance Instance of FatDriver
     */
    explicit FatNode(FatFs *fatInstance);

public:
    /**
     * Copy-constructor.
     */
    FatNode(const FatNode &copy) = delete;

    /**
     * Destructor.
     */
    ~FatNode() override;

    /**
     * Open a FatNode.
     * CAUTION: May return nullptr, if the file does not exist.
     *          Always check the return value!
     *
     * @param path The absolute path (inside the FAT-filesystem), that points to the file
     * @param fatInstance The instance of FatDriver, that manages the drive, on which the requested file is located.
     *
     * @return The FatNode (or nulltpr on failure)
     */
    static FatNode *open(const String &path, FatFs *fatInstance);

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
};

#endif