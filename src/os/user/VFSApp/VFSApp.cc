/*****************************************************************************
 *                                                                           *
 *                             V B E D E M O                                 *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Demo zu VESA.                                            *
 *                                                                           *
 * Autor:           Michael Schoettner, HHU, 26.12.2016                      *
 *****************************************************************************/

#include "user/VFSApp/VFSApp.h"
#include "devices/input/Key.h"
#include "lib/file/Directory.h"
#include "lib/file/File.h"
#include "lib/elf/ElfLoader.h"
#include "devices/graphics/text/CgaText.h"
#include "devices/graphics/text/VesaText.h"
#include "devices/block/storage/Partition.h"
#include "kernel/services/StorageService.h"
#include <cstdint>
#include <cstddef>
#include <kernel/services/ModuleLoader.h>
#include <devices/Pit.h>

extern "C" {
    #include "lib/libc/string.h"
}

/// @todo #include "lib/libc/printf.h" or #include <printf> ??
#include "lib/libc/printf.h"
#include "lib/Bmp.h"

Directory *cwd;
char currentCommand[1024];
uint32_t counter = 0;

char *appendPathToCwd(char *path) {

    String tmp = cwd->getAbsolutePath();

    if (!tmp.endsWith(FileSystem::SEPARATOR)) {
        tmp += FileSystem::SEPARATOR;
    }

    tmp += path;

    char *ret = new char[tmp.length() + 1];

    memcpy(ret, (char*) tmp, tmp.length() + 1);

    return ret;
}

namespace BmpCommand {
    
    static const char * buzz() {
        return "bmp";
    }
    
    static const char * help() {
        return "bmp [path] [x] [y] - prints a bmp file on position x,y";
    }
    
    static unsigned int minpara() {
        return 4;
    }
    
    static void start(char *path, File *file, char **params) {
        uint32_t x = strtoint(params[2]);
        uint32_t y = strtoint(params[3]);
        
        Bmp * bmp = new Bmp(file);
        bmp->print(x, y);
        
        delete bmp;
        delete file;
        delete path;
    }
};

void VFSApp::executeCommand() {
    TextDriver *stream = graphicsService->getTextDriver();
    FileSystem *fileSystem = Kernel::getService<FileSystem>();
    
    uint32_t paramCount = 0;
    char *ptr = strtok(currentCommand, " ");
    while(ptr != NULL) {
        paramCount++;
        ptr = strtok(NULL, " ");
    }

    if(paramCount == 0)
        return;

    char **params = new char *[paramCount];

    ptr = strtok(currentCommand, " ");
    for(uint32_t i = 0; i < paramCount; i++) {
            params[i] = new char[strlen(ptr) + 1];
            strcpy(params[i], ptr);
        ptr = strtok(NULL, " ");
    }

    if(!strcmp(params[0], "help")) {
        *stream << "ls [optional: path] - Lists all files in a directory" << endl;
        *stream << "cd [path] - Changes the current working directory" << endl;
        *stream << "mkdir [path] - Creates a directory" << endl;
        *stream << "touch [path] - Creates a file" << endl;
        *stream << "rm [path] - Deletes a file or directory" << endl;
        *stream << "read [path] [optional: amount] - Reads the content of a file" << endl;
        *stream << "write [path] [offset] [text] - Writes text into a file at the specified offset" << endl;
        *stream << "append [path] [text] - Appends text to a file" << endl;
        *stream << "mount [optional: device] [path] [type] - Mounts a device with a given filetype to the specified path" << endl;
        *stream << "driver [cga/lfb] - Change the driver, that is used to display text" << endl;
        *stream << "res [columns] [rows] [optional: depth] - Changes the screen resolution" << endl;
        printf("%s\n", BmpCommand::help());
        
    } else if(!strcmp(params[0], "clear")) {
        stream->clear();
    } else if(!strcmp(params[0], "ls")) {
        Directory *dir;
        if(paramCount == 1)
            dir = cwd;
        else {
            char *path;
                if(params[1][0] == '/') {
                path = new char[strlen(params[1]) + 1];
                strcpy(path, params[1]);
            } else path = appendPathToCwd(params[1]);
            dir = Directory::open(path);
        }

        if(dir == nullptr) {
            *stream << "Directory not found!" << endl;
            return;
        }

        for(const String &entry : dir->getChildren()) {
            *stream << entry;

            Directory *tmp = Directory::open(entry);

            if(tmp != nullptr) *stream << "/";

            delete tmp;

            *stream << endl;
        }

        if(dir != cwd) {
            delete dir;
        }

    } else if(!strcmp(params[0], "cd")) {
        if(paramCount < 2) {
            *stream << "Too few arguments!" << endl;
            return;
        }

        char *path;
        if(params[1][0] == '/') {
            path = new char[strlen(params[1]) + 1];
            strcpy(path, params[1]);
        } else path = appendPathToCwd(params[1]);

        Directory *dir = Directory::open(path);
        if(dir == nullptr) {
            *stream << "Directory not found!" << endl;
            return;
        }

        delete cwd;
        cwd = dir;
        delete path;
    } else if(!strcmp(params[0], "mkdir")) {
        if(paramCount < 2) {
            *stream << "Too few arguments!" << endl;
            return;
        }

        char *path;
        if(params[1][0] == '/') {
            path = new char[strlen(params[1]) + 1];
            strcpy(path, params[1]);
        } else path = appendPathToCwd(params[1]);

        if(fileSystem->createDirectory(path)) {
            *stream << "Error!";
        }

        delete path;
    } else if(!strcmp(params[0], "touch")) {
        if(paramCount < 2) {
            *stream << "Too few arguments!" << endl;
            return;
        }

        char *path;
        if(params[1][0] == '/') {
            path = new char[strlen(params[1]) + 1];
            strcpy(path, params[1]);
        } else path = appendPathToCwd(params[1]);

        if(fileSystem->createFile(path)) {
            *stream << "Error!";
        }

        delete path;
    } else if(!strcmp(params[0], "rm")) {
        if(paramCount < 2) {
            *stream << "Too few arguments!" << endl;
            return;
        }

        char *path;
        if(params[1][0] == '/') {
            path = new char[strlen(params[1]) + 1];
            strcpy(path, params[1]);
        } else path = appendPathToCwd(params[1]);

        if(fileSystem->deleteFile(path)) {
            *stream << "Error!";
        }

        delete path;
    } else if(!strcmp(params[0], "read")) {
        if(paramCount < 2) {
            *stream << "Too few arguments!" << endl;
            return;
        }
        
        char *path;
        if(params[1][0] == '/') {
            path = new char[strlen(params[1]) + 1];
            strcpy(path, params[1]);
        } else path = appendPathToCwd(params[1]);

        File *file = File::open(path, "r");
        if(file == nullptr) {
            *stream << "File not found!" << endl;
            return;
        }
        
        if(paramCount < 3) {
            *file >> *stream;
        } else {
            uint32_t length = strtoint(params[2]);
            char *buf = new char[length];
            if(file->readBytes(buf, length) == 0) {
                *stream << "Cannot read file!";
                return;
            }

            for(uint32_t i = 0; i < length; i++) {
                stream->putc(buf[i]);
            }

            delete buf;
        }

        delete path;
        delete file;
    } else if(!strcmp(params[0], BmpCommand::buzz())) {
        if(paramCount < BmpCommand::minpara()) {
            *stream << "Too few arguments!" << endl;
            return;
        }
        
        char *path;
        if(params[1][0] == '/') {
            path = new char[strlen(params[1]) + 1];
            strcpy(path, params[1]);
        } else path = appendPathToCwd(params[1]);

        File *file = File::open(path, "r");
        if(file == nullptr) {
            *stream << "File not found!" << endl;
            return;
        }
        
        BmpCommand::start(path, file, params);

    } else if(!strcmp(params[0], "write")) {
        if(paramCount < 4) {
            *stream << "Too few arguments!" << endl;
            return;
        }
        
        char *path;
        if(params[1][0] == '/') {
            path = new char[strlen(params[1]) + 1];
            strcpy(path, params[1]);
        } else path = appendPathToCwd(params[1]);

        File *file = File::open(path, "w");
        if(file == nullptr) {
            *stream << "File not found!" << endl;
            return;
        }
        
        char *string = &currentCommand[strlen(params[0]) + strlen(params[1]) + strlen(params[2]) + 3];
        uint32_t offset = strtoint(params[2]);
        file->setPos(offset, File::START);
        //file->writeBytes(string, strlen(string));
        *file << string;
        file->flush();

        delete path;
        delete file;
    } else if(!strcmp(params[0], "append")) {
        if(paramCount < 3) {
            *stream << "Too few arguments!" << endl;
            return;
        }
        
        char *path;
        if(params[1][0] == '/') {
            path = new char[strlen(params[1]) + 1];
            strcpy(path, params[1]);
        } else path = appendPathToCwd(params[1]);

        File *file = File::open(path, "a");
        if(file == nullptr) {
            *stream << "File not found!" << endl;
            return;
        }
        
        char *string = &currentCommand[strlen(params[0]) + strlen(params[1]) + 2];
        file->writeBytes(string, strlen(string));

        delete path;
        delete file;
    } else if(!strcmp(params[0], "mount")) {
        if(paramCount < 3) {
            *stream << "Too few arguments!" << endl;
        }

        if(paramCount == 3) {
            char *path;
            if(params[1][0] == '/') {
                path = new char[strlen(params[1]) + 1];
                strcpy(path, params[1]);
            } else path = appendPathToCwd(params[1]);

            if(fileSystem->mount("", path, params[2]))
                *stream << "Error!" << endl;
            
            delete path;
        } else {
            char *path;
            if(params[2][0] == '/') {
                path = new char[strlen(params[2]) + 1];
                strcpy(path, params[2]);
            } else path = appendPathToCwd(params[2]);

            if(fileSystem->mount(params[1], path, params[3]))
                *stream << "Error!" << endl;
            
            delete path;
        }
    } else if(!strcmp(params[0], "unmount")) {
        if(paramCount < 2) {
            *stream << "Too few arguments!" << endl;
        }

        char *path;
        if(params[1][0] == '/') {
            path = new char[strlen(params[1]) + 1];
            strcpy(path, params[1]);
        } else path = appendPathToCwd(params[1]);

        if(fileSystem->unmount(path))
            *stream << "Error!" << endl;
        
        delete path;
    } else if(!strcmp(params[0], "driver")) {
        if(paramCount < 2) {
            *stream << "Too few arguments!" << endl;
        }

        uint16_t columns = stream->getColumnCount();
        uint16_t rows = stream->getRowCount();
        uint8_t depth = stream->getDepth();

        if(!strcmp(params[1], "cga")) {
            stream = new CgaText();
            stream->init(columns, rows, depth);
            stream->setpos(0, 0);
            graphicsService->setTextDriver(stream);
        } else if(!strcmp(params[1], "vesa")) {
            stream = new VesaText();
            stream->init(columns, rows, depth);
            stream->setpos(0, 0);
            graphicsService->setTextDriver(stream);
        } else {
            *stream << "Invalid argument!" << endl;
        }
    } else if(!strcmp(params[0], "res")) {
        if(paramCount < 3) {
            *stream << "Too few arguments!" << endl;
        }

        uint16_t columns = strtoint(params[1]);
        uint16_t rows = strtoint(params[2]);
        uint8_t depth = paramCount > 3 ? strtoint(params[3]) : stream->getDepth();

        stream->init(columns, rows, depth);
        stream->setpos(0, 0);
    } else if (!strcmp(params[0], "insmod")) {
        if (paramCount != 2) {
            printf("Too few arguments!1");
        } else {
            char *path;
            if(params[1][0] == '/') {
                path = new char[strlen(params[1]) + 1];
                strcpy(path, params[1]);
            } else path = appendPathToCwd(params[1]);

            File *module = File::open(path, "r");

            if (module != nullptr) {
                ModuleLoader::Status status = moduleLoader->load(module);

                if (status == ModuleLoader::Status::OK) {
                    printf("Inserted module %s", params[1]);
                } else {
                    printf("Couldn't insert module %s", params[1]);
                }
            }
        }
    } else {
        char *path;
        if(params[0][1] == '/') {
            path = new char[strlen(params[0]) + 1];
            strcpy(path, params[1]);
        } else path = appendPathToCwd(params[0]);

        File *prog = File::open(path, "r");
        if(prog != nullptr) {
            ElfLoader loader;
            loader.load(prog, paramCount - 1, params + 1);
            delete prog;
        } else
            *stream << "Command not found!" << endl;
        
        delete path;
    }
    
    if(paramCount > 0) {
        for(uint32_t i = 0; i < paramCount; i++) {
            delete params[i];
        }
    }

}

/*****************************************************************************
 * Methode:         VBEdemo::run                                             *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Der Anwendungsthread erzeugt drei Threads die Zaehler    *
 *                  ausgeben und terminiert sich selbst.                     *
 *****************************************************************************/
void VFSApp::run () {
    TextDriver *stream = graphicsService->getTextDriver();

    stream->setpos(0,0);

    cwd = Directory::open("/");

    *stream << "Welcome to the VFS-Shell! This demo lets you interact with hhuOS' virtual file system." << endl;
    *stream << "Enter 'help' for a list of available commands!" << endl;

    *stream << endl << "[" << cwd->getName() << "] >";
    stream->flush();
    eventBus->subscribe(*this, KeyEvent::TYPE);
    while(true);
}

void VFSApp::onEvent(const Event &event) {
    TextDriver *stream = graphicsService->getTextDriver();
    Key key = ((KeyEvent &) event).getKey();
    if (key.valid()) {
        if(key.ascii() == '\n') {
            currentCommand[counter] = 0;
            counter = 0;
            *stream << endl;
            executeCommand();

            // Driver might have been changed
            stream = graphicsService->getTextDriver();

            *stream << endl << "[" << cwd->getName() << "] >";
            stream->flush();
        } else if(key.ascii() == '\b') {
            if(counter > 0) {
                uint16_t x, y;
                stream->getpos(x, y);
                stream->show(x, y, ' ', Colors::BLACK, Colors::BLACK);
                stream->show(--x, y, ' ', Colors::BLACK, Colors::BLACK);
                stream->setpos(x, y);
                currentCommand[counter] = 0;
                counter--;
            }
        } else {
            currentCommand[counter] = key.ascii();
            counter++;
            *stream << key.ascii();
            stream->flush();
        }
    }
}
