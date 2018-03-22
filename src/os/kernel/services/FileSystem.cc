#include <kernel/events/storage/StorageAddEvent.h>
#include <kernel/events/storage/StorageRemoveEvent.h>
#include "FileSystem.h"
#include "lib/file/Directory.h"
#include "kernel/filesystem/Fat/FatDriver.h"
#include "kernel/filesystem/RamFs/graphics/GraphicsVendorNameNode.h"
#include "kernel/filesystem/RamFs/graphics/GraphicsDeviceNameNode.h"
#include "kernel/filesystem/RamFs/graphics/GraphicsResolutionsNode.h"
#include "kernel/filesystem/RamFs/ZeroNode.h"
#include "kernel/filesystem/RamFs/RandomNode.h"
#include "kernel/filesystem/RamFs/graphics/GraphicsMemoryNode.h"
#include "kernel/filesystem/RamFs/RamFsDriver.h"
#include "kernel/filesystem/RamFs/StorageNode.h"
#include "kernel/filesystem/RamFs/PciNode.h"
#include "kernel/filesystem/RamFs/StdoutNode.h"
#include "kernel/filesystem/RamFs/StderrNode.h"

FileSystem::FileSystem() {
    eventBus = Kernel::getService<EventBus>();
    storageService = Kernel::getService<StorageService>();
}

FileSystem::~FileSystem() {
    eventBus->unsubscribe(*this, StorageAddEvent::TYPE);
    eventBus->unsubscribe(*this, StorageRemoveEvent::TYPE);
}

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

FsDriver *FileSystem::getMountedDriver(String &path) {
    String ret = String();

    for(const String &currentString : mountPoints.keySet()) {
        if (path.beginsWith(currentString)) {
            if(currentString.length() > ret.length()) {
                ret = currentString;
            }
        }
    }

    if(ret.isEmpty()) {
        return nullptr;
    }

    path = path.substring(ret.length(), path.length());
    return mountPoints.get(ret);
}

void FileSystem::init() {
    // Mount root-device
    StorageDevice *rootDevice = storageService->findRootDevice();

    if(rootDevice == nullptr) {
        // No root-device found -> Mount RAM-Device
        mount("", "/", "ram");
    } else {
        if(mount(rootDevice->getName(), "/", "fat") != SUCCESS) {
            mount("", "/", "ram");
        }
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

    // Add StdStream-nodes to dev-Directory
    addVirtualNode("/dev", new StdoutNode());
    addVirtualNode("/dev", new StderrNode());

    // Subscribe to Storage-Events from the EventBus
    eventBus->subscribe(*this, StorageAddEvent::TYPE);
    eventBus->subscribe(*this, StorageRemoveEvent::TYPE);
}

uint32_t FileSystem::addVirtualNode(const String &path, VirtualNode *node) {
    fsLock.lock();

    String parsedPath = parsePath(path);
    auto *driver = (RamFsDriver*) getMountedDriver(parsedPath);

    bool ret = driver->addNode(parsedPath, node);

    fsLock.unlock();

    return ret ? SUCCESS : ADDING_VIRTUAL_NODE_FAILED;
}

uint32_t FileSystem::createFilesystem(const String &devicePath, const String &fsType) {
    // Check if device-node exists
    FsNode *deviceNode = getNode(devicePath);
    if(deviceNode == nullptr) {
        return DEVICE_NOT_FOUND;
    }

    // Get device
    StorageDevice *disk = storageService->getDevice(deviceNode->getName());
    if(disk == nullptr) {
        return DEVICE_NOT_FOUND;
    }

    fsLock.lock();

    // Create temporary driver
    FsDriver *tmpDriver = nullptr;

    if(fsType == TYPE_FAT)
        tmpDriver = new FatDriver();
    else if(fsType == TYPE_RAM)
        tmpDriver = new RamFsDriver();
    else {
        fsLock.unlock();
        return INVALID_DRIVER;
    }

    // Format device
    bool ret = tmpDriver->createFs(disk);

    fsLock.unlock();

    delete tmpDriver;
    return ret ? SUCCESS : FORMATTING_FAILED;
}

uint32_t FileSystem::mount(const String &devicePath, const String &targetPath, const String &type) {
    StorageDevice *disk = nullptr;
    String parsedDevicePath = parsePath(devicePath);

    if(type != TYPE_RAM) {
        // Check if device-node exists
        FsNode *deviceNode = getNode(parsedDevicePath);

        if(deviceNode == nullptr) {
            // Device node is non-existent,
            // Check if devicePath is the name of a storage device.
            disk = storageService->getDevice(devicePath);
            if(disk == nullptr) {
                return DEVICE_NOT_FOUND;
            }
        } else {
            disk = storageService->getDevice(deviceNode->getName());
            if(disk == nullptr) {
                return DEVICE_NOT_FOUND;
            }
        }
    }

    String parsedPath = parsePath(targetPath);
    FsNode *targetNode = getNode(parsedPath);

    if(targetNode == nullptr) {
        if(targetPath != "/") {
            return FILE_NOT_FOUND;
        }
    } else {
        delete targetNode;
    }

    fsLock.lock();

    if(mountPoints.containsKey(parsedPath)) {
        fsLock.unlock();
        return MOUNT_TARGET_ALREADY_USED;
    }

    FsDriver *driver = nullptr;

    if(type == TYPE_FAT) {
        driver = new FatDriver();
    } else if(type == TYPE_RAM) {
        driver = new RamFsDriver();
    } else {
        fsLock.unlock();
        return INVALID_DRIVER;
    }

    if(!driver->mount(disk)) {
        fsLock.unlock();
        delete driver;
        return MOUNTING_FAILED;
    }

    mountPoints.put(parsedPath, driver);

    fsLock.unlock();
    return SUCCESS;
}

uint32_t FileSystem::unmount(const String &path) {
    String parsedPath = parsePath(path);
    FsNode *targetNode = getNode(parsedPath);

    if(targetNode == nullptr) {
        if(path != "/") {
            return FILE_NOT_FOUND;
        }
    } else {
        delete targetNode;
    }

    fsLock.lock();

    for(const String &key : mountPoints.keySet()) {
        if(key.beginsWith(parsedPath)) {
            if(key != parsedPath) {
                fsLock.unlock();
                return SUBDIRECTORY_CONTAINS_MOUNT_POINT;
            }
        }
    }

    if(mountPoints.containsKey(parsedPath)) {
        delete mountPoints.get(parsedPath);
        mountPoints.remove(parsedPath);

        fsLock.unlock();
        return SUCCESS;
    }

    fsLock.unlock();

    return NOTHING_MOUNTED_AT_PATH;
}

FsNode *FileSystem::getNode(const String &path) {
    String parsedPath = parsePath(path);

    fsLock.lock();

    FsDriver *driver = getMountedDriver(parsedPath);

    if(driver == nullptr) {
        fsLock.unlock();
        return nullptr;
    }
    
    FsNode *ret = driver->getNode(parsedPath);

    fsLock.unlock();

    return ret;
}

uint32_t FileSystem::createFile(const String &path) {
    String parsedPath = parsePath(path);

    fsLock.lock();

    FsDriver *driver = getMountedDriver(parsedPath);

    if(driver == nullptr) {
        fsLock.unlock();
        return FILE_NOT_FOUND;
    }
    
    bool ret = driver->createNode(parsedPath, FsNode::REGULAR_FILE);

    fsLock.unlock();

    return ret ? SUCCESS : CREATING_FILE_FAILED;
}

uint32_t FileSystem::createDirectory(const String &path) {
    String parsedPath = parsePath(path);
    fsLock.lock();

    FsDriver *driver = getMountedDriver(parsedPath);

    if(driver == nullptr) {
        fsLock.unlock();
        return FILE_NOT_FOUND;
    }

    bool ret = driver->createNode(parsedPath, FsNode::DIRECTORY_FILE);

    fsLock.unlock();

    return ret ? SUCCESS : CREATING_DIRECTORY_FAILED;
}

uint32_t FileSystem::deleteFile(const String &path) {
    String parsedPath = parsePath(path);

    fsLock.lock();

    for(const String &key : mountPoints.keySet()) {
        if(key.beginsWith(parsedPath)) {
            fsLock.unlock();
            return SUBDIRECTORY_CONTAINS_MOUNT_POINT;
        }
    }

    FsDriver *driver = getMountedDriver(parsedPath);

    if(driver == nullptr) {
        fsLock.unlock();
        return FILE_NOT_FOUND;
    }
    
    bool ret = driver->deleteNode(parsedPath);

    fsLock.unlock();

    return ret ? SUCCESS : DELETING_FILE_FAILED;
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
