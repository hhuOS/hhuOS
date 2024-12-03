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
    this->snapshots = new ArrayList<Pair<Layer **, int>>();
}

void History::addCommand(const String &command, Layer ***layers, int *layerCount) {
    if (commands->size() > currentCommand - 1) {
        for (uint32_t i = commands->size() - 1; i > currentCommand - 1; i--) {
            commands->removeIndex(i);
        }
        if (snapshots->size() > 0) {
            for (uint32_t i = snapshots->size(); i > currentCommand / SNAPSHOT_INTERVAL; i--) {
                auto *snapshot = snapshots->get(i - 1).first;
                for (int j = 0; j < snapshots->get(i - 1).second; j++) {
                    delete[] snapshot[j]->getPixelData();
                    delete snapshot[j];
                }
                delete[] snapshot;
                snapshots->removeIndex(i - 1);
            }
        }
    }
    bool added = false;
    if (command.beginsWith("line")) { // special case for lines
        lines->add(command.substring(5, command.length()));
    } else if (command.beginsWith("prepareNextDrawing")) {
        if (lines->size() > 0) {
            String linestogether = "line";
            for (int i = 0; i < lines->size(); i++) {
                linestogether += " " + lines->get(i);
            }
            commands->add(linestogether);
            currentCommand = commands->size();
            lines->clear();
            added = true;
        }
    } else {
        commands->add(command);
        currentCommand = commands->size();
        added = true;
    }
    if (added && currentCommand % SNAPSHOT_INTERVAL == 0) {
        auto **snapshot = new Layer *[18];
        for (int i = 0; i < *layerCount; i++) {
            auto *newPixelData = new uint32_t[(*layers)[i]->width * (*layers)[i]->height];
            for (int j = 0; j < (*layers)[i]->width * (*layers)[i]->height; j++) {
                newPixelData[j] = (*layers)[i]->getPixelData()[j];
            }
            snapshot[i] = new Layer((*layers)[i]->width, (*layers)[i]->height, (*layers)[i]->posX, (*layers)[i]->posY, 1, newPixelData);
        }
        snapshots->add(Pair<Layer **, int>(snapshot, *layerCount));
    }
}

void History::reset() {
    commands->clear();
    currentCommand = 0;
}

void History::execCommandOn(Layers *layers, const String &command, bool writeHistory) {
    if (!writeHistory) currentCommand++;
    auto comm = command.split(" ");
    if (comm.length() == 0) return;
    if (comm[0] == "addEmpty") {
        if (comm.length() != 5) mHandler->addMessage("Invalid command: " + command);
        else
            layers->addEmpty(String::parseInt(comm[1]), String::parseInt(comm[2]), String::parseInt(comm[3]), String::parseInt(comm[4]),
                             writeHistory);
    } else if (comm[0] == "addPicture") {
        if (comm.length() != 4) mHandler->addMessage("Invalid command: " + command);
        else layers->addPicture(comm[1].operator const char *(), String::parseInt(comm[2]), String::parseInt(comm[3]), writeHistory);
    } else if (comm[0] == "delete") {
        if (comm.length() != 2) mHandler->addMessage("Invalid command: " + command);
        else layers->deletetAt(String::parseInt(comm[1]), writeHistory);
    } else if (comm[0] == "swap") {
        if (comm.length() != 3) mHandler->addMessage("Invalid command: " + command);
        else layers->swap(String::parseInt(comm[1]), String::parseInt(comm[2]), writeHistory);
    } else if (comm[0] == "visible") {
        if (comm.length() != 2) mHandler->addMessage("Invalid command: " + command);
        else layers->changeVisibleAt(String::parseInt(comm[1]), writeHistory);
    } else if (comm[0] == "combine") {
        if (comm.length() != 3) mHandler->addMessage("Invalid command: " + command);
        else layers->combine(String::parseInt(comm[1]), String::parseInt(comm[2]), writeHistory);
    } else if (comm[0] == "duplicate") {
        if (comm.length() != 2) mHandler->addMessage("Invalid command: " + command);
        else layers->duplicate(String::parseInt(comm[1]), writeHistory);
    } else if (comm[0] == "move") {
        if (comm.length() != 4) mHandler->addMessage("Invalid command: " + command);
        else layers->move(String::parseInt(comm[1]), String::parseInt(comm[2]), String::parseInt(comm[3]), writeHistory);
    } else if (comm[0] == "scale") {
        if (comm.length() != 4) mHandler->addMessage("Invalid command: " + command);
        else
            layers->scale(String::parseInt(comm[1]), String::parseDouble(comm[2]), static_cast<ToolCorner>(String::parseInt(comm[3])),
                          writeHistory);
    } else if (comm[0] == "crop") {
        if (comm.length() != 6) mHandler->addMessage("Invalid command: " + command);
        else
            layers->crop(String::parseInt(comm[1]), String::parseInt(comm[2]), String::parseInt(comm[3]), String::parseInt(comm[4]),
                         String::parseInt(comm[5]), writeHistory);
    } else if (comm[0] == "autoCrop") {
        if (comm.length() != 2) mHandler->addMessage("Invalid command: " + command);
        else layers->autoCrop(String::parseInt(comm[1]), writeHistory);
    } else if (comm[0] == "rotate") {
        if (comm.length() != 3) mHandler->addMessage("Invalid command: " + command);
        else layers->rotate(String::parseInt(comm[1]), String::parseInt(comm[2]), writeHistory);
    } else if (comm[0] == "line") {
        if ((comm.length() - 1) % 7 != 0) mHandler->addMessage("Invalid command: " + command);
        else {
            for (int i = 1; i < comm.length(); i += 7) {
                layers->drawLine(String::parseInt(comm[i]), String::parseInt(comm[i + 1]), String::parseInt(comm[i + 2]),
                                 String::parseInt(comm[i + 3]),
                                 String::parseInt(comm[i + 4]), String::parseInt(comm[i + 5]), String::parseInt(comm[i + 6]), writeHistory);
            }
        }
    } else if (comm[0] == "shape") {
        if (comm.length() != 8) mHandler->addMessage("Invalid command: " + command);
        else
            layers->drawShape(String::parseInt(comm[1]), static_cast<Shape>(String::parseInt(comm[2])), String::parseInt(comm[3]),
                              String::parseInt(comm[4]), String::parseInt(comm[5]), String::parseInt(comm[6]), String::parseInt(comm[7]),
                              writeHistory);
    } else if (comm[0] == "replaceColor") {
        if (comm.length() != 6) mHandler->addMessage("Invalid command: " + command);
        else
            layers->replaceColor(String::parseInt(comm[1]), String::parseInt(comm[2]), String::parseInt(comm[3]), String::parseInt(comm[4]),
                                 String::parseDouble(comm[5]), writeHistory);
    } else mHandler->addMessage("Unknown command: " + command);
}

void History::printCommands() {
    mHandler->addMessage(Util::String::format("currentCommand: %d, commands size: %d, snapshot count: %d", currentCommand, commands->size(),
                                              snapshots->size()));
    for (int i = 0; i < commands->size(); i++) {
        auto m = commands->get(i);
        if ((i + 1) == currentCommand) m = ">> " + m;
        if ((i + 1) % SNAPSHOT_INTERVAL == 0) m = "SAVE " + m;
        mHandler->addMessage(m);
    }
}

void History::saveToFile(const Util::String &path) {
    if (Util::String(path).length() == 0) {
        mHandler->addMessage("Error: No path given");
        return;
    }
    FILE *file = fopen(path.operator const char *(), "w");
    if (file) {
        for (int i = 0; i < commands->size(); i++) {
            fputs(commands->get(i).operator const char *(), file);
            fputs("\n", file);
        }
        fclose(file);
    } else mHandler->addMessage("Error: Could not open file: " + path);
}

void History::loadFromFileInto(Layers *layers, const Util::String &path) {
    if (Util::String(path).length() == 0) {
        mHandler->addMessage("Error: No path given");
        return;
    }
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
    if (currentCommand == 0) mHandler->addMessage("No more commands to undo");
    else {
        uint32_t executeCounter;
        currentCommand--;
        if (currentCommand < SNAPSHOT_INTERVAL) { // dont use snapshots
            layers->reset();
            executeCounter = currentCommand;
        } else {
            auto snapshot = snapshots->get(currentCommand / SNAPSHOT_INTERVAL - 1);
            auto snapLayers = snapshot.first;
            for (int i = 0; i < snapshot.second; i++) {
                delete layers->layers[i];
                auto pixelData = new uint32_t[snapLayers[i]->width * snapLayers[i]->height];
                auto snapPixelData = snapLayers[i]->getPixelData();
                for (int j = 0; j < snapLayers[i]->width * snapLayers[i]->height; j++) {
                    pixelData[j] = snapPixelData[j];
                }
                layers->layers[i] = new Layer(snapLayers[i]->width, snapLayers[i]->height, snapLayers[i]->posX, snapLayers[i]->posY, 1,
                                              pixelData);
            }
            layers->layerCount = snapshot.second;
            if (layers->currentLayer >= layers->layerCount) layers->currentLayer = layers->layerCount - 1;
            executeCounter = currentCommand % SNAPSHOT_INTERVAL;
        }

        uint32_t tempCurrent = currentCommand;
        currentCommand -= executeCounter;
        for (uint32_t i = tempCurrent - executeCounter; i < tempCurrent; i++) {
            execCommandOn(layers, commands->get(i), false);
        }
    }
}

void History::redo(Layers *layers) {
    if (currentCommand < commands->size()) {
        execCommandOn(layers, commands->get(currentCommand), false);
    } else mHandler->addMessage("No more commands to redo");
}
