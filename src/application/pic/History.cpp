//
// Created by Rafael Reip on 14.11.24.
//

#include "History.h"

using namespace Util;

History::History(MessageHandler *mHandler) {
    this->commands = new ArrayList<String>();
    this->currentCommand = 0;
    this->mHandler = mHandler;
}

void History::addCommand(const String &command) {
    if (command.length() > currentCommand) {
        for (int i = commands->size(); i > currentCommand; i--) {
            commands->removeIndex(i);
        }
    }
    commands->add(command);
    currentCommand = commands->size();
    if (currentCommand % SNAPSHOT_INTERVAL == 0) {
        // TODO
    }
}

void History::execCommandOn(Layers *layers, const String &command) {
    auto comm = command.split(";");
    if (comm.length() == 0) return;
    if (comm[0] == "addEmpty") {
        if (comm.length() != 5) mHandler->addMessage("Invalid command: " + command);
        else layers->addEmpty(String::parseInt(comm[1]), String::parseInt(comm[1]), String::parseInt(comm[1]), String::parseInt(comm[1]));
    } else if (comm[0] == "addPicture") {
        if (comm.length() != 4) mHandler->addMessage("Invalid command: " + command);
        else layers->addPicture(comm[1].operator const char *(), String::parseInt(comm[2]), String::parseInt(comm[3]));
    }
    else if (comm[0] == "delete") {
        if (comm.length() != 2) mHandler->addMessage("Invalid command: " + command);
        else layers->deletetAt(String::parseInt(comm[1]));
    }
    else if (comm[0] == "swap") {
        if (comm.length() != 3) mHandler->addMessage("Invalid command: " + command);
        else layers->swap(String::parseInt(comm[1]), String::parseInt(comm[2]));
    }
    else if (comm[0] == "visible") {
        if (comm.length() != 2) mHandler->addMessage("Invalid command: " + command);
        else layers->changeVisibleAt(String::parseInt(comm[1]));
    }
    else if (comm[0] == "combine") {
        if (comm.length() != 3) mHandler->addMessage("Invalid command: " + command);
        else layers->combine(String::parseInt(comm[1]), String::parseInt(comm[2]));
    }
    else if (comm[0] == "duplicate") {
        if (comm.length() != 2) mHandler->addMessage("Invalid command: " + command);
        else layers->duplicate(String::parseInt(comm[1]));
    }
    else if (comm[0] == "move") {
        if (comm.length() != 4) mHandler->addMessage("Invalid command: " + command);
        else layers->move(String::parseInt(comm[1]), String::parseInt(comm[2]), String::parseInt(comm[3]));
    }
    else if (comm[0] == "scale") {
        if (comm.length() != 4) mHandler->addMessage("Invalid command: " + command);
        else layers->scale(String::parseInt(comm[1]), String::parseDouble(comm[2]), static_cast<ToolCorner>(String::parseInt(comm[3])));
    }
    else if (comm[0] == "crop") {
        if (comm.length() != 5) mHandler->addMessage("Invalid command: " + command);
        else layers->crop(String::parseInt(comm[1]), String::parseInt(comm[2]), String::parseInt(comm[3]), String::parseInt(comm[4]), String::parseInt(comm[5]));
    }
    else mHandler->addMessage("Unknown command: " + command);
}

void History::printCommands() {
    for (int i = 0; i < commands->size(); i++) {
        mHandler->addMessage(commands->get(i));
    }
}
