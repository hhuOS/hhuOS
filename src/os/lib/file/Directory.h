#ifndef __Directory_include__
#define __Directory_include__


#include "filesystem/FileSystem.h"
#include "kernel/Kernel.h"
#include "lib/util/Array.h"
#include <cstdint>
/**
 * Allows interacting with a directory.
 *
 * Call Directory::open() to open a directory.
 * To close a directory, just delete the pointer.
 *
 * @author Fabian Ruhland
 * @date 2017
 */
class Directory {

private:
    FsNode *node;
    String path;

    /**
     * Constructor.
     *
     * @param node The node, representing the directory
     * @param path The absolute path, that points to the directory
     */
    Directory(FsNode *node, const String &path);

public:
    /**
     * Default-constructor.
     */
    Directory() = delete;

    /**
     * Copy-constructor.
     */
    Directory(const Directory &copy) = delete;

    /**
     * Destructor.
     */
    ~Directory();

    /**
     * Open a directory.
     * CAUTION: May return nullptr, if the directory does not exist.
     *          Always check the return value!
     *
     * @param path The absolute path, that points to the file
     *
     * @return The directory (or nulltpr on failure)
     */
    static Directory *open(const String &path);

    /**
     * Get the directory's name.
     */
    String getName();

    /**
     * Get the absolute path, that points to the directory.
     */
    String getAbsolutePath();

    /**
     * Get the names of the directory's children.
     */
    Util::Array<String> getChildren();
};

#endif