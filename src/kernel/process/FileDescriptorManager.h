#ifndef HHUOS_FILEDESCRIPTORMANAGER_H
#define HHUOS_FILEDESCRIPTORMANAGER_H

#include <stdint.h>

#include "lib/util/base/String.h"

namespace Filesystem {
class Node;
}  // namespace Filesystem

namespace Kernel {
class FileDescriptor;

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

    int32_t registerFile(Filesystem::Node *node) const;

    int32_t openFile(const Util::String &path) const;

    void closeFile(int32_t fileDescriptor) const;

    [[nodiscard]] FileDescriptor& getDescriptor(int32_t fileDescriptor) const;

    int32_t size;
    FileDescriptor *descriptorTable;

    static const constexpr int32_t DEFAULT_TABLE_SIZE = 1024;

};

}


#endif
