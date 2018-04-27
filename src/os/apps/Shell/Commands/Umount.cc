/*
 * Copyright (C) 2018 Fabian Ruhland
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Umount.h"

Umount::Umount(Shell &shell) : Command(shell) {

};

void Umount::execute(Util::Array<String> &args) {
    Util::ArrayList<String> paths;

    for(uint32_t i = 1; i < args.length(); i++) {
        if(!args[i].beginsWith("-") || args[i] == "-") {
            paths.add(args[i]);
        } else if(args[i] == "-h" || args[i] == "--help") {
            stdout << "Unmounts devices from their mount paths." << endl << endl;
            stdout << "Usage: " << args[0] << " [PATH]..." << endl << endl;
            stdout << "Options:" << endl;
            stdout << "  -h, --help: Show this help-message." << endl;
            return;
        } else {
            stderr << args[0] << ": Invalid option '" << args[i] << "'!" << endl;
            return;
        }
    }

    if(paths.size() == 0) {
        stderr << args[0] << ": Missing operand!" << endl;
        return;
    }

    auto *fileSystem = Kernel::getService<FileSystem>();

    for(const String &path : paths) {
        String absolutePath = calcAbsolutePath(path);
        auto ret = fileSystem->unmount(absolutePath);

        switch (ret) {
            case FileSystem::SUCCESS :
                break;
            case FileSystem::FILE_NOT_FOUND :
                stderr << args[0] << ": '" << path << "': Directory not found!" << endl;
                break;
            case FileSystem::NOTHING_MOUNTED_AT_PATH :
                stderr << args[0] << ": '" << path << "': No device mounted at target path!" << endl;
                break;
            case FileSystem::SUBDIRECTORY_CONTAINS_MOUNT_POINT:
                stderr << args[0] << ": '" << path << "': A device is still mounted to a subdirectory!" << endl;
                break;
            default:
                break;
        }
    }
}