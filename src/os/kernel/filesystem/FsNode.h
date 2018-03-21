#ifndef __FsNode_include__
#define __FsNode_include__

#include "lib/LinkedList.h"
#include <cstdint>
#include <lib/String.h>

/**
 * Represents a node in the filesystem-tree.
 * When a file/folder is requested, the FileSystem-class returns a pointer to an FsNode,
 * that corresponds to the requested file/folder. It can then be used to read/write
 * to the file and get meta-information.
 */
class FsNode {

public:
    /**
     * Possible file types, that nodes can have.
     */
    enum FILE_TYPE {
        /**
         * A normal file, that can be read from an written to.
         */
        REGULAR_FILE = 0x01,
        /**
         * A block device file. Reading and writing bytewise is possible, but not efficient.
         * The file's name is mapped to the StorageDevice by the StorageService.
         * For efficient reading and writing to a block device,
         * the best way is to get the StorageDevice from the StorageService.
         */
        BLOCK_FILE = 0x02,
        /**
         * A file, that provides an endless stream of characters. The reported length is always 0.
         */
        CHAR_FILE = 0x03,
        /**
         * A directory. Reading from and writing to it is impossible. The reported length is always 0.
         */
        DIRECTORY_FILE = 0x04
    };

    /**
     * End of file character.
     */
    static const constexpr char END_OF_FILE = 0x04;

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
     * If (pos + numBytes) is greater than the data's length, END_OF_FILE shall be appended.
     * If pos is greater than the data's length, END_OF_FILE shall be the first and only character,
     * that is written to buf.
     *
     * @param buf The buffer to write to (Needs to be allocated already!)
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
     * @return The amount of actually written bytes
     */
    virtual uint64_t writeData(char *buf, uint64_t pos, uint64_t length) = 0;
};

#endif
