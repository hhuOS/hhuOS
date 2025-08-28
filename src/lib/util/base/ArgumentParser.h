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

#include "base/String.h"
#include "collection/ArrayList.h"
#include "collection/HashMap.h"
#include "collection/Array.h"

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

    /// This class uses other non-copyable classes, so the copy constructor and assignment operator are deleted.
    ArgumentParser(const ArgumentParser &other) = delete;

    /// This class uses other non-copyable classes, so the copy constructor and assignment operator are deleted.
    ArgumentParser &operator=(const ArgumentParser &other) = delete;

    /// Set the help text that is displayed when the argument `-h` or `--help` is given.
    void setHelpText(const String &text);

    /// Add a new argument to the parser.
    /// Each argument must have a unique name. It may also have an abbreviation.
    /// Parameters to this method must not include the leading `-` or `--`.
    /// If `required` is set to true, the argument must be given. Otherwise, `parse()` will return false.
    void addArgument(const String &name, bool required = false, const String &abbreviation = "");

    /// Add a new switch to the parser.
    /// A switch is a boolean argument that can be either on or off.
    /// Each switch must have a unique name. It may also have an abbreviation.
    void addSwitch(const String &name, const String &abbreviation = "");

    /// When `parse()` fails, it returns false and stores an error message in a private variable.
    /// This method returns the error message, so it can be displayed to the user.
    [[nodiscard]] const String& getErrorString() const;

    /// Parse the command line arguments.
    /// If an error occurs (e.g. unknown argument), false is returned and an error message
    /// is stored in a private variable (can be retrieved with `getErrorString()`).
    /// The argument --help or -h causes this function to set the error message to the help text and return false.
    [[nodiscard]] bool parse(uint32_t argc, char *argv[]);

    /// Get all unnamed arguments that were passed to the program.
    /// An unnamed argument is an argument that does not start with `-` or `--`.
    [[nodiscard]] Array<String> getUnnamedArguments() const;

    /// Check if the given argument was passed to the program.
    [[nodiscard]] bool hasArgument(const String &name) const;

    /// Get the value of the given argument.
    /// If the argument was not passed, an empty string is returned.
    [[nodiscard]] String getArgument(const String &name) const;

    /// Check if the given switch was passed to the program.
    [[nodiscard]] bool checkSwitch(const String &name) const;

private:

    String helpText;
    String errorString;

    ArrayList<String> parameters;
    ArrayList<String> switches;
    ArrayList<String> requiredParameters;
    HashMap<String, String> abbreviationMap;

    HashMap<String, String> namedArguments;
    ArrayList<String> parsedSwitches;
    ArrayList<String> unnamedArguments;
};

}

#endif
