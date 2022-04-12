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
#include "lib/file/FileStatus.h"
#include <cctype>

void WordCount::execute(Util::Array<String> &args) {

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

    bytes = 0;
    words = 0;
    lines = 0;

    File *file = File::open(absolutePath, "r");

    char c;
    uint32_t currentWordLength = 0;

    while((c = file->readChar()) != FsNode::END_OF_FILE){

        for(size_t i = 0; i < file->getLength(); i++){

            switch (c) {
                case '\n':
                    lines++;
                    currentWordLength = 0;
                    break;
                case ' ':
                case '\t':
                    if(currentWordLength > 0)
                        words++;

                    currentWordLength = 0;
                default:
                    if(isprint(c))
                        currentWordLength++;
                    else
                        currentWordLength = 0;
                    break;
            }

            bytes++;
        }
    }

    bytesTotal += bytes;
    wordsTotal += words;
    linesTotal += lines;

    wcPrintStats(parser, file);

    delete file;
}

void WordCount::wcPrintStats(Util::ArgumentParser* parser, File *pFile) {

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
