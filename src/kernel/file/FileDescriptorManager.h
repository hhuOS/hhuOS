#ifndef HHUOS_FILEDESCRIPTORMANAGER_H
#define HHUOS_FILEDESCRIPTORMANAGER_H

#include "filesystem/core/Node.h"

namespace Kernel {

class FileDescriptorManager {

public:
    /**
     * Constructor.
     */
    explicit FileDescriptorManager(int32_t size = DEFAULT_TABLE_SIZE);

    /**
     * Copy Constructor.
     */
    FileDescriptorManager(const FileDescriptorManager &copy) = delete;

    /**
     * Assignment operator.
     */
    FileDescriptorManager& operator=(const FileDescriptorManager &other) = delete;

    /**
     * Destructor.
     */
    ~FileDescriptorManager();

    int32_t openFile(const Util::Memory::String &path);

    void closeFile(int32_t fileDescriptor);

    Filesystem::Node& getNode(int32_t fileDescriptor);

private:

    int32_t size;
    Filesystem::Node **descriptorTable;

    static const constexpr int32_t DEFAULT_TABLE_SIZE = 1024;

};

}


#endif
