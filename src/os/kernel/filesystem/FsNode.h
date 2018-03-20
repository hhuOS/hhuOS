#ifndef __FsNode_include__
#define __FsNode_include__

#include "lib/LinkedList.h"
#include <stdint.h>
#include <lib/String.h>

#define VFS_EOF         4

#define REGULAR_FILE    1
#define BLOCK_FILE      2
#define CHAR_FILE       3
#define FIFO_PIPE_FILE  4
#define DIRECTORY_FILE  5

/**
 * Represents a node in the filesystem-tree.
 * When a file/folder is requested, the FileSystem-class returns a pointer to a FsNode,
 * that corresponds to the requested file/folder. It can then be used to read/write
 * to the file and get meta-information.
 */
class FsNode {
private:
    FsNode(const FsNode &copy);

public:
    FsNode() {};
    virtual ~FsNode() {};
    
    /**
     * Returns the node's name.
     * 
     * @return The node's name.
     */
    virtual String getName() = 0;

    /**
     * Returns the node's filetype;
     * 
     * @return The node's filetype.
     */
    virtual uint8_t getFileType() = 0;
    
    /**
     * Returns the length (in bytes) of the node's data.
     * If the node is a directory, this should always return 0.
     * 
     * @return The length (in bytes) of the node's data.
     */
    virtual uint64_t getLength() = 0;

    /**
     * Returns the name of the node's child at index 'pos' (0->First child, 1->second child, ...).
     * 
     * @param pos The child's index.
     * 
     * @return The node's children.
     */
    virtual Util::Array<String> getChildren() = 0;
    
    /**
     * Reads bytes from the node's data.
     * 
     * @param pos The offset, at which the data should be read.
     *            May be ignored for some special files (e.g. senseless for /dev/zero).
     * @param numBytes The amount of bytes, that should be read.
     * 
     * @return The data.
     */
    virtual char *readData(char *buf, uint64_t pos, uint64_t numBytes) = 0;
    
    /**
     * Writes bytes to the node's data. If the offset points right into the existing data,
     * it shall be overwritten with the new data. If the new data does not fit, the data size shall be increased.
     * 
     * @param buf The data, that shall be written to the node.
     * @param pos The offset, at which the new data shall be written.
     *            May be ignored for some special files.
     * @param numBytes The new data's length (in bytes).
     * 
     * @return 0, if the data has been written succesfully.
     */
    virtual int64_t writeData(char *buf, uint64_t pos, uint64_t length) = 0;
};

#endif
