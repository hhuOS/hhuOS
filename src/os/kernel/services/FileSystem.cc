#include <lib/util/List.h>
#include <lib/util/ArrayList.h>
#include <kernel/filesystem/RamFs/graphics/GraphicsVendorNameNode.h>
#include <kernel/filesystem/RamFs/graphics/GraphicsDeviceNameNode.h>
#include <kernel/filesystem/RamFs/graphics/GraphicsResolutionsNode.h>
#include <kernel/filesystem/RamFs/ZeroNode.h>
#include <kernel/filesystem/RamFs/RandomNode.h>
#include <kernel/filesystem/RamFs/graphics/GraphicsMemoryNode.h>
#include "FileSystem.h"
#include "kernel/filesystem/Fat/FatDriver.h"
#include "kernel/filesystem/RamFs/RamFsDriver.h"
#include "devices/block/storage/StorageDevice.h"
#include "kernel/services/StorageService.h"
#include "kernel/filesystem/RamFs/StorageNode.h"
#include "kernel/filesystem/RamFs/PciNode.h"
#include "kernel/filesystem/RamFs/StdoutNode.h"
#include "kernel/filesystem/RamFs/StderrNode.h"



/**
 * Parses a path and removes all '.' and '..' from it.
 * 
 * @param path The path to be parsed.
 * 
 * @return The edited path.
 */
String FileSystem::parsePath(const String &path) {

    Util::Array<String> token = path.split(FileSystem::SEPARATOR);

    Util::List<String> *parsedToken = new Util::ArrayList<String>();

    for (const String &string : token) {

        if (string == ".") {
            continue;
        } else if (string == "..") {
            parsedToken->remove(parsedToken->size() - 1);
        } else {
            parsedToken->add(*new String(string));
        }
    }

    if (parsedToken->isEmpty()) {
        delete parsedToken;
        return FileSystem::ROOT;
    }

    String parsedPath = FileSystem::SEPARATOR + String::join(FileSystem::SEPARATOR, parsedToken->toArray());

    return parsedPath;
}

/**
 * Gets the mount-point, that a given path ends in.
 * 
 * @param path The absolute path.
 * @param pathInMount After the mount-point has been found, this points to the character
 *        in path, that marks the root-directory of the mount-point.
 * 
 * @return The found mount-point.
 */
FileSystem::MountInfo FileSystem::getMountInfo(const String &path) {

    FileSystem::MountPoint *mountPoint = nullptr;

    FileSystem::MountInfo mountInfo = FileSystem::MountInfo(nullptr, "");

    for(uint32_t i = 0; i < mountPoints.length(); i++) {

        FileSystem::MountPoint *currentMountPoint = mountPoints.get(i);

        if (path.beginsWith(currentMountPoint->path)) {
            if(mountPoint == nullptr || currentMountPoint->path.length() > mountPoint->path.length()) {
                mountPoint = currentMountPoint;
            }
        }
    }

    if(mountPoint == nullptr) {
        return mountInfo;
    }

    mountInfo.mountPoint = mountPoint;
    mountInfo.pathInMount = path.remove(mountInfo.mountPoint->path);
    
    if (!mountInfo.pathInMount.beginsWith(FileSystem::SEPARATOR)) {
        mountInfo.pathInMount = FileSystem::SEPARATOR + mountInfo.pathInMount;
    }

    return mountInfo;
}

void FileSystem::init() {
    StorageService *storageService = Kernel::getService<StorageService>();

    // Mount root-device
    StorageDevice *rootDevice = storageService->findRootDevice();

    if(rootDevice == nullptr) {
        // No root-device found -> Mount RAM-Device
        mount("", "/", "ram");
    } else {
        if(mount(rootDevice->getName(), "/", "fat"))
            mount("", "/", "ram");
    }
    
    // Initialize dev-Directory
    Directory *dev = Directory::open("/dev");
    if(dev == nullptr)
        createDirectory("/dev");
    else
        delete dev;

    mount("", "/dev", "ram");

    // Create directory for StorageNodes.
    createDirectory("/dev/storage");

    // Add Video-Nodes to dev-Directory
    createDirectory("/dev/video");
    createDirectory("/dev/video/text");
    createDirectory("/dev/video/lfb");
    addVirtualNode("/dev/video/text", new GraphicsVendorNameNode(GraphicsVendorNameNode::TEXT));
    addVirtualNode("/dev/video/text", new GraphicsDeviceNameNode(GraphicsDeviceNameNode::TEXT));
    addVirtualNode("/dev/video/text", new GraphicsMemoryNode(GraphicsMemoryNode::TEXT));
    addVirtualNode("/dev/video/text", new GraphicsResolutionsNode(GraphicsDeviceNameNode::TEXT));
    addVirtualNode("/dev/video/lfb", new GraphicsVendorNameNode(GraphicsVendorNameNode::LINEAR_FRAME_BUFFER));
    addVirtualNode("/dev/video/lfb", new GraphicsDeviceNameNode(GraphicsDeviceNameNode::LINEAR_FRAME_BUFFER));
    addVirtualNode("/dev/video/lfb", new GraphicsMemoryNode(GraphicsMemoryNode::LINEAR_FRAME_BUFFER));
    addVirtualNode("/dev/video/lfb", new GraphicsResolutionsNode(GraphicsDeviceNameNode::LINEAR_FRAME_BUFFER));

    // Add PCI-Node to dev-Directory
    addVirtualNode("/dev", new PciNode());

    // Add Random-Node to dev-Directory
    addVirtualNode("/dev", new RandomNode());

    // Add Zero-Node to dev-Directory
    addVirtualNode("/dev", new ZeroNode());

    // Add StdStream-nodes to dev-Direcoty
    addVirtualNode("/dev", new StdoutNode());
    addVirtualNode("/dev", new StderrNode());

    // Subcribe to Storage-Event from the EventBus
    EventBus *eventBus = Kernel::getService<EventBus>();
    eventBus->subscribe(*this, StorageAddEvent::TYPE);
    eventBus->subscribe(*this, StorageRemoveEvent::TYPE);
}

int32_t FileSystem::addVirtualNode(const String &path, VirtualNode *node) {

    fsLock.lock();

    FileSystem::MountInfo mountInfo = getMountInfo(parsePath(path));

    RamFsDriver *driver = (RamFsDriver *) mountInfo.mountPoint->driver;
    int32_t ret = driver->addNode(mountInfo.pathInMount, node);

    fsLock.unlock();

    return ret;
}

int32_t FileSystem::createFilesystem(const String &devicePath, const String &fsType) {
    String parsedDevicePath = parsePath(devicePath);

    //Extract device-name from path.
    //For example: devicePath = "/dev/hdd0" -> device = "hdd0"
    Util::Array<String> token = parsedDevicePath.split(FileSystem::SEPARATOR);

    if(token.length() == 0) {
        return - 1;
    }

    String deviceName = token[token.length() - 1];

    fsLock.lock();

    StorageService *storageService = Kernel::getService<StorageService>();
    StorageDevice *disk = storageService->getDevice(deviceName);
    
    FsDriver *tmpDriver = nullptr;
    if(fsType == TYPE_FAT)
        tmpDriver = new FatDriver();
    else if(fsType == TYPE_RAM)
        tmpDriver = new RamFsDriver();
    else {
        fsLock.unlock();
        return -1;
    }
    
    int32_t ret = tmpDriver->makeFs(disk);

    fsLock.unlock();
    delete tmpDriver;
    return ret;
}

int32_t FileSystem::mount(const String &device, const String &path, const String &type) {
    StorageService *storageService = Kernel::getService<StorageService>();
    StorageDevice *disk = nullptr;

    String parsedPath = parsePath(path);
    String parsedDevicePath = parsePath(device);

    String deviceName;

    //Extract device-name from path.
    //For example: devicePath = "/dev/hdd0" -> device = "hdd0"
    Util::Array<String> token = parsedDevicePath.split(FileSystem::SEPARATOR);

    if(token.length() == 0) {
        deviceName = "";
    } else {
        deviceName = token[token.length() - 1];
    }

    fsLock.lock();

    for(uint32_t i = 0; i < mountPoints.length(); i++) {
        if(parsedPath == mountPoints.get(i)->path) {
            fsLock.unlock();
            return -1;
        }
    }

    MountPoint *mountPoint = new MountPoint();

    mountPoint->path = parsedPath;

    if(type == TYPE_FAT) {
        mountPoint->driver = new FatDriver();
    } else if(type == TYPE_RAM) {
        mountPoint->driver = new RamFsDriver();
    }

    disk = storageService->getDevice(deviceName);

    if(mountPoint->driver == nullptr || mountPoint->driver->mount(disk)) {
        fsLock.unlock();
        delete mountPoint->driver;
        delete mountPoint;
        return -1;
    }

    mountPoints.add(mountPoint);

    fsLock.unlock();
    return 0;
}

int32_t FileSystem::unmount(const String &path) {

    String parsedPath = parsePath(path);

    fsLock.lock();

    MountPoint *currentMount = nullptr;

    for(uint32_t i = 0; i < mountPoints.length(); i++) {

        currentMount = mountPoints.get(i);

        if(currentMount->path == path) {

            mountPoints.remove(currentMount);

            fsLock.unlock();

            delete currentMount->driver;

            delete currentMount;

            return 0;
        }
    }

    fsLock.unlock();

    return -1;
}

/**
 * Gets a FsNode, representing a file or directory that a given path points to.
 * 
 * @param path The path.
 * 
 * @return The FsNode or nullptr, if the path is invalid.
 */
FsNode *FileSystem::getNode(const String &path) {

    String parsedPath = parsePath(path);

    if(parsedPath.length() > 4095) {
        return nullptr;
    }

    fsLock.lock();

    MountInfo mountInfo = getMountInfo(parsedPath);

    if(mountInfo.mountPoint == nullptr) {

        fsLock.unlock();

        return nullptr;
    }
    
    FsNode *ret = mountInfo.mountPoint->driver->getNode(mountInfo.pathInMount);

    fsLock.unlock();

    return ret;
}

/**
 * Creates a new empty file at a given path.
 * The parent-folder of the new file must exist beforehand.
 * 
 * @param path The new file's path.
 * 
 * @return 0 on success.
 */
int32_t FileSystem::createFile(const String &path) {

    String parsedPath = parsePath(path);

    if(parsedPath.length() > 4095) {
        return -1;
    }

    fsLock.lock();

    MountInfo mountInfo = getMountInfo(parsedPath);

    if(mountInfo.mountPoint == nullptr) {

        fsLock.unlock();

        return -1;
    }
    
    int32_t ret = mountInfo.mountPoint->driver->createNode(mountInfo.pathInMount, REGULAR_FILE);

    fsLock.unlock();

    return ret;
}

/**
 * Creates a new empty folder at a given path.
 * The parent-folder of the new folder must exist beforehand.
 * 
 * @param path The new directory's path.
 * 
 * @return 0 on success.
 */
int32_t FileSystem::createDirectory(const String &path) {

    String parsedPath = parsePath(path);

    if(parsedPath.length() > 4095) {
        return -1;
    }

    fsLock.lock();

    MountInfo mountInfo = getMountInfo(parsedPath);

    if(mountInfo.mountPoint == nullptr) {

        fsLock.unlock();

        return -1;
    }
    
    int32_t ret = mountInfo.mountPoint->driver->createNode(mountInfo.pathInMount, DIRECTORY_FILE);

    fsLock.unlock();

    return ret;
}

/**
 * Deletes an existing file at a given path.
 * The file must be a regular file or an empty folder (a leaf in the filesystem tree).
 * 
 * @param path The file to be deleted.
 * 
 * @return 0 on success.
 */
int32_t FileSystem::deleteFile(const String &path) {

    String parsedPath = parsePath(path);

    if(parsedPath.length() > 4095) {
        return -1;
    }

    fsLock.lock();

    MountInfo mountInfo = getMountInfo(parsedPath);

    if(mountInfo.mountPoint == nullptr) {

        fsLock.unlock();

        return -1;
    }
    
    int32_t ret = mountInfo.mountPoint->driver->deleteNode(mountInfo.pathInMount);

    fsLock.unlock();

    return ret;
}

FileSystem::MountInfo::MountInfo(FileSystem::MountPoint *mountPoint, const String &pathInMount) {
    this->mountPoint = mountPoint;
    this->pathInMount = pathInMount;
}

void FileSystem::onEvent(const Event &event) {
    switch(event.getType()) {
        case StorageAddEvent::TYPE : {
                StorageDevice *device = ((StorageAddEvent &) event).getDevice();

                deleteFile("/dev/storage/" + device->getName());
                addVirtualNode("/dev/storage/", new StorageNode(device));
            }
            break;
        case StorageRemoveEvent::TYPE : {
                String deviceName = ((StorageRemoveEvent &) event).getDeviceName();
                deleteFile("/dev/storage/" + deviceName);
            }
            break;
        default:
            break;
    }
}
