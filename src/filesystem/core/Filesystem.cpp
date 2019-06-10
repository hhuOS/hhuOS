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

#include "kernel/event/storage/StorageAddEvent.h"
#include "kernel/event/storage/StorageRemoveEvent.h"
#include "kernel/log/FileAppender.h"
#include "kernel/log/Logger.h"
#include "device/input/Keyboard.h"
#include "kernel/service/InputService.h"
#include "kernel/multiboot/Structure.h"
#include "lib/file/tar/Archive.h"
#include "filesystem/tar/TarArchiveDriver.h"
#include "Filesystem.h"
#include "lib/file/Directory.h"
#include "filesystem/ram/RamFsDriver.h"
#include "filesystem/ram/nodes/StorageNode.h"
#include "filesystem/ram/nodes/PciNode.h"
#include "filesystem/ram/nodes/StdoutNode.h"
#include "filesystem/ram/nodes/StderrNode.h"

Logger &Filesystem::log = Logger::get("FILESYSTEM");

Filesystem::Filesystem() {
    eventBus = Kernel::getService<EventBus>();
    storageService = Kernel::getService<StorageService>();
}

Filesystem::~Filesystem() {
    eventBus->unsubscribe(*this, StorageAddEvent::TYPE);
    eventBus->unsubscribe(*this, StorageRemoveEvent::TYPE);
}

String Filesystem::parsePath(const String &path) {
    Util::Array<String> token = path.split(Filesystem::SEPARATOR);
    Util::ArrayList<String> parsedToken;

    for (const String &string : token) {
        if (string == ".") {
            continue;
        } else if (string == "..") {
            if(!parsedToken.isEmpty()) {
                parsedToken.remove(parsedToken.size() - 1);
            }
        } else {
            parsedToken.add(*new String(string));
        }
    }

    if (parsedToken.isEmpty()) {
        return "";
    }

    String parsedPath = Filesystem::SEPARATOR + String::join(Filesystem::SEPARATOR, parsedToken.toArray());
    return parsedPath;
}

FsDriver *Filesystem::getMountedDriver(String &path) {
    if(!path.endsWith(Filesystem::SEPARATOR)) {
        path += Filesystem::SEPARATOR;
    }

    String ret;

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

void Filesystem::init() {
    log.trace("Unmounting initial ramdisk");

    for(const String &path : mountPoints.keySet()) {
        delete mountPoints.get(path);
    }

    mountPoints.clear();

    FsDriver::registerPrototype(new RamFsDriver());

    // Mount root-device
    Util::Array<String> rootDeviceOptions = Multiboot::Structure::getKernelOption("root").split(",");

    if(rootDeviceOptions.length() >= 2) {
        if(mount(rootDeviceOptions[0], "/", rootDeviceOptions[1]) == SUCCESS) {
            log.info("Succesfully mounted root-device '%s'", (const char*) rootDeviceOptions[0]);
        } else {
            log.warn("Unable to mount root-device '%s'. Mounting RamFs to /", (const char*) rootDeviceOptions[0]);

            mount("", "/", "RamFsDriver");}
    } else {
        if(rootDeviceOptions.length() > 0) {
            log.warn("Invalid root-device configuration '%s'. Mounting RamFs to /",
                     (const char *) Multiboot::Structure::getKernelOption("root"));
        }

        mount("", "/", "RamFsDriver");
    }

    log.trace("Remounting initial ramdisk to /initrd/");

    createDirectory("/initrd");
    mountInitRamdisk("/initrd");

    log.trace("Initializing /dev");

    // Initialize dev-Directory
    Directory *dev = Directory::open("/dev");
    if(dev == nullptr) {
        createDirectory("/dev");
    } else {
        delete dev;
    }

    mount("", "/dev", "RamFsDriver");

    createDirectory("/dev/storage");
    createDirectory("/dev/ports");
    createDirectory("/dev/network");

    createFile("/dev/syslog");

    addVirtualNode("/dev", new PciNode());

    addVirtualNode("/dev", new StdoutNode());
    addVirtualNode("/dev", new StderrNode());

    // Subscribe to Storage-Events from the EventBus
    eventBus->subscribe(*this, StorageAddEvent::TYPE);
    eventBus->subscribe(*this, StorageRemoveEvent::TYPE);

    FileAppender *fileAppender = new FileAppender(File::open("/dev/syslog", "a+"));

    Logger::addAppender(fileAppender);
}

void Filesystem::mountInitRamdisk(const String &path) {
    Multiboot::ModuleInfo info = Multiboot::Structure::getModule("initrd");

    if(info.start == 0) {
        return;
    }

    Util::Address address(info.start);

    Tar::Archive &archive = Tar::Archive::from(address);

    FsDriver *tarDriver = new TarArchiveDriver(&archive);

    if(!path.endsWith(Filesystem::SEPARATOR)) {
        mountPoints.put(path + Filesystem::SEPARATOR, tarDriver);
    } else {
        mountPoints.put(path, tarDriver);
    }
}

uint32_t Filesystem::addVirtualNode(const String &path, VirtualNode *node) {

    fsLock.acquire();

    String parsedPath = parsePath(path);

    auto *driver = getMountedDriver(parsedPath);

    if(driver->getTypeName() != "RamFsDriver") {
        fsLock.release();

        return ADDING_VIRTUAL_NODE_FAILED;
    }

    bool ret = ((RamFsDriver*) driver)->addNode(parsedPath, node);

    fsLock.release();

    return ret ? SUCCESS : ADDING_VIRTUAL_NODE_FAILED;
}

uint32_t Filesystem::createFilesystem(const String &devicePath, const String &fsType) {
    // Check if device-node exists
    Util::SmartPointer<FsNode> deviceNode = getNode(devicePath);
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
    FsDriver *tmpDriver = (FsDriver*) FsDriver::createInstance(fsType);

    // Format device
    bool ret = tmpDriver->createFs(disk);

    fsLock.release();

    delete tmpDriver;
    return ret ? SUCCESS : FORMATTING_FAILED;
}

uint32_t Filesystem::mount(const String &devicePath, const String &targetPath, const String &fsType) {
    StorageDevice *disk = nullptr;
    String parsedDevicePath = parsePath(devicePath);

    if(fsType != TYPE_RAM_FS) {
        // Check if device-node exists
        Util::SmartPointer<FsNode> deviceNode = getNode(parsedDevicePath);

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

    String parsedPath = parsePath(targetPath) + Filesystem::SEPARATOR;
    Util::SmartPointer<FsNode> targetNode = getNode(parsedPath);

    if(targetNode == nullptr) {
        if(mountPoints.size() != 0) {
            return FILE_NOT_FOUND;
        }
    }

    fsLock.acquire();

    if(mountPoints.containsKey(parsedPath)) {
        fsLock.release();
        return MOUNT_TARGET_ALREADY_USED;
    }

    FsDriver *driver = (FsDriver*) FsDriver::createInstance(fsType);

    if(!driver->mount(disk)) {
        fsLock.release();
        delete driver;
        return MOUNTING_FAILED;
    }

    mountPoints.put(parsedPath, driver);

    fsLock.release();
    return SUCCESS;
}

uint32_t Filesystem::unmount(const String &path) {
    String parsedPath = parsePath(path) + Filesystem::SEPARATOR;
    Util::SmartPointer<FsNode> targetNode = getNode(parsedPath);

    if(targetNode == nullptr) {
        if(path != "/") {
            return FILE_NOT_FOUND;
        }
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

Util::SmartPointer<FsNode> Filesystem::getNode(const String &path) {
    String parsedPath = parsePath(path);

    fsLock.acquire();

    FsDriver *driver = getMountedDriver(parsedPath);

    if(driver == nullptr) {
        fsLock.release();
        return Util::SmartPointer<FsNode>(nullptr);
    }
    
    Util::SmartPointer<FsNode> ret = driver->getNode(parsedPath);

    fsLock.release();

    return ret;
}

uint32_t Filesystem::createFile(const String &path) {
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

uint32_t Filesystem::createDirectory(const String &path) {
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

uint32_t Filesystem::deleteFile(const String &path) {
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

void Filesystem::onEvent(const Event &event) {
    if(event.getType() == StorageAddEvent::TYPE) {
        StorageDevice *device = ((StorageAddEvent &) event).getDevice();

        deleteFile("/dev/storage/" + device->getName());
        addVirtualNode("/dev/storage/", new StorageNode(device));
    } else if(event.getType() == StorageAddEvent::TYPE) {
        String deviceName = ((StorageRemoveEvent &) event).getDeviceName();

        deleteFile("/dev/storage/" + deviceName);
    }
}
