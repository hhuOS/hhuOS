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

#include "Shell.h"
#include <kernel/events/input/KeyEvent.h>
#include <user/Shell/Commands/Echo.h>
#include <user/Shell/Commands/Cat.h>
#include <lib/file/File.h>
#include <user/Shell/Commands/Ls.h>
#include <user/Shell/Commands/Touch.h>
#include <user/Shell/Commands/Mkdir.h>
#include <user/Shell/Commands/Rm.h>
#include <user/Shell/Commands/Cd.h>
#include <user/Shell/Commands/Clear.h>
#include <user/Shell/Commands/Insmod.h>
#include <user/Shell/Commands/Mount.h>
#include <user/Shell/Commands/Umount.h>
#include <user/Application.h>
#include <kernel/threads/Scheduler.h>

Shell::Shell() : Thread("Shell") {
    stdStreamService = Kernel::getService<StdStreamService>();
    graphicsService = Kernel::getService<GraphicsService>();
    eventBus = Kernel::getService<EventBus>();

    commands.put("clear", new Clear(*this));
    commands.put("cd", new Cd(*this));
    commands.put("echo", new Echo(*this));
    commands.put("cat", new Cat(*this));
    commands.put("ls", new Ls(*this));
    commands.put("touch", new Touch(*this));
    commands.put("mkdir", new Mkdir(*this));
    commands.put("rm", new Rm(*this));
    commands.put("insmod", new Insmod(*this));
    commands.put("mount", new Mount(*this));
    commands.put("umount", new Umount(*this));

    memset(input, 0, sizeof(input));
}

Shell::~Shell() {
    eventBus->unsubscribe(*this, KeyEvent::TYPE);
}

Directory &Shell::getCurrentWorkingDirectory() {
    return *cwd;
}

void Shell::setCurrentWorkingDirectory(Directory *cwd) {
    delete this->cwd;
    this->cwd = cwd;
}

void Shell::run() {
    stdStreamService->setStdout(this);
    stdStreamService->setStderr(this);
    cwd = Directory::open("/");

    *this << "Welcome to the hhuOS-Shell! Enter 'help' for a list of all available commands." << endl;
    *this << "[/]$ ";
    this->flush();

    eventBus->subscribe(*this, KeyEvent::TYPE);

    while(isRunning) {
        if(execute) {
            execute = false;
            eventBus->unsubscribe(*this, KeyEvent::TYPE);

            executeCommand();

            memset(input, 0, sizeof(input));

            *this << "[" << (cwd->getName().isEmpty() ? "/" : cwd->getName()) << "]$ ";
            this->flush();

            eventBus->subscribe(*this, KeyEvent::TYPE);
        }
    }

    Application::getInstance()->resume();
    Scheduler::getInstance()->exit();
}

void Shell::executeCommand() {
    OutputStream *stream = this;

    Util::Array<String> tmp = String(input).split(">");

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

        return;
    } else if(args[0] == "exit") {
        isRunning = false;
        eventBus->unsubscribe(*this, KeyEvent::TYPE);

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
    Key key = ((KeyEvent &) event).getKey();

    if (key.valid()) {
        if(strlen(input) == (sizeof(input) - 1) && key.ascii() != '\b') {
            return;
        }

        if(key.ascii() == '\n') {
            input[strlen(input)] = 0;
            stream << endl;

            execute = true;
        } else if(key.ascii() == '\b') {
            if(strlen(input) > 0) {
                uint16_t x, y;
                stream.getpos(x, y);
                stream.show(x, y, ' ', Colors::BLACK, Colors::BLACK);
                stream.show(--x, y, ' ', Colors::BLACK, Colors::BLACK);
                stream.setpos(x, y);

                memset(&input[strlen(input) - 1], 0, sizeof(input) - (strlen(input) - 1));
            }
        } else {
            input[strlen(input)] = key.ascii();
            stream << key.ascii();
            stream.flush();
        }
    }
}

void Shell::flush() {
    graphicsService->getTextDriver()->puts(StringBuffer::buffer, StringBuffer::pos);
    StringBuffer::pos = 0;
}