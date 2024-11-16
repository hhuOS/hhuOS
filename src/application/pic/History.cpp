//
// Created by Rafael Reip on 14.11.24.
//

#include "History.h"

using namespace Util;

History::History(MessageHandler *mHandler) {
    this->commands = new ArrayList<String>();
    this->currentCommand = 0;
    this->mHandler = mHandler;
    this->lines = new ArrayList<Util::String>();
}

void History::addCommand(const String &command) {
    if (command.length() > currentCommand) {
        for (int i = commands->size(); i > currentCommand; i--) {
            commands->removeIndex(i);
        }
    }
    if (command.beginsWith("line")) {
        lines->add(command.substring(5, command.length()));
    } else {
        if (command.beginsWith("prepareNextDrawing") && lines->size() > 0) {
            String linestogether = "line";
            for (int i = 0; i < lines->size(); i++) {
                linestogether += " " + lines->get(i);
            }
            commands->add(linestogether);
            lines->clear();
        }
        commands->add(command);
    }
    currentCommand = commands->size();
    if (currentCommand % SNAPSHOT_INTERVAL == 0) {
        // TODO make snapshot of layers
    }
}

void History::reset() {
    commands->clear();
    currentCommand = 0;
}

void History::execCommandOn(Layers *layers, const String &command) {
    auto comm = command.split(" ");
    if (comm.length() == 0) return;
    if (comm[0] == "addEmpty") {
        if (comm.length() != 5) mHandler->addMessage("Invalid command: " + command);
        else layers->addEmpty(String::parseInt(comm[1]), String::parseInt(comm[2]), String::parseInt(comm[3]), String::parseInt(comm[4]));
    } else if (comm[0] == "addPicture") {
        if (comm.length() != 4) mHandler->addMessage("Invalid command: " + command);
        else layers->addPicture(comm[1].operator const char *(), String::parseInt(comm[2]), String::parseInt(comm[3]));
    } else if (comm[0] == "delete") {
        if (comm.length() != 2) mHandler->addMessage("Invalid command: " + command);
        else layers->deletetAt(String::parseInt(comm[1]));
    } else if (comm[0] == "swap") {
        if (comm.length() != 3) mHandler->addMessage("Invalid command: " + command);
        else layers->swap(String::parseInt(comm[1]), String::parseInt(comm[2]));
    } else if (comm[0] == "visible") {
        if (comm.length() != 2) mHandler->addMessage("Invalid command: " + command);
        else layers->changeVisibleAt(String::parseInt(comm[1]));
    } else if (comm[0] == "combine") {
        if (comm.length() != 3) mHandler->addMessage("Invalid command: " + command);
        else layers->combine(String::parseInt(comm[1]), String::parseInt(comm[2]));
    } else if (comm[0] == "duplicate") {
        if (comm.length() != 2) mHandler->addMessage("Invalid command: " + command);
        else layers->duplicate(String::parseInt(comm[1]));
    } else if (comm[0] == "move") {
        if (comm.length() != 4) mHandler->addMessage("Invalid command: " + command);
        else layers->move(String::parseInt(comm[1]), String::parseInt(comm[2]), String::parseInt(comm[3]));
    } else if (comm[0] == "scale") {
        if (comm.length() != 4) mHandler->addMessage("Invalid command: " + command);
        else layers->scale(String::parseInt(comm[1]), String::parseDouble(comm[2]), static_cast<ToolCorner>(String::parseInt(comm[3])));
    } else if (comm[0] == "crop") {
        if (comm.length() != 6) mHandler->addMessage("Invalid command: " + command);
        else
            layers->crop(String::parseInt(comm[1]), String::parseInt(comm[2]), String::parseInt(comm[3]), String::parseInt(comm[4]),
                         String::parseInt(comm[5]));
    } else if (comm[0] == "autoCrop") {
        if (comm.length() != 2) mHandler->addMessage("Invalid command: " + command);
        else layers->autoCrop(String::parseInt(comm[1]));
    } else if (comm[0] == "rotate") {
        if (comm.length() != 3) mHandler->addMessage("Invalid command: " + command);
        else layers->rotate(String::parseInt(comm[1]), String::parseInt(comm[2]));
    } else if (comm[0] == "line") {
        if ((comm.length() - 1) % 7 != 0) mHandler->addMessage("Invalid command: " + command);
        else {
            for (int i = 1; i < comm.length(); i += 7) {
                layers->drawLine(String::parseInt(comm[i]), String::parseInt(comm[i + 1]), String::parseInt(comm[i + 2]),
                                 String::parseInt(comm[i + 3]),
                                 String::parseInt(comm[i + 4]), String::parseInt(comm[i + 5]), String::parseInt(comm[i + 6]));
            }
        }

    } else if (comm[0] == "prepareNextDrawing") {
        if (comm.length() != 2) mHandler->addMessage("Invalid command: " + command);
        else layers->prepareNextDrawing(String::parseInt(comm[1]));
    } else if (comm[0] == "shape") {
        if (comm.length() != 8) mHandler->addMessage("Invalid command: " + command);
        else
            layers->drawShape(String::parseInt(comm[1]), static_cast<Shape>(String::parseInt(comm[2])), String::parseInt(comm[3]),
                              String::parseInt(comm[4]), String::parseInt(comm[5]), String::parseInt(comm[6]), String::parseInt(comm[7]));
    } else mHandler->addMessage("Unknown command: " + command);
}

void History::printCommands() {
    for (int i = 0; i < commands->size(); i++) {
        mHandler->addMessage(commands->get(i));
    }
}

void History::saveToFile(const Util::String &path) {
    FILE *file = fopen(path.operator const char *(), "w");
    if (file) {
        for (int i = 0; i < commands->size(); i++) {
            fputs(commands->get(i).operator const char *(), file);
            fputs("\n", file);
        }
        fclose(file);
    }
}

void History::loadFromFileInto(Layers *layers, const Util::String &path) {
    auto file = Util::Io::File(path);
    if (!file.exists()) {
        mHandler->addMessage("Error: File not found: " + path);
        return;
    }
    if (file.isDirectory()) {
        mHandler->addMessage("Error: File is a directory: " + path);
        return;
    }

    layers->reset();
    this->reset();

    auto fileStream = Util::Io::FileInputStream(file);
    auto bufferedStream = Util::Io::BufferedInputStream(fileStream);
    auto &stream = (file.getType() == Util::Io::File::REGULAR) ? static_cast<Util::Io::InputStream &>(bufferedStream)
                                                               : static_cast<Util::Io::InputStream &>(fileStream);

    bool eof = false;
    auto line = stream.readLine(eof);
    while (!eof) {
        if (line.length() > 0) {
            print(line);
            execCommandOn(layers, line);
        }
        line = stream.readLine(eof);
    }
}

void History::undo(Layers *layers) {
    // TODO
}

void History::redo(Layers *layers) {
    // TODO
}
