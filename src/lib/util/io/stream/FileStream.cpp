#include "lib/util/io/stream/FileStream.h"
#include "lib/libc/errno.h"
#include "lib/util/io/file/File.h"
#include "lib/util/base/String.h"
#include "lib/interface.h"
#include "lib/util/base/System.h"


namespace Util::Io {


FileStream::FileStream(const char* filename, FileMode mode) {
	auto file = File(Util::String(filename));
	
	if (file.exists() && file.isDirectory()) {
		//errno = EISDIR; //TODO errno
		_error = true;
		return;
	}
	
	if (mode == FileMode::WRITE || mode == FileMode::WRITE_EXTEND) {
		file.create(File::REGULAR);
	}
	
	fileDescriptor = File::open(file.getCanonicalPath());
	
	if (fileDescriptor < 0) return;
	
	switch (mode) {
		case FileMode::READ:
			_readAllowed  = true;
			break;
			
		case FileMode::WRITE:
			_writeAllowed = true;
			break;
			
		case FileMode::APPEND:
			_writeAllowed = true;
			pos = file.getLength();
			break;
			
		case FileMode::READ_EXTEND:
			_readAllowed  = true;
			_writeAllowed = true;
			break;
		
		case FileMode::WRITE_EXTEND:
			_readAllowed  = true;
			_writeAllowed = true;
			break; 
			
		case FileMode::APPEND_EXTEND:
			_readAllowed  = true;
			_writeAllowed = true;
			pos = file.getLength();
			break; 
	}
}

FileStream::FileStream(int32_t fileDescriptor, bool allowRead, bool allowWrite) {
	this->fileDescriptor = fileDescriptor;
	_readAllowed = allowRead;
	_writeAllowed = allowWrite;
}


FileStream::~FileStream() {
	flush();
	File::close(fileDescriptor);
	if (_freeBufferOnDelete && buffer) delete [] buffer;
}


int FileStream::setBuffer(char* newBuffer, BufferMode mode, size_t size) {
	if (!_bufferChangeAllowed || isError()) return -1;
	
	if (mode == BufferMode::NONE) {
		buffer = NULL;
		return 0;
	}
	
	if (!newBuffer) {
		buffer = (char*) new uint8_t[size];
		_freeBufferOnDelete = true;
	} else {
		buffer = newBuffer;
		this->bufferSize = size;
	}
	
	bufferMode = mode;
	bufferSize = size;
	bufferPos = 0;
	
	_bufferChangeAllowed = false;
	return 0;
}


int FileStream::fflush() {
	if (!buffer || !writeAllowed() || isError()) return EOF;
	
	
	_bufferChangeAllowed = false;
	writeFile(fileDescriptor, (const uint8_t*)buffer, pos, bufferPos);
	pos += bufferPos;
	bufferPos = 0;
	return 0;
}

void FileStream::flush() {
	fflush();
}


int FileStream::fputc(int c) {
	if (!isOpen() || !writeAllowed() || isError()) {
		_error = true;
		return EOF;
	}
	
	_bufferChangeAllowed = false;
	if (buffer) {
		
		if (bufferPos == bufferSize) flush(); //flush if buffer is full
		buffer[bufferPos++] = c;
		if (bufferMode == BufferMode::LINE && c == '\n') flush(); //flush if line mode and end of line
		
	} else {
		writeFile(fileDescriptor, (const uint8_t*)&c, pos++, 1);
	}
	
	return 0;
}


void FileStream::write(uint8_t c) {
	fputc(c);
}


void FileStream::write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) {
	if (isError() || !writeAllowed()) return;
	if (offset) flush();
	pos += offset;
	
	for (uint32_t i=0; i<length; i++) write(sourceBuffer[i]);
}


int16_t FileStream::read() {
	uint8_t ret;
	_bufferChangeAllowed = false;
	
	if (!readAllowed() || isError()) return EOF;
	
	if (!ungottenChars.isEmpty()) {
		return ungottenChars.pop();
	}
	
	int32_t len = readFile(fileDescriptor, &ret, pos++, 1);
	if (len == 0) {
		_eof = true;
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
	if (!readAllowed() || isError()) return EOF;
	
	pos += offset;
	_bufferChangeAllowed = false;
	
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
	
	if (len < length) _eof = true;
	return len;
}

int FileStream::ungetChar(int ch) {
	ungottenChars.add(ch);
	return ch;
}


uint32_t FileStream::getPos() {
	return pos;
}

void FileStream::setPos(uint32_t newPos, SeekMode mode) {
	flush();
	_eof = false;
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
	_eof = false;
	_error = false;
}


bool FileStream::readAllowed() {
	return _readAllowed;
}

bool FileStream::writeAllowed() {
	return _writeAllowed;
}

bool FileStream::isError() {
	return _error;
}

bool FileStream::isEOF() {
	return _eof;
}

bool FileStream::isOpen() {
	return fileDescriptor >= 0;
}

bool FileStream::setAccessMode(File::AccessMode accessMode) const {
    return File::setAccessMode(fileDescriptor, accessMode);
}


}