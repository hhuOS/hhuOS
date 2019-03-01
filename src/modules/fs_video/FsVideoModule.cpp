#include <kernel/Kernel.h>
#include "FsVideoModule.h"
#include "GraphicsVendorNameNode.h"
#include "GraphicsDeviceNameNode.h"
#include "GraphicsMemoryNode.h"
#include "GraphicsResolutionsNode.h"
#include "CurrentResolutionNode.h"
#include "GraphicsDriversNode.h"
#include "CurrentGraphicsDriverNode.h"

MODULE_PROVIDER {

    return new FsVideoModule();
};

int32_t FsVideoModule::initialize() {

    log = &Logger::get("FILESYSTEM");

    fileSystem = Kernel::getService<FileSystem>();

    fileSystem->createDirectory("/dev/video");
    fileSystem->createDirectory("/dev/video/text");
    fileSystem->createDirectory("/dev/video/lfb");

    fileSystem->addVirtualNode("/dev/video/text", new GraphicsVendorNameNode(GraphicsNode::TEXT));
    fileSystem->addVirtualNode("/dev/video/text", new GraphicsDeviceNameNode(GraphicsNode::TEXT));
    fileSystem->addVirtualNode("/dev/video/text", new GraphicsMemoryNode(GraphicsNode::TEXT));
    fileSystem->addVirtualNode("/dev/video/text", new GraphicsResolutionsNode(GraphicsNode::TEXT));
    fileSystem->addVirtualNode("/dev/video/text", new CurrentResolutionNode(GraphicsNode::TEXT));
    fileSystem->addVirtualNode("/dev/video/text", new GraphicsDriversNode(GraphicsNode::TEXT));
    fileSystem->addVirtualNode("/dev/video/text", new CurrentGraphicsDriverNode(GraphicsNode::TEXT));
    fileSystem->addVirtualNode("/dev/video/lfb", new GraphicsVendorNameNode(GraphicsNode::LINEAR_FRAME_BUFFER));
    fileSystem->addVirtualNode("/dev/video/lfb", new GraphicsDeviceNameNode(GraphicsNode::LINEAR_FRAME_BUFFER));
    fileSystem->addVirtualNode("/dev/video/lfb", new GraphicsMemoryNode(GraphicsNode::LINEAR_FRAME_BUFFER));
    fileSystem->addVirtualNode("/dev/video/lfb", new GraphicsResolutionsNode(GraphicsNode::LINEAR_FRAME_BUFFER));
    fileSystem->addVirtualNode("/dev/video/lfb", new CurrentResolutionNode(GraphicsNode::LINEAR_FRAME_BUFFER));
    fileSystem->addVirtualNode("/dev/video/lfb", new GraphicsDriversNode(GraphicsNode::LINEAR_FRAME_BUFFER));
    fileSystem->addVirtualNode("/dev/video/lfb", new CurrentGraphicsDriverNode(GraphicsNode::LINEAR_FRAME_BUFFER));

    return 0;
}

int32_t FsVideoModule::finalize() {

    return 0;
}

String FsVideoModule::getName() {

    return String();
}

Util::Array<String> FsVideoModule::getDependencies() {

    return Util::Array<String>(0);
}

void FsVideoModule::creatNode(const char *path, VirtualNode *node) {

    if(fileSystem->addVirtualNode(path, node) == FileSystem::ADDING_VIRTUAL_NODE_FAILED) {

        log->error("Unable to create '%s/%s'", path, (const char*) node->getName());
    }
}
