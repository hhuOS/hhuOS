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

#ifndef __File_include__
#define __File_include__

#include "filesystem/FileSystem.h"
#include "filesystem/FsNode.h"
#include "kernel/Kernel.h"
#include "lib/InputStream.h"
#include <cstdint>

/**
 * Allows reading from and writing to a file.
 *
 * Call File::open() to open a file.
 * To close a file, just delete the pointer.
 *
 * @author Fabian Ruhland
 * @date 2017
 */
class File : public OutputStream, public InputStream {

private:
    Util::SmartPointer<FsNode> node;
    String path;
    String mode;
    uint64_t pos = 0;

    /**
     * Constructor.
     *
     * @param node The node, representing the file
     * @param path The absolute path, that points to the file
     * @param mode The operating mode (See File::open() for more details)
     */
    File(Util::SmartPointer<FsNode> node, const String &path, const String &mode);

public:
    /**
     * Possible origins for File::setPos().
     */
    enum SEEK_ORIGIN {
        START = 0x00,
        CURRENT = 0x01,
        END = 0x02
    };

    /**
     * Default-constructor.
     */
    File() = delete;

    /**
     * Copy-constructor.
     */
    File(const File &copy) = delete;

    /**
     * Destructor.
     */
    ~File() override = default;

    /**
     * Open a file.
     * CAUTION: May return nullptr, if the file does not exist.
     *          Always check the return value!
     *
     * @param path The absolute path, that points to the file
     * @param mode The desired mode:
     *             "r":  Reading only.
     *             "w":  Writing only; Creates the file, if it does not exist; Starts writing at the file's beginning.
     *             "a":  Writing only; Creates the file, if it does not exist; Always appends to the file's end.
     *             "r+": Reading and writing; Starts writing at the file's beginning.
     *             "w+": Reading and writing; Truncates the file to zero length, if it already exists;
     *                   Creates the file, if it does not exist; Starts writing at the file's beginning.
     *             "a+": Reading and writing; Creates the file, if it does not exist; Starts reading from the file's beginning;
     *                   Always appends to the file's end.
     *
     * @return The file (or nulltpr on failure)
     */
    static File *open(const String &path, const String &mode);

    /**
     * Get the filename.
     */
    String getName();

    /**
     * Get the absolute path, that points to the file
     */
    String getAbsolutePath();

    /**
     * Get the file type.
     */
    uint32_t getFileType();

    /**
     * Get the file's length in bytes.
     */
    uint64_t getLength();

    /**
     * Write a single character to the file.
     *
     * @param ch The character
     *
     * @return The amount of actually written bytes
     */
    uint64_t writeChar(char c);

    /**
     * Write a null-terminated string to the file.
     *
     * @param string The string
     *
     * @return The amount of actually written bytes
     */
    uint64_t writeString(char *string);

    /**
     * Write a given amount of bytes to the file.
     *
     * @param data The data-buffer
     * @param len The buffer's length
     *
     * @return The amount of actually written bytes
     */
    uint64_t writeBytes(char *data, uint64_t len);

    /**
     * Read a single character from the file.
     *
     * @return The char
     */
    char readChar();

    /**
     * Read at most len - 1 bytes from the file and always appends a null-terminator.
     * If a '\n' or 'EOF' character is encountered, the function returns the read string up to that point.
     *
     * @param buf The buffer to write to (Needs to be allocated already!)
     * @param len The amount of bytes ot read. At most, len - 1 characters will be read.
     *
     * @return The amount of actually read bytes
     */
    uint64_t readString(char *buf, uint64_t len);

    /**
     * Read a given amount of bytes from the file.
     *
     * @param buf The buffer to write to (Needs to be already allocated already!)
     * @param len The amount of bytes ot read.
     *
     * @return The amount of actually read bytes
     */
    uint64_t readBytes(char *buf, uint64_t len);

    /**
     * Get the current position.
     */
    uint64_t getPos();

    /**
     * Set the position. This determines, which character will be read next.
     *
     * @param offset Amount of bytes to offset from 'origin'.
     * @param origin File::START: The beginning of the file;
     *               File::CURRENT: The current position;
     *               File::END: The end of the file.
     */
    void setPos(uint64_t offset, uint32_t origin = START);

    /**
     * Overriding function from OutputStream.
     */
    void flush() override;

    /**
     * Overriding function from InputStream.
     */
    InputStream& operator >> (char &c) override;

    /**
     * Overriding function from InputStream.
     */
    InputStream& operator >> (char *&string) override;

    /**
     * Overriding function from InputStream.
     */
    InputStream& operator >> (OutputStream &outStream) override;
};

#endif