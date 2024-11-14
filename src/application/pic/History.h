//
// Created by Rafael Reip on 14.11.24.
//

#ifndef HHUOS_HISTORY_H
#define HHUOS_HISTORY_H

#include "Layers.h"


class History {
public:
    explicit History(MessageHandler *mHandler);

    ~History() = default;

    void addCommand(const Util::String& command);

    void execCommandOn(Layers *layers, const Util::String& command);

    void undo(Layers *layers);

    void redo(Layers *layers);

    void saveToFile(Util::String path);

    void loadFromFile(Util::String path);

    void printCommands();

private:
    Util::List<Util::String> *commands;
    uint32_t currentCommand;
    MessageHandler* mHandler;
    static const int SNAPSHOT_INTERVAL = 5;
};


#endif //HHUOS_HISTORY_H
