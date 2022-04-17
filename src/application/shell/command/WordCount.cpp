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

#include "lib/file/FileStatus.h"
#include "WordCount.h"
#include "lib/file/File.h"
#include <cctype>

void WordCount::execute(Util::Array<String> &args) {

    bytesTotal = 0;
    wordsTotal = 0;
    linesTotal = 0;

    Util::ArrayList<String> paths;

    Util::ArgumentParser parser(getHelpText(), 1);

    parser.addSwitch("lines", "l");
    parser.addSwitch("words","w");
    parser.addSwitch("bytes","c");

    if(!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }

    for(const String &path : parser.getUnnamedArguments()) {
        paths.add(path);
    }

    for(const String &path : paths) {
        String absolutePath = calcAbsolutePath(path);

        if (FileStatus::exists(absolutePath)) {
            FileStatus *fStat = FileStatus::stat(absolutePath);

            if (fStat->getFileType() != FsNode::DIRECTORY_FILE) {

                count(absolutePath, &parser);
            }
            delete fStat;

        } else {
            stderr << args[0] << " '" << path << "': File not found!" << endl;
        }
    }

    if(paths.size() > 1){
        wcPrintTotal(&parser);
    }
}

void WordCount::wcPrintTotal(Util::ArgumentParser* parser) {

    if(parser->checkSwitch("lines")){
        stdout << linesTotal << " ";
    }else if(parser->checkSwitch("words")){
        stdout << wordsTotal << " ";
    }else if(parser->checkSwitch("bytes")){
        stdout << bytesTotal << " ";
    }else{
        stdout
                << linesTotal << " "
                << wordsTotal << " "
                << bytesTotal << " ";
    }

    stdout << "total";

    stdout << endl;
    stdout.flush();
}

void WordCount::count(const String &absolutePath, Util::ArgumentParser* parser) {

    size_t bytes = 0;
    size_t words = 0;
    size_t lines = 0;

    File *file = File::open(absolutePath, "r");

    char c;
    bool isWord = false;

    const size_t l = file->getLength();

    for(size_t i=0;i < l; i++){

        c = file->readChar();
        bytes++;

        if(c == '\n')
            lines++;

        if(c == ' ' || c == '\t' || isspace(c)){

            if (isWord)
                words++;

            isWord = false;

            continue;
        }

        if(!isspace(c) && isprint(c))
            isWord = true;
        else
            isWord = false;
    }

    bytesTotal += bytes;
    wordsTotal += words;
    linesTotal += lines;

    wcPrintStats(parser, file, bytes, words, lines);

    delete file;
}

void WordCount::wcPrintStats(Util::ArgumentParser *parser, File *pFile, size_t bytes, size_t words, size_t lines) {

    if(parser->checkSwitch("lines")){
        stdout << lines << " ";
    }else if(parser->checkSwitch("words")){
        stdout << words << " ";
    }else if(parser->checkSwitch("bytes")){
        stdout << bytes << " ";
    }else{
        stdout
        << lines << " "
        << words << " "
        << bytes << " ";
    }

    stdout << pFile->getName();

    stdout << endl;
    stdout.flush();
}

const String WordCount::getHelpText(){
    return "Counts the Bytes, Words and Lines in File(s)\n\n"
           "Usage: wc [OPTION]... [PATH]...\n\n"
           "Options:\n"
           "  -c, --bytes: count only bytes\n"
           "  -w, --words: count only words\n"
           "  -l, --lines: count only newlines\n"
           "  -h, --help: Show this help-message.";
}

WordCount::WordCount(Shell &shell) : Command(shell) {

}
