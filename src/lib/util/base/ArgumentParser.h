/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

#ifndef HHUOS_LIB_UTIL_ARGUMENTPARSER_H
#define HHUOS_LIB_UTIL_ARGUMENTPARSER_H

#include <stdint.h>

#include "util/base/String.h"
#include "util/collection/ArrayList.h"
#include "util/collection/HashMap.h"
#include "util/collection/Array.h"

namespace Util {

/// This class is used to parse command line arguments.
/// The muser must first declare the expected arguments and switches.
/// After that, `parse()` must be called with the command line arguments.
/// Finally, the user can check for the presence of arguments and switches.
///
/// ## Example
/// ```
/// int main(int argc, char *argv[]) {
///     // Create parser instance
///     auto parser = Util::ArgumentParser();
///
///     // Add known arguments and switches.
///     parser.setHelpText("Usage: program [options] [arguments]");
///     parser.addArgument("arg1", false, "a"); // Optional argument --arg1 or -a
///     parser.addArgument("arg2", true, "b"); // Required argument --arg2 or -b
///     parser.addSwitch("switch1", "s"); // Switch --switch1 or -s
///
///     // Parse the command line arguments.
///     if (!parser.parse(argc, argv)) {
///         // Parsing failed, print error message and exit.
///         Util::System::out << parser.getErrorString() << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
///         return -1;
///     }
///
///     // Parsing succeeded, check for arguments and switches.
///     if (parser.hasArgument("arg1")) {
///         // Argument --arg1 or -a was given.
///         auto arg1 = parser.getArgument("arg1");
///         ...
///     }
///
///     if (parser.hasArgument("arg2")) {
///         // Argument --arg2 or -b was given.
///         auto arg2 = parser.getArgument("arg2");
///         ...
///     }
///
///     if (parser.checkSwitch("switch1")) {
///         // Switch --switch1 or -s was given.
///         ...
///     }
/// }
/// ```
class ArgumentParser {

public:
    /// Create a new argument parser.
    ArgumentParser() = default;

    /// Set whether unknown arguments should be ignored.
    /// If this is set to false (default), `parse()` will fail if an unknown argument is encountered.
    /// If this is set to true, unknown arguments will be treated as unnamed arguments.
    void setIgnoreUnknownArguments(const bool ignore) {
        ignoreUnknownArguments = ignore;
    }

    /// Set the help text that is displayed when the argument `-h` or `--help` is given.
    void setHelpText(const String &text) {
        helpText = text;
    }

    /// Add a new argument to the parser.
    /// Each argument must have a unique name. It may also have an abbreviation.
    /// Parameters to this method must not include the leading `-` or `--`.
    /// If `required` is set to true, the argument must be given. Otherwise, `parse()` will return false.
    void addArgument(const String &name, const bool required = false, const String &abbreviation = "") {
        parameters.add(name);
        abbreviationMap.put(abbreviation, name);
        if (required) {
            requiredParameters.add(name);
        }
    }

    /// Add a new switch to the parser.
    /// A switch is a boolean argument that can be either on or off.
    /// Each switch must have a unique name. It may also have an abbreviation.
    void addSwitch(const String &name, const String &abbreviation = "") {
        switches.add(name);
        abbreviationMap.put(abbreviation, name);
    }

    /// When `parse()` fails, it returns false and stores an error message in a private variable.
    /// This method returns the error message, so it can be displayed to the user.
    const String& getErrorString() const {
        return errorString;
    }

    /// Parse the command line arguments.
    /// If an error occurs (e.g. unknown argument), false is returned and an error message
    /// is stored in a private variable (can be retrieved with `getErrorString()`).
    /// The argument --help or -h causes this function to set the error message to the help text and return false.
    bool parse(uint32_t argc, char *argv[]);

    /// Get all unnamed arguments that were passed to the program.
    /// An unnamed argument is an argument that does not start with `-` or `--`.
    /// If the parser is set to ignore unknown arguments and an unknown argument is encountered,
    /// it is also treated as an unnamed argument.
    const Array<String>& getUnnamedArguments() const {
        return unnamedArguments;
    }

    /// Check if the given argument was passed to the program.
    bool hasArgument(const String &name) const {
        return namedArguments.containsKey(name);
    }

    /// Get the value of the given argument.
    /// If the argument was not passed, an empty string is returned.
    String getArgument(const String &name) const {
        return namedArguments.containsKey(name) ? namedArguments.get(name) : "";
    }

    /// Check if the given switch was passed to the program.
    bool checkSwitch(const String &name) const {
        return parsedSwitches.contains(name);
    }

private:

    bool ignoreUnknownArguments = false;
    String helpText;
    String errorString;

    ArrayList<String> parameters;
    ArrayList<String> switches;
    ArrayList<String> requiredParameters;
    HashMap<String, String> abbreviationMap;

    HashMap<String, String> namedArguments;
    ArrayList<String> parsedSwitches;
    Array<String> unnamedArguments;
};

}

#endif
