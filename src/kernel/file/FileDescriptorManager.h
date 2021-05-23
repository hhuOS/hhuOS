#ifndef HHUOS_FILEDESCRIPTORMANAGER_H
#define HHUOS_FILEDESCRIPTORMANAGER_H

#include <kernel/service/FilesystemService.h>
#include <filesystem/core/Node.h>

namespace Kernel {

class FileDescriptorManager {

public:
    /**
     * Constructor.
     */
    explicit FileDescriptorManager(uint32_t size = DEFAULT_TABLE_SIZE);

    /**
     * Copy constructor.
     */
    FileDescriptorManager(const FileDescriptorManager &copy) = delete;

    /**
     * Assignment operator.
     */
    FileDescriptorManager& operator=(const FileDescriptorManager &other) = delete;

    /**
     * Destructor.
     */
    ~FileDescriptorManager() = default;

    int32_t openFile(const Util::Memory::String &path);

    void closeFile(int32_t fileDescriptor);

    Filesystem::Node& getNode(int32_t fileDescriptor);

private:

    uint32_t size;
    Filesystem::Node **descriptorTable;

    static const constexpr uint32_t DEFAULT_TABLE_SIZE = 1024;

};

}


#endif
