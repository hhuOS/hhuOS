#include "lib/libc/StdioFileStream.h"
#include "lib/util/io/file/File.h"
#include "lib/util/base/String.h"
#include "lib/interface.h"
#include "lib/util/base/System.h"


namespace Libc {


StdioFileStream::StdioFileStream(const char* filename, FileMode mode) {
	auto file = Util::Io::File(Util::String(filename));
	
	if (mode == WRITE || mode == WRITE_EXTEND) {
		file.create(Util::Io::File::REGULAR);
	}
	
	fileDescriptor = Util::Io::File::open(file.getCanonicalPath());
	if (fileDescriptor < 0) return;
	
	switch (mode) {
		case READ:
			_readAllowed  = true;
			break;
			
		case WRITE:
			_writeAllowed = true;
			break;
			
		case APPEND:
			_writeAllowed = true;
			pos = file.getLength();
			break;
			
		case READ_EXTEND:
			_readAllowed  = true;
			_writeAllowed = true;
			break;
		
		case WRITE_EXTEND:
			_readAllowed  = true;
			_writeAllowed = true;
			break; 
			
		case APPEND_EXTEND:
			_readAllowed  = true;
			_writeAllowed = true;
			pos = file.getLength();
			break; 
	}
}

StdioFileStream::StdioFileStream(int32_t fileDescriptor, bool allowRead, bool allowWrite) {
	this->fileDescriptor = fileDescriptor;
	_readAllowed = allowRead;
	_writeAllowed = allowWrite;
}


StdioFileStream::~StdioFileStream() {
	flush();
	Util::Io::File::close(fileDescriptor);
	if (_freeBufferOnDelete && buffer) delete [] buffer;
}


int StdioFileStream::setBuffer(char* newBuffer, int mode, size_t size) {
	if (!_bufferChangeAllowed) return -1;
	
	if (mode == _IONBF) {
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


int StdioFileStream::fflush() {
	if (!buffer || !writeAllowed()) return EOF;
	
	
	_bufferChangeAllowed = false;
	writeFile(fileDescriptor, (const uint8_t*)buffer, pos, bufferPos);
	pos += bufferPos;
	bufferPos = 0;
	return 0;
}

void StdioFileStream::flush() {
	fflush();
}


int StdioFileStream::fputc(int c) {
	if (!isOpen() || !writeAllowed()) {
		_error = true;
		return EOF;
	}
	
	_bufferChangeAllowed = false;
	if (buffer) {
		
		if (bufferPos == bufferSize) flush(); //flush if buffer is full
		buffer[bufferPos++] = c;
		if (bufferMode == _IOLBF && c == '\n') flush(); //flush if line mode and end of line
		
	} else {
		writeFile(fileDescriptor, (const uint8_t*)&c, pos++, 1);
	}
	
	return 0;
}


void StdioFileStream::write(uint8_t c) {
	fputc(c);
}


void StdioFileStream::write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) {
	if (offset) flush();
	pos += offset;
	
	for (uint32_t i=0; i<length; i++) write(sourceBuffer[i]);
}


int16_t StdioFileStream::read() {
	uint8_t ret;
	_bufferChangeAllowed = false;
	
	if (!readAllowed()) return EOF;
	
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

int32_t StdioFileStream::read(uint8_t *targetBuffer, uint32_t offset, uint32_t length) {
	if (!readAllowed()) return EOF;
	
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
	
	int32_t len = readFile(fileDescriptor, targetBuffer, pos, length);
	pos += len;
	
	if (len < length) _eof = true;
	return len;
}

int StdioFileStream::ungetChar(int ch) {
	ungottenChars.add(ch);
	return ch;
}


uint32_t StdioFileStream::getPos() {
	return pos;
}

void StdioFileStream::setPos(uint32_t newPos, int mode) {
	flush();
	_eof = false;
	ungottenChars.clear();
	
	switch (mode) {
		case SEEK_SET:
			break;
		case SEEK_CUR:
			newPos += pos;
			break;
		case SEEK_END:
			newPos = getFileLength(fileDescriptor) - newPos;
			break;
	}
	
	pos = newPos;
}

void StdioFileStream::clearError() {
	_eof = false;
	_error = false;
}


bool StdioFileStream::readAllowed() {
	return _readAllowed;
}

bool StdioFileStream::writeAllowed() {
	return _writeAllowed;
}

bool StdioFileStream::isError() {
	return _error;
}

bool StdioFileStream::isEOF() {
	return _eof;
}

bool StdioFileStream::isOpen() {
	return fileDescriptor >= 0;
}

}