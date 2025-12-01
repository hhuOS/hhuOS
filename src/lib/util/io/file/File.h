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

#ifndef HHUOS_LIB_UTIL_IO_FILE_H
#define HHUOS_LIB_UTIL_IO_FILE_H

#include <stdint.h>
#include <stddef.h>

#include "util/base/String.h"
#include "util/collection/Array.h"

namespace Util::Io {

/// The standard output file descriptor.
static constexpr int32_t STANDARD_INPUT = 0;
/// The standard input file descriptor.
static constexpr int32_t STANDARD_OUTPUT = 1;
/// The standard error file descriptor.
static constexpr int32_t STANDARD_ERROR = 2;

/// Representation of a file in the file system.
/// This class provides methods to interact with files and directories,
/// such as checking their existence, type, and length, as well as creating,
/// removing, and listing files and directories.
/// It uses a file descriptor internally to perform operations on the file.
/// The file descriptor is opened when needed and closed when the `File` object is destroyed.
/// Reading and writing cannot be done directly via this class,
/// but must be performed via `FileInputStream` and `FileOutputStream`.
class File {

public:
    /// Enumeration of possible file types.
    enum Type {
        /// A regular file (e.g., a text file or binary file).
        /// Regular files can be read from and written to, and always have a valid length.
        REGULAR,
        /// A directory file (e.g., a folder).
        /// Directory files can contain other files and directories, but cannot be read from or written to.
        /// The length of a directory file is always zero.
        /// Use `getChildren()` to list the files and directories contained in a directory file.
        DIRECTORY,
        /// A character device file (e.g., a terminal or serial port).
        /// Character device files can be read from and written to, but do not have a valid length.
        /// They provide an endless stream of data and their length is always zero.
        CHARACTER,
        /// System files (e.g. shared memory files) cannot be read from or written to and their length is always zero.
        /// They are only meant to be accessed via `controlFile()` requests.
        SYSTEM
    };

    /// Files can either be accessed blocking or non-blocking.
    /// Blocking reads will return only after at least one byte has been read,
    /// while non-blocking reads will return immediately, even if no bytes were read.
    enum AccessMode {
        /// Blocking access mode.
        BLOCKING,
        /// Non-blocking access mode.
        NON_BLOCKING
    };

    /// Requests to manipulate the behavior of a file descriptor.
    enum FileDescriptorRequest {
        /// Set the access mode of the file descriptor (blocking or non-blocking, see `AccessMode`).
        SET_ACCESS_MODE,
        /// Check if the file descriptor currently has data available to read.
        /// This is for example useful for network sockets, where data can arrive at any time.
        IS_READY_TO_READ
    };

    /// Seek modes for file position manipulation.
    /// This is used by `FileInputStream` and `FileOutputStream`.
    enum class SeekMode {
        /// Set the file position to an absolute offset (from the beginning of the file).
        SET,
        /// Move the file position relative to the current position.
        CURRENT,
        /// Move the file position relative to the end of the file.
        END
    };

    /// Create a new file instance without a path (always referring to the root directory "/").
    /// This is mostly useless and just provided, so that data structures like arrays can be created of files.
    ///
    /// ### Example
    /// ```c++
    /// const auto file = Util::Io::File();
    /// const auto exists = file.exists(); // true, because "/" does always exist
    /// const auto name = file.getName(); // "", because the root directory has no name
    /// const auto type = file.getType(); // Util::Io::File::DIRECTORY
    /// ```
    File() = default;

    /// Create a new file instance with the given path.
    /// The path can be either absolute (starting with '/') or relative to the current working directory.
    ///
    /// ### Example
    /// ```c++
    /// const auto file = Util::Io::File("/device/random");
    /// const auto name = file.getName(); // random
    /// const auto type = file.getType(); // Util::Io::File::CHARACTER
    /// const auto length = file.getLength(); // 0, because character files have no valid length
    ///
    /// const auto parent = file.getParent();
    /// const auto parentName = parent.getName(); // device
    /// const auto parentType = parent.getType(); // Util::Io::File::DIRECTORY
    /// const auto parentLength = parent.getLength(); // 0, because directories have no valid length
    /// ```
    explicit File(const String &path);

    /// Create a new file instance from an existing one (copy constructor).
    /// The new instance will refer to the same path as the original one,
    /// but will have its own file descriptor, which is opened when needed.
    File(const File &copy);

    /// Assign the given file to this file, overwriting the existing path.
    /// The file descriptor is not copied, but will be opened when needed.
    File& operator=(const File &other);

    /// Destroy the file instance and close the file descriptor if it is open.
    ~File();

    /// Check if the file exists in the file system.
    /// This will return true for any valid path, pointing to a file or directory.
    ///
    /// ### Example
    /// ```c++
    /// const auto file1 = Util::Io::File("/device/random");
    /// const auto exists1 = file1.exists(); // true, because "/device/random" exists
    ///
    /// const auto file2 = Util::Io::File("/device/nonexistent");
    /// const auto exists2 = file2.exists(); // false, because "/device/nonexistent" does not exist
    /// ```
    bool exists() const;

    /// Get the type of the file (regular, directory, character, or system).
    /// If the file does not exist, a panic is fired.
    ///
    /// ### Example
    /// ```c++
    /// const auto file1 = Util::Io::File("/device/random");
    /// const auto type1 = file1.getType(); // Util::Io::File::CHARACTER
    ///
    /// const auto file2 = Util::Io::File("/bin/ls");
    /// const auto type2 = file2.getType(); // Util::Io::File::REGULAR
    ///
    /// const auto file3 = Util::Io::File("/user");
    /// const auto type3 = file3.getType(); // Util::Io::File::DIRECTORY
    ///
    /// const auto file4 = Util::Io::File("/device/nonexistent");
    /// const auto type4 = file4.getType(); // Panic: Could not open file!
    /// ```
    Type getType() const;

    /// Check if the file is a file (not a directory).
    /// If the file does not exist, a panic is fired.
    ///
    /// ### Example
    /// ```c++
    /// const auto file1 = Util::Io::File("/device/random");
    /// const auto isFile1 = file1.isFile(); // true, because "/device/random" is a character file
    ///
    /// const auto file2 = Util::Io::File("/user");
    /// const auto isFile2 = file2.isFile(); // false, because "/user" is a directory
    ///
    /// const auto file3 = Util::Io::File("/device/nonexistent");
    /// const auto isFile3 = file3.getType(); // Panic: Could not open file!
    /// ```
    bool isFile() const;

    /// Check if the file is a directory.
    /// If the file does not exist, a panic is fired.
    ///
    /// ### Example
    /// ```c++
    /// const auto file1 = Util::Io::File("/device/random");
    /// const auto isDirectory1 = file1.isDirectory(); // false, because "/device/random" is a character file
    ///
    /// const auto file2 = Util::Io::File("/user");
    /// const auto isDirectory2 = file2.isDirectory(); // true, because "/user" is a directory
    ///
    /// const auto file3 = Util::Io::File("/device/nonexistent");
    /// const auto isDirectory3 = file3.getType(); // Panic: Could not open file!
    /// ```
    bool isDirectory() const;

    /// Get the length of the file in bytes.
    /// Only files of type `REGULAR` have a valid length.
    /// For all other file types, the length is always zero.
    /// If the file does not exist, a panic is fired.
    ///
    /// ### Example
    /// ```c++
    /// const auto directory = Util::Io::File("/user");
    /// const auto dirLength = directory.getLength(); // 0, because directories have no valid length
    ///
    /// const auto charFile = Util::Io::File("/device/random");
    /// const auto charFileLength = charFile.getLength(); // 0, because character files have no valid length
    ///
    /// auto file = Util::Io::File("/user/test.txt"); // Assuming this file does not exist
    /// if (!file.create(Util::Io::File::REGULAR)) {
    ///     Util::System::out << "Failed to create file!"
    ///         << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
    ///     return;
    /// }
    ///
    /// auto length = file.getLength(); // 0, because the file is still empty
    ///
    /// // Write some data to the file via a `FileOutputStream`
    /// auto outputStream = Util::Io::FileOutputStream(file);
    /// outputStream.write(reinterpret_cast<const uint8_t*>("Hello, World!"), 0, 13);
    ///
    /// length = file.getLength(); // 13, because we wrote 13 bytes to the file
    /// ```
    size_t getLength() const;

    /// Get the name of the file (the last component of the path).
    /// This method does not check if the file exists. In fact, it does not even access the file system.
    /// It simply extracts the name from the path string, that was used to create the `File` object.
    ///
    /// ### Example
    /// ```c++
    /// const auto file1 = Util::Io::File("/device/random");
    /// const auto name1 = file1.getName(); // random
    ///
    /// const auto file2 = Util::Io::File("/user/documents/");
    /// const auto name2 = file2.getName(); // documents
    /// ```
    String getName() const;

    /// Get the absolute path that points to this file.
    /// If the File object was created with a relative path, the current working directory is used to resolve it.
    /// This method does not check if the file exists. In fact, it does not even access the file system.
    /// It simply resolves the path string, that was used to create the `File` object.
    ///
    /// ### Example
    /// ```c++
    /// const auto file1 = Util::Io::File("/device/random");
    /// const auto path1 = file1.getCanonicalPath(); // /device/random
    ///
    /// const auto file2 = Util::Io::File("documents/report.txt"); // Assuming the current working directory is "/user"
    /// const auto path2 = file2.getCanonicalPath(); // /user/documents/report.txt
    ///
    /// const auto file3 = Util::Io::File("../bin/ls");
    /// const auto path3 = file3.getCanonicalPath(); // /bin/ls
    ///
    /// const auto file4 = Util::Io::File("/user/./documents/../pictures/./image.png");
    /// const auto path4 = file4.getCanonicalPath(); // /user/pictures/image.png
    /// ```
    String getCanonicalPath() const;

    /// Get the parent directory of this file.
    /// If the file is the root directory ("/"), the parent is itself.
    /// This method does not check if the file exists. In fact, it does not even access the file system.
    /// It simply uses the path string, that was used to create the `File` object to determine the parent path.
    ///
    /// ### Example
    /// ```c++
    /// const auto file1 = Util::Io::File("/device/random");
    /// const auto parent1 = file1.getParent(); // /device
    ///
    /// const auto file2 = Util::Io::File("/user/nonexistent/test.txt");
    /// const auto parent2 = file2.getParent(); // /user/nonexistent (Does not exist, but is not accessed)
    /// const auto parent2Length = parent2.getLength(); // Panic: Could not open file!
    ///
    /// const auto file3 = Util::Io::File("/user/documents/important/report.txt");
    /// const auto parent3 = file3.getParent(); // /user/documents/important
    /// const auto grandParent3 = parent3.getParent(); // /user/documents
    /// const auto greatGrandParent3 = grandParent3.getParent(); // /user
    /// ```
    File getParent() const;

    /// Get the files and directories contained in this directory.
    /// If the file is not a directory, an empty array is returned.
    /// If the directory does not exist, a panic is fired.
    ///
    /// ### Example
    /// ```c++
    /// const auto directory = Util::Io::File("/user");
    /// const auto children = directory.getChildren();
    ///
    /// // Print the names of all the files and directories contained in "/user"
    /// for (const auto &child : children) {
    ///     Util::System::out << child.getName() << Util::Io::PrintStream::ln;
    /// }
    ///
    /// Util::System::out << Util::Io::PrintStream::flush;
    /// ```
    Array<File> getChildren() const;

    /// Create the file or directory represented by this `File` object.
    /// On success, true is returned.
    /// If the file or directory already exists, false is returned.
    /// If the parent directory does not exist, false is returned.
    /// If the creation fails for any other reason, false is returned.
    ///
    /// ### Example
    /// ```c++
    /// auto directory = Util::Io::File("/user/testdir");
    /// if (!directory.create(Util::Io::File::DIRECTORY)) {
    ///     Util::System::out << "Failed to create directory!"
    ///         << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
    ///     return;
    /// }
    ///
    /// auto file = Util::Io::File("/user/testdir/test.txt");
    /// if (!file.create(Util::Io::File::REGULAR)) {
    ///     Util::System::out << "Failed to create file!"
    ///         << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
    ///     return;
    /// }
    /// ```
    bool create(Type fileType);

    /// Remove the file or directory represented by this `File` object.
    /// On success, true is returned.
    /// If the file or directory does not exist, false is returned.
    /// If the file is a directory and is not empty, false is returned.
    ///
    /// ### Example
    /// ```c++
    /// auto directory = Util::Io::File("/user/testdir");
    /// if (!directory.create(Util::Io::File::DIRECTORY)) {
    ///     Util::System::out << "Failed to create directory!"
    ///         << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
    ///     return;
    /// }
    ///
    /// auto file = Util::Io::File("/user/testdir/test.txt");
    /// if (!file.create(Util::Io::File::REGULAR)) {
    ///     Util::System::out << "Failed to create file!"
    ///         << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
    ///     return;
    /// }
    ///
    /// auto success = directory.remove(); // false, because the directory is not empty
    /// success = file.remove(); // true, because the file was removed successfully
    /// success = directory.remove(); // true, because the directory is now empty and was removed successfully
    ///
    /// if (!success) {
    ///     Util::System::out << "Failed to remove file or directory!"
    ///         << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
    ///     return;
    /// }
    /// ```
    bool remove();

    /// Issue a control request to the file.
    /// Control requests are used to manipulate special files, such as character devices or system files.
    /// The available requests and their parameters are specific to the file type and driver.
    /// If the file does not exist, a panic is fired.
    ///
    /// ### Example
    /// ```c++
    /// auto file = Util::Io::File("/device/terminal");
    ///
    /// // Disable cursor in terminal
    /// if (!file.controlFile(Util::Graphic::Terminal::SET_CURSOR, Util::Array<size_t>({false}))) {
    ///     Util::System::out << "Failed to disable cursor!"
    ///         << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
    /// }
    /// ```
    bool controlFile(size_t request, const Array<size_t> &parameters);

    /// Resolve the given path to its canonical absolute form.
    /// If the path is relative, it is resolved against the current working directory.
    /// This method does not check if the path exists. In fact, it does not even access the file system.
    /// It simply resolves the path string.
    ///
    /// ### Example
    /// ```c++
    ///  // /device/random
    /// const auto path1 = Util::Io::File::getCanonicalPath("/device/random");
    ///
    /// // Assuming the current working directory is "/user", this results in "/user/documents/report.txt"
    /// const auto path2 = Util::Io::File::getCanonicalPath("documents/report.txt");
    ///
    ///  // /bin/ls
    /// const auto path3 = Util::Io::File::getCanonicalPath("../bin/ls");
    ///
    ///  // /user/pictures/image.png
    /// const auto path4 = Util::Io::File::getCanonicalPath("/user/./documents/../pictures/./image.png");
    /// ```
    static String getCanonicalPath(const String &path);

    /// Get the current working directory as a `File` object.
    /// The current working directory is a property of the process and is used to resolve relative paths.
    /// It can be changed via `changeDirectory()`.
    /// Each process starts with the root directory ("/") as its current working directory.
    ///
    /// ### Example
    /// ```c++
    /// if (!Util::Io::File::changeDirectory("/user")) {
    ///     Util::System::out << "Failed to change directory!"
    ///         << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
    ///     return;
    /// }
    ///
    /// auto cwd = Util::Io::File::getCurrentWorkingDirectory();
    /// auto cwdPath = cwd.getCanonicalPath(); // /user
    /// ```
    static File getCurrentWorkingDirectory();

    /// Change the current working directory of the process to the given path.
    /// On success, true is returned.
    /// If the path does not exist or is not a directory, false is returned.
    ///
    /// ### Example
    /// ```c++
    /// if (!Util::Io::File::changeDirectory("/user")) {
    ///     Util::System::out << "Failed to change directory!"
    ///         << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
    ///     return;
    /// }
    ///
    /// auto cwd = Util::Io::File::getCurrentWorkingDirectory();
    /// auto cwdPath = cwd.getCanonicalPath(); // /user
    /// ```
    static bool changeDirectory(const String &path);

    /// Open a file and return its file descriptor (or -1 on failure).
    /// This method is intended to be user internally by the `File` class
    /// and other classes that operate directly on file descriptors,
    /// such as `FileInputStream` and `FileOutputStream`.
    /// Most applications should not need to use this method directly.
    ///
    /// ### Example
    /// ```c++
    /// const auto fd = Util::Io::File::open("/device/random");
    /// if (fd < 0) {
    ///     Util::System::out << "Failed to open file!"
    ///         << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
    ///     return;
    /// }
    /// ```
    static int32_t open(const String &path);

    /// Issue a control request to the file identified by the given file descriptor.
    /// This is a static version of the non-static `controlFile()` method.
    /// It can for example be used to issue control requests to standard input, output, and error.
    ///
    /// ```c++
    ///  // Disable cursor in terminal (only works if /device/terminal is the standard output)
    /// const auto success = Util::Io::File::controlFile(Util::Io::STANDARD_OUTPUT,
    ///     Util::Graphic::Terminal::SET_CURSOR, Util::Array<size_t>({false}));
    ///
    /// if (!success) {
    ///     Util::System::out << "Failed to disable cursor!"
    ///         << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
    /// }
    /// ```
    static bool controlFile(int32_t fileDescriptor, size_t request, const Array<size_t> &parameters);

    /// Issue a control request to the file identified by the given file descriptor.
    /// This method can for example be used to set the access mode (blocking or non-blocking)
    /// or to check if the file descriptor is ready to read.
    /// It is recommended to use the convenience methods `setAccessMode()` and `isReadyToRead()`
    /// instead of calling this method directly.
    ///
    /// ```c++
    /// // Set standard input to non-blocking mode
    /// const auto success = Util::Io::File::controlFileDescriptor(Util::Io::STANDARD_INPUT,
    ///     Util::Io::File::SET_ACCESS_MODE, Util::Array<size_t>({Util::Io::File::NON_BLOCKING}));
    ///
    /// if (!success) {
    ///     Util::System::out << "Failed to set access mode!"
    ///         << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
    /// }
    /// ```
    static bool controlFileDescriptor(int32_t fileDescriptor, FileDescriptorRequest request, const Array<size_t> &parameters);

    /// Set the access mode (blocking or non-blocking) of the given file descriptor.
    /// Note that this only affects the given file descriptor and not the underlying file itself.
    /// This means that if the same file is opened multiple times, each file descriptor can have
    /// a different access mode.
    ///
    /// ```c++
    /// // Set standard input to non-blocking mode
    /// const auto success = Util::Io::File::setAccessMode(Util::Io::STANDARD_INPUT, Util::Io::File::NON_BLOCKING);
    /// if (!success) {
    ///     Util::System::out << "Failed to set access mode!"
    ///         << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
    /// }
    /// ```
    static bool setAccessMode(int32_t fileDescriptor, AccessMode accessMode);

    /// Check if the given file descriptor is ready to read.
    /// This is for example useful for network sockets, where data can arrive at any time.
    /// If the file descriptor is not valid, a panic is fired.
    ///
    /// ```c++
    /// // Set standard input to non-blocking mode
    /// const auto success = Util::Io::File::setAccessMode(Util::Io::STANDARD_INPUT, Util::Io::File::NON_BLOCKING);
    /// if (!success) {
    ///     Util::System::out << "Failed to set access mode!"
    ///         << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
    /// }
    ///
    /// char buffer[128];
    /// size_t bytesRead = 0;
    ///
    /// // Read data from standard input, while available (non-blocking)
    /// while (Util::Io::File::isReadyToRead(Util::Io::STANDARD_INPUT) && bytesRead < sizeof(buffer) - 1) {
    ///      buffer[bytesRead++] = static_cast<char>(Util::System::in.read());
    /// }
    ///
    /// buffer[bytesRead] = '\0'; // Null-terminate the string
    ///
    /// // Print the read data
    /// Util::System::out << "Read: " << buffer << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
    /// ```
    static bool isReadyToRead(int32_t fileDescriptor);

    /// Close the given file descriptor.
    /// This method is intended to be used internally by the `File` class
    /// and other classes that operate directly on file descriptors,
    /// such as `FileInputStream` and `FileOutputStream`.
    /// Most applications should not need to use this method directly.
    /// If the file descriptor does not refer to an open file, nothing happens.
    /// If the file descriptor is negative, a panic is fired.
    ///
    /// ### Example
    /// ```c++
    /// // Open a file
    /// const auto fd = Util::Io::File::open("/device/random");
    /// if (fd < 0) {
    ///     Util::System::out << "Failed to open file!"
    ///         << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
    ///     return;
    /// }
    ///
    /// // Close the file
    /// Util::Io::File::close(fd);
    /// ```
    static void close(int32_t fileDescriptor);

    /// Mount a file system driver to a device at the given target path.
    /// On success, true is returned.
    /// If the device or target path does not exist, false is returned.
    /// If the target path is already used as a mount point, false is returned.
    /// If any other error occurs, false is returned.
    static bool mount(const String &device, const String &targetPath, const String &driverName);

    /// Unmount the file system driver mounted at the given path.
    /// On success, true is returned.
    /// If the path does not exist or is not a mount point, or any other error occurs, false is returned.
    static bool unmount(const String &path);

private:

    /// If the file descriptor is not opened yet (-1), try to open it.
    /// If the file cannot be opened, a panic is fired.
    /// This method is used internally by the `File` class to lazily open the file descriptor
    /// when it is needed for the first time.
    void ensureFileIsOpened() const;

    String path;
    mutable int32_t fileDescriptor = -1;
};

}

#endif
