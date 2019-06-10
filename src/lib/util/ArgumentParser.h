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

#ifndef HHUOS_ARGPARSER_H
#define HHUOS_ARGPARSER_H

#include "lib/util/HashMap.h"
#include "lib/util/HashSet.h"
#include "lib/string/String.h"

namespace Util {

class ArgumentParser {

private:

    String helpText;
    uint32_t startIndex;

    String errorString;

    Util::ArrayList<String> parameters;
    Util::ArrayList<String> switches;
    Util::HashSet<String> requiredParameters;
    Util::HashMap<String, String> abbreviationMap;

    Util::HashMap<String, String> namedArguments;
    Util::HashMap<String, bool> switchMap;
    Util::ArrayList<String> unnamedArguments;

public:

    explicit ArgumentParser(String helpText, uint32_t startAt = 0);

    ArgumentParser(const ArgumentParser &copy) = delete;

    ~ArgumentParser() = default;

    void addParameter(const String &name, const String &abbreviation = "", bool required = false);

    void addSwitch(const String &name, const String &abbreviation = "");

    const String &getErrorString();

    bool parse(Util::Array<String> &arguments);

    Util::Array<String> getUnnamedArguments();

    const String getNamedArgument(const String &argument);

    bool checkSwitch(const String &name);
};

}

#endif