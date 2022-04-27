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

#ifndef HHUOS_WORDCOUNT_H
#define HHUOS_WORDCOUNT_H

#include "lib/stream/OutputStream.h"
#include "lib/string/String.h"
#include "Command.h"
#include "lib/file/File.h"

/**
 * Implementation of Command.
 * Counts Bytes, Words and Lines in a File
 *
 * -h, --help: Show help message
 *
 * @author Alexander Hansen
 * @date 2022
 */
class WordCount : public Command {

public:
    /**
     * Constructor.
     *
     * @param shell The shell, that executes this command
     */
    explicit WordCount(Shell &shell);

    /**
     * Overriding function from Command.
     */
    void execute(Util::Array<String> &args) override;

    /**
     * Overriding function from Command.
     */
    const String getHelpText() override;

private:

    size_t bytesTotal = 0;
    size_t wordsTotal = 0;
    size_t linesTotal = 0;

    void count(const String &absolutePath, Util::ArgumentParser* parser);

    void wcPrintStats(Util::ArgumentParser *parser, File *pFile, size_t bytes, size_t words, size_t lines);

    void wcPrintTotal(Util::ArgumentParser* parser);
};

#endif //HHUOS_WORDCOUNT_H
