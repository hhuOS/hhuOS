#include "lib/util/io/stream/FileStream.h"

#include <stdio.h>

#include "lib/util/io/file/File.h"
#include "lib/util/base/String.h"
#include "lib/interface.h"

namespace Util::Io {

FileStream::FileStream(const char* filename, FileMode mode) {
	auto file = File(Util::String(filename));
	
	if (file.exists() && file.isDirectory()) {
		// errno = EISDIR; // TODO errno
		error = true;
		return;
	}
	
	if (mode == FileMode::WRITE || mode == FileMode::WRITE_EXTEND) {
		file.create(File::REGULAR);
	}
	
	fileDescriptor = File::open(file.getCanonicalPath());
	
	if (fileDescriptor < 0) {
        error = true;
        return;
    }
	
	switch (mode) {
		case FileMode::READ:
            readAllowed  = true;
			break;
		case FileMode::WRITE:
            writeAllowed = true;
			break;
		case FileMode::APPEND:
            writeAllowed = true;
			pos = file.getLength();
			break;
		case FileMode::READ_EXTEND:
            readAllowed  = true;
            writeAllowed = true;
			break;
        case FileMode::WRITE_EXTEND:
            readAllowed  = true;
            writeAllowed = true;
            break;
		case FileMode::APPEND_EXTEND:
            readAllowed  = true;
            writeAllowed = true;
			pos = file.getLength();
			break; 
	}
}

FileStream::FileStream(int32_t fileDescriptor, bool allowRead, bool allowWrite) : fileDescriptor(fileDescriptor), readAllowed(allowRead), writeAllowed(allowWrite) {}

FileStream::~FileStream() {
	flush();
	File::close(fileDescriptor);

	if (freeBufferOnDelete && buffer) {
        delete[] buffer;
    }
}

int FileStream::setBuffer(char* newBuffer, BufferMode mode, size_t size) {
	if (!bufferChangeAllowed || isError()) return -1;
	
	if (mode == BufferMode::NONE) {
		buffer = nullptr;
		return 0;
	}
	
	if (newBuffer == nullptr) {
		buffer = new uint8_t[size];
        freeBufferOnDelete = true;
	} else {
		buffer = reinterpret_cast<uint8_t*>(newBuffer);
		this->bufferSize = size;
	}
	
	bufferMode = mode;
	bufferSize = size;
	bufferPos = 0;

    bufferChangeAllowed = false;
	return 0;
}

int FileStream::fflush() {
	if (buffer == nullptr || !isWriteAllowed() || isError()) {
        return EOF;
    }

    bufferChangeAllowed = false;
	writeFile(fileDescriptor, (const uint8_t*)buffer, pos, bufferPos);
	pos += bufferPos;
	bufferPos = 0;

	return 0;
}

void FileStream::flush() {
	fflush();
}

int FileStream::fputc(int c) {
	write(c);
	return error ? EOF : c;
}

void FileStream::write(uint8_t c) {
	write(&c, 0, 1);
}

void FileStream::write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) {
    if (!isOpen() || !isWriteAllowed() || isError()) {
        error = true;
    }

    if (offset > 0) {
        flush();
        pos += offset;
    }

    bufferChangeAllowed = false;
    if (buffer) {
        for (uint32_t i = 0; i < length; i++) {
            if (bufferPos >= bufferSize) {
                flush(); // Flush if buffer is full
            }

            auto c = sourceBuffer[i];
            buffer[bufferPos++] = c;

            if (bufferMode == BufferMode::LINE && c == '\n') {
                flush(); // Flush if line mode and end of line
            }
        }
    } else {
        pos += writeFile(fileDescriptor, sourceBuffer, pos, length);
    }
}

int16_t FileStream::read() {
	uint8_t ret;
    bufferChangeAllowed = false;
	
	if (!isReadAllowed() || isError()) {
        return EOF;
    }
	
	if (!ungottenChars.isEmpty()) {
		return ungottenChars.pop();
	}
	
	int32_t len = readFile(fileDescriptor, &ret, pos++, 1);
	if (len == 0) {
        eof = true;
		return EOF;
	} 
	
	return ret;
}

int16_t FileStream::peek() {
	int16_t ret = read();
	ungetChar(ret);

	return ret;
}

bool FileStream::isReadyToRead() {
	return Util::Io::File::isReadyToRead(fileDescriptor);
}

int32_t FileStream::read(uint8_t *targetBuffer, uint32_t offset, uint32_t length) {
	if (!isReadAllowed() || isError()) {
        return EOF;
    }
	
	pos += offset;
    bufferChangeAllowed = false;
	
	if (!offset) {
		uint32_t i = 0;
		for (; i<length && !ungottenChars.isEmpty(); i++) {
			targetBuffer[i] = ungottenChars.pop();
		}
		targetBuffer += i;
		length -= i;
	}
	
	uint32_t len = readFile(fileDescriptor, targetBuffer, pos, length);
	pos += len;
	
	if (len < length) eof = true;
	return len;
}

int FileStream::ungetChar(int ch) {
	ungottenChars.add(ch);
	return ch;
}

uint32_t FileStream::getPos() const {
	return pos;
}

void FileStream::setPos(uint32_t newPos, SeekMode mode) {
	flush();
    eof = false;
	ungottenChars.clear();
	
	switch (mode) {
		case SeekMode::SET:
			break;
		case SeekMode::CURRENT:
			newPos += pos;
			break;
		case SeekMode::END:
			newPos = getFileLength(fileDescriptor) - newPos;
			break;
	}
	
	pos = newPos;
}

void FileStream::clearError() {
    eof = false;
    error = false;
}


bool FileStream::isReadAllowed() const {
	return readAllowed;
}

bool FileStream::isWriteAllowed() const {
	return writeAllowed;
}

bool FileStream::isError() const {
	return error;
}

bool FileStream::isEOF() const {
	return eof;
}

bool FileStream::isOpen() const {
	return fileDescriptor >= 0;
}

bool FileStream::setAccessMode(File::AccessMode accessMode) const {
    return File::setAccessMode(fileDescriptor, accessMode);
}

}