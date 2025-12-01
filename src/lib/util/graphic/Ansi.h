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

#ifndef HHUOS_LIB_UTIL_GRAPHIC_ANSI_H
#define HHUOS_LIB_UTIL_GRAPHIC_ANSI_H

#include <stdint.h>

#include "util/base/String.h"
#include "util/base/System.h"
#include "util/graphic/Color.h"
#include "util/graphic/Colors.h"

/// Contains functions and definitions for ANSI terminal graphics control.
/// Most function use ANSI escape sequences to control text formatting, colors and cursor movement.
/// Some special function use a control system call on the standard input/output file descriptors
/// to change terminal settings (e.g. enabling/disabling echoing of input characters).
namespace Util::Graphic::Ansi {

/// The standard 8 ANSI colors.
enum AnsiColor : uint8_t {
    /// Black color.
    BLACK = 0,
    /// Red color.
    RED = 1,
    /// Green color.
    GREEN = 2,
    /// Yellow color.
    YELLOW = 3,
    /// Blue color.
    BLUE = 4,
    /// Magenta color.
    MAGENTA = 5,
    /// Cyan color.
    CYAN = 6,
    /// White color.
    WHITE = 7,
};

/// Graphic effects that can be applied to text.
enum GraphicRendition : uint8_t {
    // No effect.
    NORMAL = 0,
    // Bright (bold) text.
    BRIGHT = 1,
    DIM = 2,
    ITALIC = 3,
    UNDERLINE = 4,
    SLOW_BLINK = 5,
    FAST_BLINK = 6,
    INVERT = 7,
    RESET_BRIGHT_DIM = 22,
    RESET_ITALIC = 23,
    RESET_UNDERLINE = 24,
    RESET_BLINK = 25,
    RESET_INVERT = 27
};

/// Codes for special keys, that can be returned by `readChar()` if it encounters the corresponding escape sequence.
enum Key : int16_t {
    /// The up arrow key.
    UP = 0x0100,
    /// The down arrow key.
    DOWN = 0x0101,
    /// The right arrow key.
    RIGHT = 0x0102,
    /// The left arrow key.
    LEFT = 0x0103,
    /// The home key.
    HOME = 0x0104,
    /// The end key.
    END = 0x0105
};

/// Represents a position of the cursor in the terminal.
struct CursorPosition {
    /// The column of the cursor.
    uint16_t column;
    /// The row of the cursor.
    uint16_t row;

    /// Compare this cursor position to another one for equality.
    bool operator==(const CursorPosition &other) const {
        return column == other.column && row == other.row;
    }
};

/// Enable echoing of input characters for the standard input.
/// In practice, this means that characters read from standard input will also be printed to standard output.
void enableEcho();

/// Disable echoing of input characters for the standard input.
/// In practice, this means that characters read from standard input will not be printed to standard output.
void disableEcho();

/// Enable line aggregation for the standard input.
/// This causes the underlying terminal to buffer input until a newline is encountered.
void enableLineAggregation();

/// Disable line aggregation for the standard input.
/// This causes the underlying terminal to provide input characters immediately.
void disableLineAggregation();

/// Enable the display of the cursor for the standard output.
void enableCursor();

/// Disable the display of the cursor for the standard output.
void disableCursor();

/// Enable ANSI escape sequence parsing for the standard output.
/// This allows the use of ANSI escape sequences to control text formatting and cursor movement
/// in the underlying terminal.
void enableAnsiParsing();

/// Disable ANSI escape sequence parsing for the standard output.
/// This prevents the underlying terminal from interpreting ANSI escape sequences,
/// causing them to be displayed as plain text.
void disableAnsiParsing();

/// Prepare the standard input terminal for graphical application usage.
/// This disables the cursor, line aggregation and ANSI parsing, so that each input character can be read immediately.
/// If `enableScancodes` is true, raw PS/2 keyboard scancodes will be provided instead of ASCII characters.
/// This is useful for applications that need to handle special keys or key combinations (e.g. games).
void prepareGraphicalApplication(bool enableScancodes);

/// Restore the standard input terminal from graphical application usage.
/// This re-enables the cursor, line aggregation and ANSI parsing.
void cleanupGraphicalApplication();

/// Enable raw mode for the standard input terminal.
/// In raw mode, input is made available character by character, without any processing or interpretation.
/// Furthermore, echoing is disabled.
void enableRawMode();

/// Enable canonical mode for the standard input terminal.
/// In canonical mode, input is processed line by line and echoing parsing of ANSI escape sequences is enabled.
void enableCanonicalMode();

/// Generate an ANSI escape sequence for setting the foreground color using an 8-bit color index.
/// The index is in the range 0-255, where the first 16 colors are the standard ANSI colors,
/// the next 216 colors are a 6x6x6 color cube, and the last 24 colors are grayscale colors
/// (as defined in the ANSI Xterm 256-color palette).
/// The returned string can be printed to the terminal to change the foreground color.
///
/// ### Example
/// ```c++
/// Util::System::out <<
///     Util::Graphic::Ansi::foreground8BitColor(196) << // Set foreground color to bright red
///     "This text is red!" << // Print text
///     Util::Graphic::Ansi::RESET << // Reset colors and effects
///     Util::Io::PrintStream::flush; // Flush output
/// ```
String foreground8BitColor(uint8_t colorIndex);

/// Generate an ANSI escape sequence for setting the background color using an 8-bit color index.
/// The index is in the range 0-255, where the first 16 colors are the standard ANSI colors,
/// the next 216 colors are a 6x6x6 color cube, and the last 24 colors are grayscale colors
/// (as defined in the ANSI Xterm 256-color palette).
/// The returned string can be printed to the terminal to change the background color.
///
//// ### Example
/// ```c++
/// Util::System::out <<
///     Util::Graphic::Ansi::background8BitColor(21) << // Set background color to bright blue
///     "This text has a blue background!" << // Print text
///     Util::Graphic::Ansi::RESET << // Reset colors and effects
///     Util::Io::PrintStream::flush; // Flush output
/// ```
String background8BitColor(uint8_t colorIndex);

/// Generate an ANSI escape sequence for setting the foreground color using a 24-bit RGB color.
/// The returned string can be printed to the terminal to change the foreground color.
///
/// ### Example
/// ```c++
/// Util::System::out <<
///     Util::Graphic::Ansi::foreground24BitColor(Util::Graphic::Color(255, 100, 0)) << // Set fg color to orange
///     "This text is orange!" << // Print text
///     Util::Graphic::Ansi::RESET << // Reset colors and effects
///     Util::Io::PrintStream::flush; // Flush output
/// ```
String foreground24BitColor(const Color &color);

/// Generate an ANSI escape sequence for setting the background color using a 24-bit RGB color.
/// The returned string can be printed to the terminal to change the background color.
///
/// ### Example
/// ```c++
/// Util::System::out <<
///     Util::Graphic::Ansi::background24BitColor(Util::Graphic::Color(0, 100, 255)) << // Set bg color to light blue
///     "This text has a light blue background!" << // Print text
///     Util::Graphic::Ansi::RESET << // Reset colors and effects
///     Util::Io::PrintStream::flush; // Flush output
/// ```
String background24BitColor(const Color &color);

/// Set the foreground color of the standard output using a standard ANSI color.
/// The `bright` parameter specifies whether to use the bright variant of the color.
///
/// ### Example
/// ```c++
/// Util::Graphic::Ansi::setForegroundColor(Util::Graphic::Ansi::RED, true); // Set foreground color to bright red
/// Util ::System::out << "This text is bright red!" << Util::Io::PrintStream::flush; // Print text
/// ```
void setForegroundColor(AnsiColor color, bool bright);

/// Set the background color of the standard output using a standard ANSI color.
/// The `bright` parameter specifies whether to use the bright variant of the color.
///
/// ### Example
/// ```c++
/// Util::Graphic::Ansi::setBackgroundColor(Util::Graphic::Ansi::BLUE, true); // Set background color to bright blue
/// Util::System::out << "This text has a bright blue background!" << Util::Io::PrintStream::flush; // Print text
/// ```
void setBackgroundColor(AnsiColor color, bool bright);

/// Set the foreground color of the standard output using an 8-bit color index.
/// The index is in the range 0-255, where the first 16 colors are the standard ANSI colors,
/// the next 216 colors are a 6x6x6 color cube, and the last 24 colors are grayscale colors
/// (as defined in the ANSI Xterm 256-color palette).
///
/// ### Example
/// ```c++
/// Util::Graphic::Ansi::setForegroundColor(196); // Set foreground color to bright red
/// Util::System::out << "This text is red!" << Util::Io::PrintStream::flush; // Print text
/// ```
void setForegroundColor(uint8_t colorIndex);

/// Set the background color of the standard output using an 8-bit color index.
/// The index is in the range 0-255, where the first 16 colors are the standard ANSI colors,
/// the next 216 colors are a 6x6x6 color cube, and the last 24 colors are grayscale colors
/// (as defined in the ANSI Xterm 256-color palette).
///
/// ### Example
/// ```c++
/// Util::Graphic::Ansi::setBackgroundColor(21); // Set background color to bright blue
/// Util::System::out << "This text has a blue background!" << Util::Io::PrintStream::flush; // Print text
/// ```
void setBackgroundColor(uint8_t colorIndex);

/// Set the foreground color of the standard output using a 24-bit RGB color.
///
/// ### Example
/// ```c++
/// Util::Graphic::Ansi::setForegroundColor(Util::Graphic::Color(255, 100, 0)); // Set foreground color to orange
/// Util::System::out << "This text is orange!" << Util::Io::PrintStream::flush; // Print text
/// ```
void setForegroundColor(const Color &color);

/// Set the background color of the standard output using a 24-bit RGB color.
///
/// ### Example
/// ```c++
/// Util::Graphic::Ansi::setBackgroundColor(Util::Graphic::Color(0, 100, 255)); // Set background color to light blue
/// Util::System::out << "This text has a light blue background!" << Util::Io::PrintStream::flush; // Print text
/// ```
void setBackgroundColor(const Color &color);

/// Reset the foreground color of the standard output to the default color of the terminal (usually white).
///
/// ### Example
/// ```c++
/// Util::Graphic::Ansi::setForegroundColor(Util::Graphic::Ansi::RED, true); // Set foreground color to bright red
/// Util::System::out << "This text is bright red!" << Util::Io::PrintStream::flush; // Print text
/// Util::Graphic::Ansi::resetForegroundColor(); // Reset foreground color to default
/// Util::System::out << "This text is in the default color!" << Util::Io::PrintStream::flush; // Print text
/// ```
void resetForegroundColor();

/// Reset the background color of the standard output to the default color of the terminal (usually black).
///
/// ### Example
/// ```c++
/// Util::Graphic::Ansi::setBackgroundColor(Util::Graphic::Ansi::BLUE, true); // Set background color to bright blue
/// Util::System::out << "This text has a bright blue background!" << Util::Io::PrintStream::flush; // Print text
/// Util::Graphic::Ansi::resetBackgroundColor(); // Reset background color to default
/// Util::System::out << "This text has the default background color!" << Util::Io::PrintStream::flush; // Print text
/// ```
void resetBackgroundColor();

/// Reset all colors and graphic effects of the standard output to their default values.
/// This resets both foreground and background colors, as well as any applied text effects (like bold, underline, etc.).
//
//// ### Example
/// ```c++
/// Util::Graphic::Ansi::setForegroundColor(Util::Graphic::Ansi::RED, true); // Set foreground color to bright red
/// Util::Graphic::Ansi::setBackgroundColor(Util::Graphic::Ansi::BLUE, true); // Set background color to bright blue
/// Util::System::out << "This text is bright red on a bright blue background!" << Util::Io::PrintStream::flush;
/// Util::Graphic::Ansi::resetColorsAndEffects(); // Reset colors and effects
/// Util::System::out << "This text is in the default colors!" << Util::Io::PrintStream::flush;
/// ```
void resetColorsAndEffects();

/// Set the cursor position in the terminal to the specified position.
void setPosition(const CursorPosition &position);

/// Move the cursor up by the specified number of lines.
void moveCursorUp(uint16_t lines);

/// Move the cursor down by the specified number of lines.
void moveCursorDown(uint16_t lines);

/// Move the cursor right by the specified number of columns.
void moveCursorRight(uint16_t columns);

/// Move the cursor left by the specified number of columns.
void moveCursorLeft(uint16_t columns);

/// Move the cursor to the beginning of the next line, offset by the specified number of lines.
void moveCursorToBeginningOfNextLine(uint16_t offset);

/// Move the cursor to the beginning of the previous line, offset by the specified number of lines.
void moveCursorToBeginningOfPreviousLine(uint16_t offset);

/// Set the row of the cursor to the specified value.
void setColumn(uint16_t column);

/// Save the current cursor position.
/// The position can be restored later using `restoreCursorPosition()`.
/// Only one position can be saved at a time.
void saveCursorPosition();

/// Restore the cursor position to the last saved position using `saveCursorPosition()`.
/// If no position has been saved, the cursor position remains unchanged.
void restoreCursorPosition();

/// Clear the entire screen and move the cursor to the home position (0,0).
void clearScreen();

/// Clear the screen from the current cursor position to the end of the screen.
void clearScreenFromCursor();

/// Clear the screen from the beginning of the screen to the current cursor position.
void clearScreenToCursor();

/// Clear the entire current line.
void clearLine();

/// Clear the current line from the cursor position to the end of the line.
void clearLineFromCursor();

/// Clear the current line from the beginning of the line to the cursor position.
void clearLineToCursor();

/// Get the current cursor position in the terminal.
CursorPosition getCursorPosition();

/// Get the limits of the cursor position in the terminal (maximum columns and rows).
/// This is typically the size of the terminal window.
CursorPosition getCursorLimits();

/// Read a character from the specified input stream (default: standard input).
/// This function handles ANSI escape sequences for special keys (like arrow keys) and returns
/// the corresponding `Key` enum value. If a regular character is read, its ASCII value is returned.
/// If the end of the stream is reached, -1 is returned.
//// ### Example
/// ```c++
/// const auto ch = Util::Graphic::Ansi::readChar(); // Read a character from standard input
///
/// if (ch == Util::Graphic::Ansi::UP) {
///     Util::System::out << "Up arrow key pressed!" << Util::Io::PrintStream::flush;
/// } else if (ch >= 0) {
///     Util::System::out << "Character pressed: " << static_cast<char>(ch) << Util::Io::PrintStream::flush;
/// }
/// ```
int16_t readChar(Io::InputStream &stream = System::in);

static constexpr char ESCAPE_SEQUENCE_START = 0x1b;
static constexpr const char *RESET = "\u001b[0m";
static constexpr const char *FOREGROUND_BLACK = "\u001b[30m";
static constexpr const char *FOREGROUND_RED = "\u001b[31m";
static constexpr const char *FOREGROUND_GREEN = "\u001b[32m";
static constexpr const char *FOREGROUND_YELLOW = "\u001b[33m";
static constexpr const char *FOREGROUND_BLUE = "\u001b[34m";
static constexpr const char *FOREGROUND_MAGENTA = "\u001b[35m";
static constexpr const char *FOREGROUND_CYAN = "\u001b[36m";
static constexpr const char *FOREGROUND_WHITE = "\u001b[37m";
static constexpr const char *FOREGROUND_DEFAULT = "\u001b[39m";
static constexpr const char *FOREGROUND_BRIGHT_BLACK = "\u001b[90m";
static constexpr const char *FOREGROUND_BRIGHT_RED = "\u001b[91m";
static constexpr const char *FOREGROUND_BRIGHT_GREEN = "\u001b[92m";
static constexpr const char *FOREGROUND_BRIGHT_YELLOW = "\u001b[93m";
static constexpr const char *FOREGROUND_BRIGHT_BLUE = "\u001b[94m";
static constexpr const char *FOREGROUND_BRIGHT_MAGENTA = "\u001b[95m";
static constexpr const char *FOREGROUND_BRIGHT_CYAN = "\u001b[96m";
static constexpr const char *FOREGROUND_BRIGHT_WHITE = "\u001b[97m";
static constexpr const char *BACKGROUND_BLACK = "\u001b[40m";
static constexpr const char *BACKGROUND_RED = "\u001b[41m";
static constexpr const char *BACKGROUND_GREEN = "\u001b[42m";
static constexpr const char *BACKGROUND_YELLOW = "\u001b[43m";
static constexpr const char *BACKGROUND_BLUE = "\u001b[44m";
static constexpr const char *BACKGROUND_MAGENTA = "\u001b[45m";
static constexpr const char *BACKGROUND_CYAN = "\u001b[46m";
static constexpr const char *BACKGROUND_WHITE = "\u001b[47m";
static constexpr const char *BACKGROUND_DEFAULT = "\u001b[49m";
static constexpr const char *BACKGROUND_BRIGHT_BLACK = "\u001b[100m";
static constexpr const char *BACKGROUND_BRIGHT_RED = "\u001b[101m";
static constexpr const char *BACKGROUND_BRIGHT_GREEN = "\u001b[102m";
static constexpr const char *BACKGROUND_BRIGHT_YELLOW = "\u001b[103m";
static constexpr const char *BACKGROUND_BRIGHT_BLUE = "\u001b[104m";
static constexpr const char *BACKGROUND_BRIGHT_MAGENTA = "\u001b[105m";
static constexpr const char *BACKGROUND_BRIGHT_CYAN = "\u001b[106m";
static constexpr const char *BACKGROUND_BRIGHT_WHITE = "\u001b[107m";

static const Color COLOR_TABLE_256[256] = {
    // 16 predefined colors, matching the 4-bit ANSI colors
    Colors::BLACK, Colors::RED, Colors::GREEN, Colors::YELLOW,
    Colors::BLUE, Colors::MAGENTA, Colors::CYAN, Colors::WHITE,
    Colors::BLACK.bright(), Colors::RED.bright(), Colors::GREEN.bright(), Colors::YELLOW.bright(),
    Colors::BLUE.bright(), Colors::MAGENTA.bright(), Colors::CYAN.bright(), Colors::WHITE.bright(),

    // 216 colors
    Color(0, 0, 0), Color(0, 0, 95), Color(0, 0, 135),
    Color(0, 0, 175), Color(0, 0, 215), Color(0, 0, 255),

    Color(0, 95, 0), Color(0, 95, 95), Color(0, 95, 135),
    Color(0, 95, 175), Color(0, 95, 215), Color(0, 95, 255),

    Color(0, 135, 0), Color(0, 135, 95), Color(0, 135, 135),
    Color(0, 135, 175), Color(0, 135, 215), Color(0, 135, 255),

    Color(0, 175, 0), Color(0, 175, 95), Color(0, 175, 135),
    Color(0, 175, 175), Color(0, 175, 215), Color(0, 175, 255),

    Color(0, 215, 0), Color(0, 215, 95), Color(0, 215, 135),
    Color(0, 215, 175), Color(0, 215, 215), Color(0, 215, 255),

    Color(0, 255, 0), Color(0, 255, 95), Color(0, 255, 135),
    Color(0, 255, 175), Color(0, 255, 215), Color(0, 255, 255),

    Color(95, 0, 0), Color(95, 0, 95), Color(95, 0, 135),
    Color(95, 0, 175), Color(95, 0, 215), Color(95, 0, 255),

    Color(95, 95, 0), Color(95, 95, 95), Color(95, 95, 135),
    Color(95, 95, 175), Color(95, 95, 215), Color(95, 95, 255),

    Color(95, 135, 0), Color(95, 135, 95), Color(95, 135, 135),
    Color(95, 135, 175), Color(95, 135, 215), Color(95, 135, 255),

    Color(95, 175, 0), Color(95, 175, 95), Color(95, 175, 135),
    Color(95, 175, 175), Color(95, 175, 215), Color(95, 175, 255),

    Color(95, 215, 0), Color(95, 215, 95), Color(95, 215, 135),
    Color(95, 215, 175), Color(95, 215, 215), Color(95, 215, 255),

    Color(95, 255, 0), Color(95, 255, 95), Color(95, 255, 135),
    Color(95, 255, 175), Color(95, 255, 215), Color(95, 255, 255),

    Color(135, 0, 0), Color(135, 0, 95), Color(135, 0, 135),
    Color(135, 0, 175), Color(135, 0, 215), Color(135, 0, 255),

    Color(135, 95, 0), Color(135, 95, 95), Color(135, 95, 135),
    Color(135, 95, 175), Color(135, 95, 215), Color(135, 95, 255),

    Color(135, 135, 0), Color(135, 135, 95), Color(135, 135, 135),
    Color(135, 135, 175), Color(135, 135, 215), Color(135, 135, 255),

    Color(135, 175, 0), Color(135, 175, 95), Color(135, 175, 135),
    Color(135, 175, 175), Color(135, 175, 215), Color(135, 175, 255),

    Color(135, 215, 0), Color(135, 215, 95), Color(135, 215, 135),
    Color(135, 215, 175), Color(135, 215, 215), Color(135, 215, 255),

    Color(135, 255, 0), Color(135, 255, 95), Color(135, 255, 135),
    Color(135, 255, 175), Color(135, 255, 215), Color(135, 255, 255),

    Color(175, 0, 0), Color(175, 0, 95), Color(175, 0, 135),
    Color(175, 0, 175), Color(175, 0, 215), Color(175, 0, 255),

    Color(175, 95, 0), Color(175, 95, 95), Color(175, 95, 135),
    Color(175, 95, 175), Color(175, 95, 215), Color(175, 95, 255),

    Color(175, 135, 0), Color(175, 135, 95), Color(175, 135, 135),
    Color(175, 135, 175), Color(175, 135, 215), Color(175, 135, 255),

    Color(175, 175, 0), Color(175, 175, 95), Color(175, 175, 135),
    Color(175, 175, 175), Color(175, 175, 215), Color(175, 175, 255),

    Color(175, 215, 0), Color(175, 215, 95), Color(175, 215, 135),
    Color(175, 215, 175), Color(175, 215, 215), Color(175, 215, 255),

    Color(175, 255, 0), Color(175, 255, 95), Color(175, 255, 135),
    Color(175, 255, 175), Color(175, 255, 215), Color(175, 255, 255),

    Color(215, 0, 0), Color(215, 0, 95), Color(215, 0, 135),
    Color(215, 0, 175), Color(215, 0, 215), Color(215, 0, 255),

    Color(215, 95, 0), Color(215, 95, 95), Color(215, 95, 135),
    Color(215, 95, 175), Color(215, 95, 215), Color(215, 95, 255),

    Color(215, 135, 0), Color(215, 135, 95), Color(215, 135, 135),
    Color(215, 135, 175), Color(215, 135, 215), Color(215, 135, 255),

    Color(215, 175, 0), Color(215, 175, 95), Color(215, 175, 135),
    Color(215, 175, 175), Color(215, 175, 215), Color(215, 175, 255),

    Color(215, 215, 0), Color(215, 215, 95), Color(215, 215, 135),
    Color(215, 215, 175), Color(215, 215, 215), Color(215, 215, 255),

    Color(215, 255, 0), Color(215, 255, 95), Color(215, 255, 135),
    Color(215, 255, 175), Color(215, 255, 215), Color(215, 255, 255),

    Color(255, 0, 0), Color(255, 0, 95), Color(255, 0, 135),
    Color(255, 0, 175), Color(255, 0, 215), Color(255, 0, 255),

    Color(255, 95, 0), Color(255, 95, 95), Color(255, 95, 135),
    Color(255, 95, 175), Color(255, 95, 215), Color(255, 95, 255),

    Color(255, 135, 0), Color(255, 135, 95), Color(255, 135, 135),
    Color(255, 135, 175), Color(255, 135, 215), Color(255, 135, 255),

    Color(255, 175, 0), Color(255, 175, 95), Color(255, 175, 135),
    Color(255, 175, 175), Color(255, 175, 215), Color(255, 175, 255),

    Color(255, 215, 0), Color(255, 215, 95), Color(255, 215, 135),
    Color(255, 215, 175), Color(255, 215, 215), Color(255, 215, 255),

    Color(255, 255, 0), Color(255, 255, 95), Color(255, 255, 135),
    Color(255, 255, 175), Color(255, 255, 215), Color(255, 255, 255),

    // 24 grayscale Colors
    Color(8, 8, 8), Color(18, 18, 18), Color(28, 28, 28),
    Color(38, 38, 38), Color(48, 48, 48), Color(58, 58, 58),
    Color(68, 68, 68), Color(78, 78, 78), Color(88, 88, 88),
    Color(98, 98, 98), Color(108, 108, 108), Color(118, 118, 118),
    Color(128, 128, 128), Color(138, 138, 138), Color(148, 148, 148),
    Color(158, 158, 158), Color(168, 168, 168), Color(178, 178, 178),
    Color(188, 188, 188), Color(198, 198, 198), Color(208, 208, 208),
    Color(218, 218, 218), Color(228, 228, 228), Color(238, 238, 238)
};

}

#endif
