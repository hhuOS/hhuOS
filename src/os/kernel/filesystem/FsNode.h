#ifndef __FsNode_include__
#define __FsNode_include__

#include "lib/LinkedList.h"
#include <cstdint>
#include <lib/String.h>

#define VFS_EOF         4

#define REGULAR_FILE    1
#define BLOCK_FILE      2
#define CHAR_FILE       3
#define FIFO_PIPE_FILE  4
#define DIRECTORY_FILE  5

/**
 * Represents a node in the filesystem-tree.
 * When a file/folder is requested, the FileSystem-class returns a pointer to an FsNode,
 * that corresponds to the requested file/folder. It can then be used to read/write
 * to the file and get meta-information.
 */
class FsNode {

public:
    /**
     * Constructor.
     */
    FsNode() = default;

    /**
     * Copy-constructor.
     */
    FsNode(const FsNode &copy) = delete;

    /**
     * Destructor.
     */
    virtual ~FsNode() = default;
    
    /**
     * Get the name.
     */
    virtual String getName() = 0;

    /**
     * Returns the filetype;
     */
    virtual uint8_t getFileType() = 0;
    
    /**
     * Get the length (in bytes) of the node's data.
     * If the node is a directory, this should always return 0.
     */
    virtual uint64_t getLength() = 0;

    /**
     * Get the node's children.
     */
    virtual Util::Array<String> getChildren() = 0;
    
    /**
     * Read bytes from the node's data.
     * 
     * @param pos The offset
     * @param numBytes The amount of bytes to read
     * 
     * @return The amount of actually read bytes
     */
    virtual uint64_t readData(char *buf, uint64_t pos, uint64_t numBytes) = 0;
    
    /**
     * Write bytes to the node's data. If the offset points right into the existing data,
     * it shall be overwritten with the new data. If the new data does not fit, the data size shall be increased.
     * 
     * @param buf The data to write
     * @param pos The offset
     * @param numBytes The amount of bytes to write
     * 
     * @return The amount actually written bytes
     */
    virtual uint64_t writeData(char *buf, uint64_t pos, uint64_t length) = 0;
};

#endif
