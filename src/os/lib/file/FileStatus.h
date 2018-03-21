#ifndef __FileStatus_include__
#define __FileStatus_include__

/**
 * Check if a file or directory exists and get Meta-information about it.
 *
 * Call File::open() to open a file.
 * To close a file, just delete the pointer.
 *
 * @author Fabian Ruhland
 * @date 2018
 */
class FileStatus {

private:
    FsNode *node;
    String path;

    FileSystem *fileSystem;

    /**
     * Constructor.
     *
     * @param node The node, representing the file
     */
    FileStatus(FsNode *node, const String &path);

public:
    /**
     * Default-constructor.
     */
    FileStatus() = delete;

    /**
     * Copy-constructor.
     */
    FileStatus(const FileStatus &copy) = delete;

    /**
     * Destructor.
     */
    ~FileStatus();

    /**
     * Check, if a file or directory at a specified path exists.
     *
     * @param path The path
     *
     * @return true, if the file exists
     */
    static bool exists(const String &path);

    /**
     * Open a file or directory.
     * The returned FileStatus-object can be used to get Meta-information about it.
     *
     * @param path The path
     *
     * @return The FileStatus-object (or nullptr on failure)
     */
    static FileStatus *stat(const String &path);

    /**
     * Get the filename.
     */
    String getName();

    /**
     * Get the absolute path, that points to the file
     */
    String getAbsolutePath();

    /**
     * Get the file type.
     */
    uint32_t getFileType();
};
