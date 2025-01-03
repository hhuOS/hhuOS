//
// Created by Rafael Reip on 14.11.24.
//

#ifndef HHUOS_HISTORY_H
#define HHUOS_HISTORY_H

#include "lib/libc/stdio.h"
#include "lib/util/collection/Pair.h"

class Layers;

class Layer;

class MessageHandler;

class History {
public:
    explicit History(MessageHandler *mHandler);

    ~History();

    void addCommand(const Util::String &command, Layer ***layers, const int *layerCount);

    void reset();

    void undo(Layers *layers);

    void redo(Layers *layers);

    void saveToFile(const Util::String &path);

    void loadFromFileInto(Layers *layers, const Util::String &path);

    void printCommands();

private:

    void execCommandOn(Layers *layers, const Util::String &command, bool writeHistory);

    Util::List<Util::String> *commands;
    uint32_t currentCommand;
    MessageHandler *mHandler;
    static const int SNAPSHOT_INTERVAL = 5;
    Util::List<Util::String> *lines;
    Util::List<Util::Pair<Layer **, int>> *snapshots;
};


#endif //HHUOS_HISTORY_H
