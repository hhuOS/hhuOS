#ifndef HHUOS_FSVIDEOMODULE_H
#define HHUOS_FSVIDEOMODULE_H

#include <kernel/Module.h>
#include <kernel/log/Logger.h>
#include <filesystem/FileSystem.h>

class FsVideoModule : public Module {

public:

    FsVideoModule() = default;

    int32_t initialize() override;

    int32_t finalize() override;

    String getName() override;

    Util::Array<String> getDependencies() override;

private:

    void creatNode(const char *path, VirtualNode *node);

private:

    Logger *log = nullptr;

    FileSystem *fileSystem = nullptr;
};

#endif
