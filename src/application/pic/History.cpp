//
// Created by Rafael Reip on 14.11.24.
//

#include "History.h"

#include "lib/util/io/stream/BufferedInputStream.h"
#include "lib/util/io/stream/FileInputStream.h"

#include "helper.h"
#include "MessageHandler.h"
#include "Layer.h"
#include "Layers.h"

/**
 * Constructor for the History class.
 * Initializes the command list, current command index, message handler, line list, and snapshot list.
 *
 * @param mHandler Pointer to the MessageHandler object.
 */
History::History(MessageHandler *mHandler) {
    this->commands = new Util::ArrayList<Util::String>();
    this->currentCommand = 0;
    this->mHandler = mHandler;
    this->lines = new Util::ArrayList<Util::String>();
    this->snapshots = new Util::ArrayList<Util::Pair<Layer **, int>>();
}

/**
 * Destructor for the History class.
 */
History::~History() {
    for (uint32_t i = 0; i < snapshots->size(); i++) {
        auto *snapshot = snapshots->get(i).first;
        for (int j = 0; j < snapshots->get(i).second; j++) {
            delete[] snapshot[j]->getPixelData();
            delete snapshot[j];
        }
        delete[] snapshot;
    }
    delete snapshots;
    delete lines;
    delete commands;
}

/**
 * Adds a command to the history.
 * If the command is a line command, it saves the line points.
 * If the command is to prepare the next drawing, it combines all line points into one line command.
 * Otherwise, it adds the command normally.
 * It also creates a snapshot every SNAPSHOT_INTERVAL commands.
 *
 * @param command The command to add.
 * @param layers Pointer to the layers.
 * @param layerCount Pointer to the number of layers.
 */
void History::addCommand(const Util::String &command, Layer ***layers, int *layerCount) {
    if (commands->size() > currentCommand - 1) { // remove all commands after current command
        for (uint32_t i = commands->size() - 1; i > currentCommand - 1; i--) {
            commands->removeIndex(i);
        }
        if (snapshots->size() > 0) { // remove all snapshots after current command
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
    if (command.beginsWith("line")) { // special case for lines - just save line points
        lines->add(command.substring(5, command.length()));
    } else if (command.beginsWith("prepareNextDrawing")) { // add all line points together to one line command
        if (lines->size() > 0) {
            Util::String linestogether = "line";
            for (uint32_t i = 0; i < lines->size(); i++) {
                linestogether += " " + lines->get(i);
            }
            commands->add(linestogether);
            currentCommand = commands->size();
            lines->clear();
            added = true;
        }
    } else { // add normal command
        commands->add(command);
        currentCommand = commands->size();
        added = true;
    }

    if (added && currentCommand % SNAPSHOT_INTERVAL == 0) { // create snapshot every SNAPSHOT_INTERVAL commands
        auto **snapshot = new Layer *[18];
        for (int i = 0; i < *layerCount; i++) {
            auto *newPixelData = new uint32_t[(*layers)[i]->width * (*layers)[i]->height];
            for (int j = 0; j < (*layers)[i]->width * (*layers)[i]->height; j++) {
                newPixelData[j] = (*layers)[i]->getPixelData()[j];
            }
            snapshot[i] = new Layer((*layers)[i]->width, (*layers)[i]->height, (*layers)[i]->posX, (*layers)[i]->posY, 1, newPixelData);
        }
        snapshots->add(Util::Pair<Layer **, int>(snapshot, *layerCount));
    }
}

/**
 * Resets the history by clearing all commands and snapshots.
 * Sets the current command index to 0.
 */
void History::reset() {
    commands->clear();
    snapshots->clear();
    currentCommand = 0;
}

/**
 * \brief Executes a command on the given layers.
 *
 * This function decodes the command String and executes the corresponding command on the given Layers object.
 *
 * @param layers Pointer to the Layers object.
 * @param command The command to execute.
 * @param writeHistory Boolean indicating whether to write the command to history.
 */
void History::execCommandOn(Layers *layers, const Util::String &command, bool writeHistory) {
    if (!writeHistory) currentCommand++;
    auto comm = command.split(" ");
    if (comm.length() == 0) return;
    if (comm[0] == "addEmpty") {
        if (comm.length() != 5) mHandler->addMessage("Invalid command: " + command);
        else
            layers->addEmpty(Util::String::parseInt(comm[1]), Util::String::parseInt(comm[2]),
                             Util::String::parseInt(comm[3]), Util::String::parseInt(comm[4]), writeHistory);
    } else if (comm[0] == "addPicture") {
        if (comm.length() != 4) mHandler->addMessage("Invalid command: " + command);
        else
            layers->addPicture(comm[1].operator const char *(), Util::String::parseInt(comm[2]), Util::String::parseInt(comm[3]),
                               writeHistory);
    } else if (comm[0] == "delete") {
        if (comm.length() != 2) mHandler->addMessage("Invalid command: " + command);
        else layers->deletetAt(Util::String::parseInt(comm[1]), writeHistory);
    } else if (comm[0] == "swap") {
        if (comm.length() != 3) mHandler->addMessage("Invalid command: " + command);
        else layers->swap(Util::String::parseInt(comm[1]), Util::String::parseInt(comm[2]), writeHistory);
    } else if (comm[0] == "visible") {
        if (comm.length() != 2) mHandler->addMessage("Invalid command: " + command);
        else layers->changeVisibleAt(Util::String::parseInt(comm[1]), writeHistory);
    } else if (comm[0] == "combine") {
        if (comm.length() != 3) mHandler->addMessage("Invalid command: " + command);
        else layers->combine(Util::String::parseInt(comm[1]), Util::String::parseInt(comm[2]), writeHistory);
    } else if (comm[0] == "duplicate") {
        if (comm.length() != 2) mHandler->addMessage("Invalid command: " + command);
        else layers->duplicate(Util::String::parseInt(comm[1]), writeHistory);
    } else if (comm[0] == "move") {
        if (comm.length() != 4) mHandler->addMessage("Invalid command: " + command);
        else layers->move(Util::String::parseInt(comm[1]), Util::String::parseInt(comm[2]), Util::String::parseInt(comm[3]), writeHistory);
    } else if (comm[0] == "scale") {
        if (comm.length() != 4) mHandler->addMessage("Invalid command: " + command);
        else
            layers->scale(Util::String::parseInt(comm[1]), Util::String::parseDouble(comm[2]),
                          static_cast<ToolCorner>(Util::String::parseInt(comm[3])), writeHistory);
    } else if (comm[0] == "crop") {
        if (comm.length() != 6) mHandler->addMessage("Invalid command: " + command);
        else
            layers->crop(Util::String::parseInt(comm[1]), Util::String::parseInt(comm[2]), Util::String::parseInt(comm[3]),
                         Util::String::parseInt(comm[4]), Util::String::parseInt(comm[5]), writeHistory);
    } else if (comm[0] == "autoCrop") {
        if (comm.length() != 2) mHandler->addMessage("Invalid command: " + command);
        else layers->autoCrop(Util::String::parseInt(comm[1]), writeHistory);
    } else if (comm[0] == "rotate") {
        if (comm.length() != 3) mHandler->addMessage("Invalid command: " + command);
        else layers->rotate(Util::String::parseInt(comm[1]), Util::String::parseInt(comm[2]), writeHistory);
    } else if (comm[0] == "line") {
        if ((comm.length() - 1) % 7 != 0) mHandler->addMessage("Invalid command: " + command);
        else {
            for (uint32_t i = 1; i < comm.length(); i += 7) {
                layers->drawLine(Util::String::parseInt(comm[i]), Util::String::parseInt(comm[i + 1]), Util::String::parseInt(comm[i + 2]),
                                 Util::String::parseInt(comm[i + 3]), Util::String::parseInt(comm[i + 4]),
                                 Util::String::parseInt(comm[i + 5]), Util::String::parseInt(comm[i + 6]), writeHistory);
            }
        }
    } else if (comm[0] == "shape") {
        if (comm.length() != 8) mHandler->addMessage("Invalid command: " + command);
        else
            layers->drawShape(Util::String::parseInt(comm[1]), static_cast<Shape>(Util::String::parseInt(comm[2])),
                              Util::String::parseInt(comm[3]), Util::String::parseInt(comm[4]), Util::String::parseInt(comm[5]),
                              Util::String::parseInt(comm[6]), Util::String::parseInt(comm[7]), writeHistory);
    } else if (comm[0] == "replaceColor") {
        if (comm.length() != 6) mHandler->addMessage("Invalid command: " + command);
        else
            layers->replaceColor(Util::String::parseInt(comm[1]), Util::String::parseInt(comm[2]), Util::String::parseInt(comm[3]),
                                 Util::String::parseInt(comm[4]), Util::String::parseDouble(comm[5]), writeHistory);
    } else if (comm[0] == "filterBlackWhite") {
        if (comm.length() != 2) mHandler->addMessage("Invalid command: " + command);
        else layers->filterBlackWhite(Util::String::parseInt(comm[1]), writeHistory);
    } else if (comm[0] == "filterInvert") {
        if (comm.length() != 2) mHandler->addMessage("Invalid command: " + command);
        else layers->filterInvert(Util::String::parseInt(comm[1]), writeHistory);
    } else if (comm[0] == "filterSepia") {
        if (comm.length() != 2) mHandler->addMessage("Invalid command: " + command);
        else layers->filterSepia(Util::String::parseInt(comm[1]), writeHistory);
    } else if (comm[0] == "filterKernel") {
        if (comm.length() != 13) mHandler->addMessage("Invalid command: " + command);
        else {
            int kernel[9];
            for (int i = 0; i < 9; i++) {
                kernel[i] = Util::String::parseInt(comm[i + 2]);
            }
            layers->filterKernel(Util::String::parseInt(comm[1]), kernel, Util::String::parseInt(comm[11]),
                                 Util::String::parseInt(comm[12]), writeHistory);
        }
    } else mHandler->addMessage("Unknown command: " + command);
}

/**
 * \brief Prints the list of commands in the history.
 *
 * This function prints the current command index, the size of the command list,
 * and the number of snapshots. It also prints each command in the history.
 * Commands that are at the current command index are prefixed with ">> ".
 * Commands that are at snapshot intervals are prefixed with "SAVE ".
 */
void History::printCommands() {
    mHandler->addMessage(Util::String::format(
            "currentCommand: %d, commands size: %d, snapshot count: %d", currentCommand, commands->size(), snapshots->size()));
    for (uint32_t i = 0; i < commands->size(); i++) {
        auto m = commands->get(i);
        if ((i + 1) == currentCommand) m = ">> " + m;
        if ((i + 1) % SNAPSHOT_INTERVAL == 0) m = "SAVE " + m;
        mHandler->addMessage(m);
    }
}

/**
 * \brief Saves the command history to a file.
 *
 * This function writes the list of commands in the history to a specified file.
 *
 * \param path The file path where the command history will be saved.
 */
void History::saveToFile(const Util::String &path) {
    if (Util::String(path).length() == 0) {
        mHandler->addMessage("Error: No path given");
        return;
    }
    FILE *file = fopen(path.operator const char *(), "w");
    if (file) {
        for (uint32_t i = 0; i < commands->size(); i++) {
            fputs(commands->get(i).operator const char *(), file);
            fputs("\n", file);
        }
        fclose(file);
    } else mHandler->addMessage("Error: Could not open file: " + path);
}

/**
 * \brief Loads the command history from a file and executes the commands on the given layers.
 *
 * This function reads the list of commands from a specified file and executes each command on the provided Layers object after resetting the layers.
 * This results in the layers being in the same state as when the commands were saved.
 *
 * \param layers Pointer to the Layers object.
 * \param path The file path from which the command history will be loaded.
 */
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
            execCommandOn(layers, line, true);
            if (line.beginsWith("line")) { // special case for lines
                addCommand(Util::String("prepareNextDrawing"), &layers->layers, &layers->layerCount);
            }
        }
        line = stream.readLine(eof);
    }
}

/**
 * \brief Undoes the last command executed on the given layers.
 *
 * This function reverts the state of the layers to the state before the last command was executed.
 * If the current command index is 0, it indicates that there are no more commands to undo.
 *
 * @param layers Pointer to the Layers object.
 */
void History::undo(Layers *layers) {
    if (currentCommand == 0) {
        mHandler->addMessage("No more commands to undo");
        return;
    }

    uint32_t executeCounter;
    currentCommand--;
    if (currentCommand < SNAPSHOT_INTERVAL) { // dont use snapshots before first snapshot is taken
        layers->reset();
        executeCounter = currentCommand;
    } else { // apply last snapshot before current command
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
    for (uint32_t i = tempCurrent - executeCounter; i < tempCurrent; i++) { // execute commands between snapshot and current command
        execCommandOn(layers, commands->get(i), false);
    }

}

/**
 * \brief Redoes the last undone command on the given layers.
 *
 * This function re-executes the command that was last undone, if there are any commands to redo.
 * If the current command index is at the end of the command list, it indicates that there are no more commands to redo.
 *
 * @param layers Pointer to the Layers object.
 */
void History::redo(Layers *layers) {
    if (currentCommand < commands->size()) {
        execCommandOn(layers, commands->get(currentCommand), false);
    } else mHandler->addMessage("No more commands to redo");
}
