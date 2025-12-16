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
 *
 * The stanard C library is based on a master's thesis, written by Tobias Fabian Oehme.
 * The original source code can be found here: https://github.com/ToboterXP/hhuOS/tree/thesis
 */

#include "stdio.h"

#include "errno.h"
#include "string.h"
#include "interface.h"
#include "util/base/String.h"
#include "util/collection/ArrayList.h"
#include "util/io/file/File.h"
#include "util/io/stream/ByteArrayInputStream.h"
#include "util/io/stream/ScanStream.h"

// A stream that can read from and write to a file.
// Exposes libc compatible functions (fflush, fputc, ungetc, etc.)
class FileStream : public Util::Io::InputStream, public Util::Io::OutputStream {

public:

	enum class FileMode {
		READ,
		WRITE,
		APPEND,
		READ_EXTEND,
		WRITE_EXTEND,
		APPEND_EXTEND
	};

	enum class SeekMode {
		SET,
		CURRENT,
		END
	};

	enum class BufferMode {
		FULL,
		LINE,
		NONE
	};

	explicit FileStream(const char *filename, const FileMode mode) {
		const auto file = Util::Io::File(filename);
		if (file.exists() && file.isDirectory()) {
			// errno = EISDIR; // TODO errno
			error = true;
			return;
		}

		if (mode == FileMode::WRITE || mode == FileMode::WRITE_EXTEND) {
			file.create(Util::Io::File::REGULAR);
		}

		fileDescriptor = Util::Io::File::open(file.getCanonicalPath());
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

	explicit FileStream(const int32_t fileDescriptor, const bool allowRead, const bool allowWrite) :
		fileDescriptor(fileDescriptor), readAllowed(allowRead), writeAllowed(allowWrite) {}

	FileStream(const FileStream &copy) = delete;

	FileStream &operator=(const FileStream &copy) = delete;

	~FileStream() override {
		FileStream::flush();
		Util::Io::File::close(fileDescriptor);

		if (freeBufferOnDelete && buffer) {
			delete[] buffer;
		}
	}

	bool write(const uint8_t c) override {
		return write(&c, 0, 1) == 1;
	}

	uint32_t write(const uint8_t *sourceBuffer, const uint32_t offset, const uint32_t length) override {
		if (!isOpen() || !isWriteAllowed() || isError()) {
			error = true;
		}

		bufferChangeAllowed = false;
		if (buffer) {
			for (uint32_t i = 0; i < length; i++) {
				if (bufferPos >= bufferSize) {
					flush(); // Flush if buffer is full

					if (bufferPos >= bufferSize) {
						return i;
					}
				}

				const auto c = sourceBuffer[offset + i];
				buffer[bufferPos++] = c;

				if (bufferMode == BufferMode::LINE && c == '\n') {
					flush(); // Flush if line mode and end of line
				}
			}

			return length;
		}

		const auto written = writeFile(fileDescriptor, sourceBuffer, pos, length);
		pos += written;

		return written;
	}

	int fputc(const int c) {
		write(c);
		return error ? END_OF_FILE : c;
	}

	uint32_t flush() override  {
		const auto flushed = fflush();
		return flushed > 0 ? static_cast<uint32_t>(flushed) : 0;
	}

	int fflush() {
		if (buffer == nullptr || !isWriteAllowed() || isError()) {
			return END_OF_FILE;
		}

		bufferChangeAllowed = false;
		const auto written = writeFile(fileDescriptor, buffer, pos, bufferPos);
		pos += written;
		bufferPos -= written;

		return static_cast<int>(written);
	}

	int16_t read() override {
		uint8_t ret;
		bufferChangeAllowed = false;

		if (!isReadAllowed() || isError()) {
			return END_OF_FILE;
		}

		if (!ungottenChars.isEmpty()) {
			return static_cast<int16_t>(ungottenChars.removeIndex(ungottenChars.size() - 1));
		}

		const auto len = readFile(fileDescriptor, &ret, pos++, 1);
		if (len == 0) {
			eof = true;
			return END_OF_FILE;
		}

		return ret;
	}

	int16_t peek() override {
		const auto ret = read();
		ungetChar(ret);

		return ret;
	}

	bool isReadyToRead() override {
		return Util::Io::File::isReadyToRead(fileDescriptor);
	}

	int32_t read(uint8_t *targetBuffer, const uint32_t offset, uint32_t length) override {
		if (!isReadAllowed() || isError()) {
			return END_OF_FILE;
		}

		pos += offset;
		bufferChangeAllowed = false;

		if (!offset) {
			uint32_t i = 0;
			for (; i<length && !ungottenChars.isEmpty(); i++) {
				targetBuffer[i] = ungottenChars.removeIndex(ungottenChars.size() - 1);
			}
			targetBuffer += i;
			length -= i;
		}

		const auto len = readFile(fileDescriptor, targetBuffer, pos, length);
		pos += len;

		if (len < length) {
			eof = true;
		}

		return static_cast<int32_t>(len);
	}

	int ungetChar(const int ch) {
		ungottenChars.add(ch);
		return ch;
	}

	bool isReadAllowed() const {
		return readAllowed;
	}

	bool isWriteAllowed() const {
		return writeAllowed;
	}

	bool isError() const {
		return error;
	}

	bool endOfFileReached() const {
		return eof;
	}

	bool isOpen() const {
		return fileDescriptor >= 0;
	}

	uint32_t getPos() const {
		return pos;
	}

	void setPos(uint32_t newPos, const SeekMode mode) {
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

	int setBuffer(char* newBuffer, const BufferMode mode, const size_t size) {
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

	void clearError() {
		eof = false;
		error = false;
	}

	bool setAccessMode(const Util::Io::File::AccessMode accessMode) const {
		return Util::Io::File::setAccessMode(fileDescriptor, accessMode);
	}

	static constexpr int16_t END_OF_FILE = -1;

private:

	int32_t fileDescriptor;
	uint32_t pos = 0; // Next position to be written to, or start of buffer

	bool readAllowed = false;
	bool writeAllowed = false;
	bool error = false;
	bool eof = false;

	BufferMode bufferMode = BufferMode::NONE;
	bool bufferChangeAllowed = true;
	bool freeBufferOnDelete = false;

	uint32_t bufferPos = 0; // Current position inside buffer
	uint32_t bufferSize = 0;
	uint8_t *buffer = nullptr;

	Util::ArrayList<int> ungottenChars;

};

FILE *stdin;
FILE *stdout;
FILE *stderr;

void libc_init_stdio() {
	stdin = reinterpret_cast<FILE*>(new FileStream(Util::Io::STANDARD_INPUT, true, false));
	setvbuf(stdin, nullptr, _IOLBF, BUFSIZ);
	
	stdout = reinterpret_cast<FILE*>(new FileStream(Util::Io::STANDARD_OUTPUT, false, true));
	setvbuf(stdout, nullptr, _IOLBF, BUFSIZ);
	
	stderr = reinterpret_cast<FILE*>(new FileStream(Util::Io::STANDARD_ERROR, false, true));
}

FILE* fopen(const char *filename, const char *mode) {
	auto fmode = FileStream::FileMode::READ;
	
	for (;*mode != '\0'; mode++) {
		switch (*mode) {
			case 'r':
				fmode = FileStream::FileMode::READ;
				break;
			case 'w':
				fmode = FileStream::FileMode::WRITE;
				break;
			case 'a':
				fmode = FileStream::FileMode::APPEND;
				break;
			case '+':
				if (fmode == FileStream::FileMode::READ) {
					fmode = FileStream::FileMode::READ_EXTEND;
				} else if (fmode == FileStream::FileMode::WRITE) {
					fmode = FileStream::FileMode::WRITE_EXTEND;
				} else if (fmode == FileStream::FileMode::APPEND) {
					fmode = FileStream::FileMode::APPEND_EXTEND;
				}
				break;
			default:
				break;
		}
	}
	
	auto *ret = new FileStream(filename, fmode);
	return ret->isOpen() ? reinterpret_cast<FILE*>(ret) : nullptr;
}

FILE* freopen(const char *filename, const char *mode, FILE *stream) {
	delete stream;
	return fopen(filename, mode);
}

int fclose(FILE *stream) {
	delete stream;
	return 0;
}

int fflush(FILE *stream) {
	auto *fileStream = reinterpret_cast<FileStream*>(stream);
	return fileStream->fflush();
}

void setbuf(FILE *stream, char *buf) {
	if (buf) {
		setvbuf(stream, buf, _IOFBF, BUFSIZ);
	} else {
		setvbuf(stream, nullptr, _IONBF, 0);
	}
}

int setvbuf(FILE *stream, char *buf, const int mode, const size_t size) {
	FileStream::BufferMode bmode;
	
	switch (mode) {
		case _IOFBF:
			bmode = FileStream::BufferMode::FULL;
			break;
		case _IOLBF:
			bmode = FileStream::BufferMode::LINE;
			break;
		case _IONBF:
		default:
			bmode = FileStream::BufferMode::NONE;
			break;
	}

	auto *fileStream = reinterpret_cast<FileStream*>(stream);
	return fileStream->setBuffer(buf, bmode, size);
}

size_t fread(void *buffer, const size_t size, const size_t count, FILE *stream) {
	auto *fileStream = reinterpret_cast<FileStream*>(stream);
	return fileStream->read(static_cast<uint8_t*>(buffer), 0, size * count);
}

size_t fwrite(const void *buffer, const size_t size, const size_t count, FILE * stream) {
	auto *fileStream = reinterpret_cast<FileStream*>(stream);
	fileStream->write(static_cast<const uint8_t*>(buffer), 0, size * count);
	return size * count;
}

int fgetc(FILE *stream) {
	auto *fileStream = reinterpret_cast<FileStream*>(stream);
	return fileStream->read();
}

int getc(FILE *stream) {
	auto *fileStream = reinterpret_cast<FileStream*>(stream);
	return fileStream->read();
}

char* fgets(char *str, const int count, FILE *stream) {
	char *ret = str;
	for (int i = 0; i < count; i++, str++) {
		const int c = getc(stream);
		if (c == EOF) {
			break;
		}

		*str = static_cast<char>(c);

		if (c == '\n') {
			break;
		}
	}

	auto *fileStream = reinterpret_cast<FileStream*>(stream);
	if (fileStream->isError()) {
		return nullptr;
	}
	
	*(str + 1) = '\0';
	return ret;
}

int fputc(const int ch, FILE *stream) {
	auto *fileStream = reinterpret_cast<FileStream*>(stream);
	return fileStream->fputc(ch);
}

int putc(const int ch, FILE *stream) {
	auto *fileStream = reinterpret_cast<FileStream*>(stream);
	return fileStream->fputc(ch);
}

int fputs(const char *str, FILE *stream) {
	auto *fileStream = reinterpret_cast<FileStream*>(stream);
	for (;*str != '\0'; str++) {
		if (fileStream->fputc(*str) == EOF) {
			return EOF;
		}
	}

	return 0;
}

int getchar() {
	return fgetc(stdin);
}

char* gets(char *str) {
	char *ret = str;

	while(true) {
		const int next = getchar();

		if (next == '\n' || next == EOF) {
			*str = '\0';
			if (next == EOF) {
				return nullptr;
			}

			return ret; 
		}

		*str++ = static_cast<char>(next);
	}
}

int putchar(const int ch) {
	return fputc(ch, stdout);
}

int puts(const char *str) {
	return fputs(str, stdout);
}

int ungetc(const int ch, FILE  *stream) {
	auto *fileStream = reinterpret_cast<FileStream*>(stream);
	return fileStream->ungetChar(ch);
}

long ftell(FILE *stream) {
	const auto *fileStream = reinterpret_cast<FileStream*>(stream);
	return static_cast<long>(fileStream->getPos());
}

int fgetpos(FILE *stream, fpos_t *pos) {
	const auto *fileStream = reinterpret_cast<FileStream*>(stream);
	*pos = fileStream->getPos();

	return 0;
}

int fseek(FILE *stream, const long offset, const int origin) {
	FileStream::SeekMode smode;
	
	switch (origin) {
		case SEEK_SET:
		default:
			smode = FileStream::SeekMode::SET;
			break;
		case SEEK_CUR:
			smode = FileStream::SeekMode::CURRENT;
			break;
		case SEEK_END:
			smode = FileStream::SeekMode::END;
			break;
	}

	auto *fileStream = reinterpret_cast<FileStream*>(stream);
	fileStream->setPos(offset, smode);

	return 0;
}

int fsetpos(FILE *stream, const fpos_t *pos) {
	auto *fileStream = reinterpret_cast<FileStream*>(stream);
	fileStream->setPos(*pos, FileStream::SeekMode::SET);

	return 0;
}

void rewind(FILE *stream) {
	auto *fileStream = reinterpret_cast<FileStream*>(stream);
	fileStream->setPos(0, FileStream::SeekMode::SET);
	fileStream->clearError();
}

void clearerr(FILE *stream) {
	auto *fileStream = reinterpret_cast<FileStream*>(stream);
	fileStream->clearError();
}

int feof(FILE *stream) {
	const auto *fileStream = reinterpret_cast<FileStream*>(stream);
	return fileStream->endOfFileReached() ? 1 : 0;
}

int ferror(FILE *stream) {
	const auto *fileStream = reinterpret_cast<FileStream*>(stream);
	return fileStream->isError() ? 1 : 0;
}

void perror(const char *s) {
	if (s && *s) {
		fputs(s, stderr);
		fputs(": ", stderr);
	}
	
	fputs(strerror(errno), stderr);
	fputc('\n', stderr);
}

int remove(const char *pathname) {
	return Util::Io::File(Util::String(pathname)).remove()? 0 : -1;
}

int rename(const char *old_filename, const char *new_filename) {
	FILE *nf = fopen(new_filename, "w");
	FILE *of = fopen(old_filename, "r");
	
	if (!nf || !of) {
		delete nf;
		delete of;
		return -1;
	}
	
	while (true) {
		const int c = fgetc(of);
		if (c == EOF) {
			break;
		}

		fputc(c, nf);
	}

	remove(old_filename);

	delete nf;
	delete of;
	return 0;
}

int lastTmp = 0;
char tmpFileName[L_tmpnam];

char* tmpnam(char *filename) {
	if (lastTmp >= TMP_MAX) {
		return nullptr;
	}

	if (!filename) {
		filename = tmpFileName;
	}
	
	sprintf(filename, "/temp/tmp%d", lastTmp++);

	return filename;
}

FILE* tmpfile() {
	for (char *tmpNam = tmpnam(nullptr); tmpNam != nullptr;  tmpNam = tmpnam(nullptr))	 {
		if (!Util::Io::File(Util::String(tmpNam)).exists()) {
			return fopen(tmpNam, "wb+");
		}
	}

	return nullptr;
}

int stream_vprintf(Util::Io::OutputStream &os, const char *format, const va_list args) {
	return Util::String::format(format, args, os);
}

int vfprintf(FILE *stream, const char *format, const va_list vlist) {
	auto *fileStream = reinterpret_cast<FileStream*>(stream);
	return stream_vprintf(*fileStream, format, vlist);
}

int vprintf(const char *format, const va_list vlist) {
	return vfprintf(stdout, format, vlist);
}

int vsprintf(char *buffer, const char *format, const va_list vlist) {
	Util::Io::ByteArrayOutputStream os(reinterpret_cast<uint8_t*>(buffer), -1);

	const auto ret = stream_vprintf(os, format, vlist);
	os.write('\0');

	return ret;
}

int vsnprintf(char *buffer, const size_t bufsz, const char *format, const va_list vlist) {
	Util::Io::ByteArrayOutputStream os(reinterpret_cast<uint8_t*>(buffer), bufsz);

	const auto ret = stream_vprintf(os, format, vlist);
	os.write('\0');

	return ret;
}

int printf(const char *format, ...) {
	va_list list;

	va_start(list, format);
	const auto ret = vprintf(format, list);
	va_end(list);

	return ret;
}

int fprintf(FILE *stream, const char *format, ...) {
	va_list list;

	va_start(list, format);
	const auto ret = vfprintf(stream, format, list);
	va_end(list);

	return ret;
}

int sprintf(char *buffer, const char *format, ...) {
	va_list list;

	va_start(list, format);
	const int ret = vsprintf(buffer, format, list);
	va_end(list);

	return ret;
}

int snprintf(char * buffer, const size_t bufsz, const char *format, ...) {
	va_list list;

	va_start(list, format);
	const int ret = vsnprintf(buffer, bufsz, format, list);
	va_end(list);

	return ret;
}

int stream_vscanf(Util::Io::InputStream &is, const char *format, const va_list vlist) {
	Util::Io::ScanStream ss(is);
	return ss.scan(format, vlist);
}


int vscanf(const char *format, const va_list args) {
	return vfscanf(stdin, format, args);
}

int vfscanf(FILE *stream, const char *format, const va_list args) {
	auto *fileStream = reinterpret_cast<FileStream*>(stream);
	return stream_vscanf(*fileStream, format, args);
}

int vsscanf(const char *s, const char *format, const va_list args) {
	Util::Io::ByteArrayInputStream bs(reinterpret_cast<const uint8_t*>(s));
	bs.stopAtNullTerminator(true);

	return stream_vscanf(bs, format, args);
}

int scanf(const char *format, ...) {
	va_list args;

	va_start(args, format);
	const auto ret = vscanf(format, args);
	va_end(args);

	return ret;
}

int fscanf(FILE *stream, const char *format, ...) {
	va_list args;

	va_start(args, format);
	const auto ret = vfscanf(stream, format, args);
	va_end(args);

	return ret;
}

int sscanf(const char *s, const char *format, ...) {
	va_list args;

	va_start(args, format);
	const auto ret = vsscanf(s, format, args);
	va_end(args);

	return ret;
}