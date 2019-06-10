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

#ifndef __Shell_include__
#define __Shell_include__

#include "kernel/service/GraphicsService.h"
#include "kernel/service/EventBus.h"
#include "lib/file/Directory.h"
#include "kernel/service/KernelStreamService.h"
#include "lib/stream/AnsiOutputStream.h"

class Command;
class History;

/**
 * A simple UNIX-like Shell.
 *
 * @author Fabian Ruhland
 * @date 2018
 */
class Shell : public Thread, public Receiver, public AnsiOutputStream, public InputStream {

private:
    friend History;

    enum HistoryDirection {
        UP, DOWN
    };

    KernelStreamService *kernelStreamService;
    GraphicsService *graphicsService;
    EventBus *eventBus;

    Util::HashMap<String, Command*> commands;
    Util::ArrayList<String> history;

    Directory *cwd = nullptr;

    Spinlock inputLock;

    Color fgColor = Colors::TERM_WHITE;

    Color bgColor = Colors::TERM_BLACK;

    bool execute = false;
    bool isRunning = true;

    bool charAvailable = false;
    bool stringAvailable = false;

    char lastChar;
    String lastString;

    char inputBuffer[4096];

    uint16_t currentBase = 0;

    uint32_t historyIndex = 0;

    void showHistory(HistoryDirection direction);

    /**
     * Parse the user input and execute the respective command if the input is valid.
     *
     * @param input The user input
     */
    void executeCommand(String input);

public:

    /**
     * Constructor.
     */
    Shell();

    /**
     * Copy-constructor.
     */
    Shell(const Shell &copy) = delete;

    /**
     * Destructor.
     */
    ~Shell() override;

    /**
     * Get the current working directory.
     */
    Directory &getCurrentWorkingDirectory();

    /**
     * Set the current working directory.
     */
    void setCurrentWorkingDirectory(Directory *cwd);

    /**
     * Overriding function from Thread.
     */
    void run() override;

    /**
     * Overriding function from Receiver.
     */
    void onEvent(const Event &event) override;

    void flush() override;

    InputStream& operator >> (char &c) override;

    InputStream& operator >> (char *&string) override;

    InputStream& operator >> (OutputStream &outStream) override;
};

#endif