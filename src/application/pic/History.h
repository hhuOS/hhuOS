//
// Created by Rafael Reip on 14.11.24.
//

#ifndef HHUOS_HISTORY_H
#define HHUOS_HISTORY_H

#include "Layers.h"
#include "lib/libc/stdio.h"
#include "lib/util/io/stream/BufferedInputStream.h"


class History {
public:
    explicit History(MessageHandler *mHandler);

    ~History() = default;

    void addCommand(const Util::String &command);

    void reset();

    void execCommandOn(Layers *layers, const Util::String &command);

    void undo(Layers *layers);

    void redo(Layers *layers);

    void saveToFile(const Util::String &path);

    void loadFromFileInto(Layers *layers, const Util::String &path);

    void printCommands();

private:
    Util::List<Util::String> *commands;
    uint32_t currentCommand;
    MessageHandler *mHandler;
    static const int SNAPSHOT_INTERVAL = 5;
    Util::List<Util::String> *lines;
};


#endif //HHUOS_HISTORY_H
