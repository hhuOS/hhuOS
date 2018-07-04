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

#include "Shell.h"
#include <kernel/events/input/KeyEvent.h>
#include <apps/Shell/Commands/Echo.h>
#include <apps/Shell/Commands/Cat.h>
#include <lib/file/File.h>
#include <apps/Application.h>
#include <kernel/threads/Scheduler.h>
#include <apps/Shell/Commands/Ls.h>
#include <apps/Shell/Commands/Touch.h>
#include <apps/Shell/Commands/Mkdir.h>
#include <apps/Shell/Commands/Rm.h>
#include <apps/Shell/Commands/Cd.h>
#include <apps/Shell/Commands/Clear.h>
#include <apps/Shell/Commands/Insmod.h>
#include <apps/Shell/Commands/Mount.h>
#include <apps/Shell/Commands/Umount.h>
#include <apps/Shell/Commands/Head.h>
#include <apps/Shell/Commands/Tail.h>
#include <apps/Shell/Commands/Uptime.h>
#include <apps/Shell/Commands/Date.h>
#include <apps/Shell/Commands/History.h>
#include <kernel/events/input/SerialEvent.h>
#include <apps/Shell/Commands/ComConfig.h>
#include <apps/Shell/Commands/LptConfig.h>
#include <apps/Shell/Commands/Mkfs.h>
#include <apps/Shell/Commands/DiskInfo.h>
#include <apps/Shell/Commands/AddPart.h>
#include <apps/Shell/Commands/MkPartTable.h>

Shell::Shell() : Thread("Shell") {
    stdStreamService = Kernel::getService<StdStreamService>();
    graphicsService = Kernel::getService<GraphicsService>();
    eventBus = Kernel::getService<EventBus>();

    stdStreamService->setStdout(this);
    stdStreamService->setStderr(this);
    stdStreamService->setStdin(this);

    commands.put("history", new History(*this));
    commands.put("clear", new Clear(*this));
    commands.put("cd", new Cd(*this));
    commands.put("echo", new Echo(*this));
    commands.put("cat", new Cat(*this));
    commands.put("head", new Head(*this));
    commands.put("tail", new Tail(*this));
    commands.put("ls", new Ls(*this));
    commands.put("touch", new Touch(*this));
    commands.put("mkdir", new Mkdir(*this));
    commands.put("rm", new Rm(*this));
    commands.put("insmod", new Insmod(*this));
    commands.put("mount", new Mount(*this));
    commands.put("umount", new Umount(*this));
    commands.put("mkparttable", new MkPartTable(*this));
    commands.put("addpart",  new AddPart(*this));
    commands.put("mkfs", new Mkfs(*this));
    commands.put("diskinfo", new DiskInfo(*this));
    commands.put("uptime", new Uptime(*this));
    commands.put("date", new Date(*this));
    commands.put("comconfig", new ComConfig(*this));
    commands.put("lptconfig", new LptConfig(*this));

    memset(inputBuffer, 0, sizeof(inputBuffer));
}

Shell::~Shell() {
    for(const auto &string : commands.keySet()) {
        delete commands.get(string);
    }
}

Directory &Shell::getCurrentWorkingDirectory() {
    return *cwd;
}

void Shell::setCurrentWorkingDirectory(Directory *cwd) {
    delete this->cwd;
    this->cwd = cwd;
}

void Shell::run() {
    cwd = Directory::open("/");

    *this << "Welcome to the hhuOS-Shell! Enter 'help' for a list of all available commands." << endl;
    *this << "[/]$ ";
    this->flush();

    eventBus->subscribe(*this, KeyEvent::TYPE);
    eventBus->subscribe(*this, SerialEvent::TYPE);

    while(isRunning) {
        char* input = nullptr;
        *this >> input;

        history.add(input);
        executeCommand(input);

        delete input;

        *this << "[" << (cwd->getName().isEmpty() ? "/" : cwd->getName()) << "]$ ";
        this->flush();
    }

    Application::getInstance()->resume();
    Scheduler::getInstance()->exit();
}

void Shell::executeCommand(String input) {
    OutputStream *stream = this;

     Util::Array<String> tmp = input.split(">");

    if(tmp.length() == 0) {
        return;
    }

    Util::Array<String> args = String(tmp[0]).split(" ");

    if(args.length() == 0) {
        return;
    }

    if(!commands.containsKey(args[0]) && args[0] != "help" && args[0] != "exit") {
        *this << "Shell: '" << args[0] << "': Command not found!" << endl;
        return;
    }

    if(tmp.length() > 1) {
        // Command shall output to file
        String relativePath = tmp[1];

        // Remove heading whitespaces
        while(relativePath.beginsWith(" ")) {
            relativePath = relativePath.substring(1, relativePath.length());
        }

        // Assemble absolute path
        String absolutePath;

        if(relativePath.beginsWith("/")) {
            // User has already entered an absolute path
            // We don't need to do anything
            absolutePath = relativePath;
        } else {
            // Append relative path to current working directory
            absolutePath = cwd->getAbsolutePath();

            if (!absolutePath.endsWith(FileSystem::SEPARATOR)) {
                absolutePath += FileSystem::SEPARATOR;
            }

            absolutePath += relativePath;
        }

        // Try to open the output file
        File *file = File::open(absolutePath, "w");
        if(file == nullptr) {
            *this << "Shell: '" << relativePath << "': File or Directory not found!" << endl;
            return;
        }

        stream = file;
    }

    if(args[0] == "help") {
        for(const String &command : commands.keySet()) {
            *stream << command << endl;
        }

        *stream << "help" << endl;
        *stream << "exit" << endl;

        return;
    } else if(args[0] == "exit") {
        isRunning = false;
        eventBus->unsubscribe(*this, KeyEvent::TYPE);
        eventBus->unsubscribe(*this, SerialEvent::TYPE);

        return;
    }

    stdStreamService->setStdout(stream);
    stdStreamService->setStderr(stream);

    commands.get(args[0])->execute(args);

    stdStreamService->setStdout(this);
    stdStreamService->setStderr(this);

    if(stream != this) {
        delete stream;
    }
}

void Shell::onEvent(const Event &event) {
    TextDriver &stream = *graphicsService->getTextDriver();
    char c;

    if(event.getType() == KeyEvent::TYPE) {
        Key key = ((KeyEvent &) event).getKey();

        if(key.valid()) {
            c = key.ascii();
        } else {
            return;
        };
    } else if(event.getType() == SerialEvent::TYPE) {
        c = ((SerialEvent &) event).getChar();
    } else {
        return;
    }

    if(strlen(inputBuffer) == (sizeof(inputBuffer) - 1) && c != '\b') {
        return;
    }

    inputLock.acquire();

    if(c == '\n' || c == 13) {
        inputBuffer[strlen(inputBuffer)] = 0;
        stream << endl;

        lastString = String(inputBuffer);
        stringAvailable = true;

        memset(inputBuffer, 0, sizeof(inputBuffer));
    } else if(c == '\b' || c == 127) {
        if(strlen(inputBuffer) > 0) {
            uint16_t x, y;
            stream.getpos(x, y);
            stream.show(x, y, ' ', Colors::BLACK, Colors::BLACK);
            stream.show(--x, y, ' ', Colors::BLACK, Colors::BLACK);
            stream.setpos(x, y);

            memset(&inputBuffer[strlen(inputBuffer) - 1], 0, sizeof(inputBuffer) - (strlen(inputBuffer) - 1));
        }
    } else {
        lastChar = c;
        charAvailable = true;

        inputBuffer[strlen(inputBuffer)] = c;
        stream << c;
        stream.flush();
    }

    inputLock.release();
}

void Shell::flush() {
    graphicsService->getTextDriver()->puts(StringBuffer::buffer, StringBuffer::pos);
    StringBuffer::pos = 0;
}

InputStream &Shell::operator>>(char &c) {
    while(true) {
        inputLock.acquire();

        if(charAvailable) {
            charAvailable = false;
            c = lastChar;

            inputLock.release();
            return *this;
        }

        inputLock.release();
    }
}

InputStream &Shell::operator>>(char *&string) {
    while(true) {
        inputLock.acquire();

        if(stringAvailable) {
            stringAvailable = false;

            string = new char[lastString.length() + 1];
            memcpy(string, (char *) lastString, lastString.length() + 1);

            inputLock.release();
            return *this;
        }

        inputLock.release();
    }
}

InputStream &Shell::operator>>(OutputStream &outStream) {
    char *string = nullptr;

    *this >> string;
    outStream << string;
    outStream.flush();

    delete string;
    return *this;
}
