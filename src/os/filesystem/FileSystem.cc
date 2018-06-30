/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include <kernel/events/storage/StorageAddEvent.h>
#include <kernel/events/storage/StorageRemoveEvent.h>
#include <filesystem/RamFs/memory/KernelHeapNode.h>
#include <filesystem/RamFs/memory/IOMemoryNode.h>
#include <filesystem/RamFs/memory/PFANode.h>
#include <filesystem/RamFs/memory/PagingAreaNode.h>
#include <filesystem/RamFs/ports/SerialNode.h>
#include <kernel/services/ParallelService.h>
#include <filesystem/RamFs/ports/ParallelNode.h>
#include <kernel/log/FileAppender.h>
#include <kernel/log/Logger.h>
#include "FileSystem.h"
#include "lib/file/Directory.h"
#include "filesystem/Fat/FatDriver.h"
#include "filesystem/RamFs/graphics/GraphicsVendorNameNode.h"
#include "filesystem/RamFs/graphics/GraphicsDeviceNameNode.h"
#include "filesystem/RamFs/graphics/GraphicsResolutionsNode.h"
#include "filesystem/RamFs/graphics/GraphicsMemoryNode.h"
#include "filesystem/RamFs/RamFsDriver.h"
#include "filesystem/RamFs/storage/StorageNode.h"
#include "filesystem/RamFs/PciNode.h"
#include "filesystem/RamFs/StdoutNode.h"
#include "filesystem/RamFs/StderrNode.h"

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
            if(!parsedToken->isEmpty()) {
                parsedToken->remove(parsedToken->size() - 1);
            }
        } else {
            parsedToken->add(*new String(string));
        }
    }

    if (parsedToken->isEmpty()) {
        delete parsedToken;
        return "";
    }

    String parsedPath = FileSystem::SEPARATOR + String::join(FileSystem::SEPARATOR, parsedToken->toArray());
    return parsedPath;
}

FsDriver *FileSystem::getMountedDriver(String &path) {
    if(!path.endsWith(FileSystem::SEPARATOR)) {
        path += FileSystem::SEPARATOR;
    }

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

    path = path.substring(ret.length(), path.length() - 1);
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

    // Add Serial-nodes to dev-Directory
    createDirectory("/dev/ports");

    auto *serialService = Kernel::getService<SerialService>();

    if(serialService->isPortAvailable(Serial::COM1)) {
        addVirtualNode("/dev/ports", new SerialNode(serialService->getSerialPort(Serial::COM1)));
    }

    if(serialService->isPortAvailable(Serial::COM2)) {
        addVirtualNode("/dev/ports", new SerialNode(serialService->getSerialPort(Serial::COM2)));
    }

    if(serialService->isPortAvailable(Serial::COM3)) {
        addVirtualNode("/dev/ports", new SerialNode(serialService->getSerialPort(Serial::COM3)));
    }

    if(serialService->isPortAvailable(Serial::COM4)) {
        addVirtualNode("/dev/ports", new SerialNode(serialService->getSerialPort(Serial::COM4)));
    }

    // Add parallel-nodes to dev-Directory
    auto *parallelService = Kernel::getService<ParallelService>();

    if(parallelService->isPortAvailable(Parallel::LPT1)) {
        addVirtualNode("/dev/ports", new ParallelNode(parallelService->getParallelPort(Parallel::LPT1)));
    }

    if(parallelService->isPortAvailable(Parallel::LPT2)) {
        addVirtualNode("/dev/ports", new ParallelNode(parallelService->getParallelPort(Parallel::LPT2)));
    }

    if(parallelService->isPortAvailable(Parallel::LPT3)) {
        addVirtualNode("/dev/ports", new ParallelNode(parallelService->getParallelPort(Parallel::LPT3)));
    }

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

    // Add Memory Nodes to dev-directory
    createDirectory("/dev/memory");
    addVirtualNode("/dev/memory", new KernelHeapNode());
    addVirtualNode("/dev/memory", new IOMemoryNode());
    addVirtualNode("/dev/memory", new PFANode());
    addVirtualNode("/dev/memory", new PagingAreaNode());

    // Add PCI-node to dev-Directory
    addVirtualNode("/dev", new PciNode());

    // Add syslog file to dev-Directory
    createFile("/dev/syslog");

    // Add StdStream-nodes to dev-Directory
    addVirtualNode("/dev", new StdoutNode());
    addVirtualNode("/dev", new StderrNode());

    // Subscribe to Storage-Events from the EventBus
    eventBus->subscribe(*this, StorageAddEvent::TYPE);
    eventBus->subscribe(*this, StorageRemoveEvent::TYPE);

    FileAppender *fileAppender = new FileAppender(File::open("/dev/syslog", "a+"));

    Logger::addAppender(fileAppender);
}

uint32_t FileSystem::addVirtualNode(const String &path, VirtualNode *node) {

    fsLock.acquire();

    String parsedPath = parsePath(path);

    auto *driver = (RamFsDriver*) getMountedDriver(parsedPath);
    bool ret = driver->addNode(parsedPath, node);

    fsLock.release();

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

    fsLock.acquire();

    // Create temporary driver
    FsDriver *tmpDriver = nullptr;

    if(fsType == TYPE_FAT)
        tmpDriver = new FatDriver();
    else if(fsType == TYPE_RAM)
        tmpDriver = new RamFsDriver();
    else {
        fsLock.release();
        return INVALID_DRIVER;
    }

    // Format device
    bool ret = tmpDriver->createFs(disk);

    fsLock.release();

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

    String parsedPath = parsePath(targetPath) + "/";
    FsNode *targetNode = getNode(parsedPath);

    if(targetNode == nullptr) {
        if(mountPoints.size() != 0) {
            return FILE_NOT_FOUND;
        }
    } else {
        delete targetNode;
    }

    fsLock.acquire();

    if(mountPoints.containsKey(parsedPath)) {
        fsLock.release();
        return MOUNT_TARGET_ALREADY_USED;
    }

    FsDriver *driver = nullptr;

    if(type == TYPE_FAT) {
        driver = new FatDriver();
    } else if(type == TYPE_RAM) {
        driver = new RamFsDriver();
    } else {
        fsLock.release();
        return INVALID_DRIVER;
    }

    if(!driver->mount(disk)) {
        fsLock.release();
        delete driver;
        return MOUNTING_FAILED;
    }

    mountPoints.put(parsedPath, driver);

    fsLock.release();
    return SUCCESS;
}

uint32_t FileSystem::unmount(const String &path) {
    String parsedPath = parsePath(path) + "/";
    FsNode *targetNode = getNode(parsedPath);

    if(targetNode == nullptr) {
        if(path != "/") {
            return FILE_NOT_FOUND;
        }
    } else {
        delete targetNode;
    }

    fsLock.acquire();

    for(const String &key : mountPoints.keySet()) {
        if(key.beginsWith(parsedPath)) {
            if(key != parsedPath) {
                fsLock.release();
                return SUBDIRECTORY_CONTAINS_MOUNT_POINT;
            }
        }
    }

    if(mountPoints.containsKey(parsedPath)) {
        delete mountPoints.get(parsedPath);
        mountPoints.remove(parsedPath);

        fsLock.release();
        return SUCCESS;
    }

    fsLock.release();

    return NOTHING_MOUNTED_AT_PATH;
}

FsNode *FileSystem::getNode(const String &path) {
    String parsedPath = parsePath(path);

    fsLock.acquire();

    FsDriver *driver = getMountedDriver(parsedPath);

    if(driver == nullptr) {
        fsLock.release();
        return nullptr;
    }
    
    FsNode *ret = driver->getNode(parsedPath);

    fsLock.release();

    return ret;
}

uint32_t FileSystem::createFile(const String &path) {
    String parsedPath = parsePath(path);

    fsLock.acquire();

    FsDriver *driver = getMountedDriver(parsedPath);

    if(driver == nullptr) {
        fsLock.release();
        return FILE_NOT_FOUND;
    }
    
    bool ret = driver->createNode(parsedPath, FsNode::REGULAR_FILE);

    fsLock.release();

    return ret ? SUCCESS : CREATING_FILE_FAILED;
}

uint32_t FileSystem::createDirectory(const String &path) {
    String parsedPath = parsePath(path);
    fsLock.acquire();

    FsDriver *driver = getMountedDriver(parsedPath);

    if(driver == nullptr) {
        fsLock.release();
        return FILE_NOT_FOUND;
    }

    bool ret = driver->createNode(parsedPath, FsNode::DIRECTORY_FILE);

    fsLock.release();

    return ret ? SUCCESS : CREATING_DIRECTORY_FAILED;
}

uint32_t FileSystem::deleteFile(const String &path) {
    String parsedPath = parsePath(path);

    fsLock.acquire();

    for(const String &key : mountPoints.keySet()) {
        if(key.beginsWith(parsedPath)) {
            fsLock.release();
            return SUBDIRECTORY_CONTAINS_MOUNT_POINT;
        }
    }

    FsDriver *driver = getMountedDriver(parsedPath);

    if(driver == nullptr) {
        fsLock.release();
        return FILE_NOT_FOUND;
    }
    
    bool ret = driver->deleteNode(parsedPath);

    fsLock.release();

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
