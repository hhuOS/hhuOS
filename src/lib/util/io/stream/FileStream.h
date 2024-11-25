#ifndef  HHUOS_FILE_STREAM
#define HHUOS_FILE_STREAM

#include <stdint.h>
#include <stddef.h>

#include "lib/util/io/stream/InputStream.h"
#include "lib/util/io/stream/OutputStream.h"
#include "lib/util/io/file/File.h"
#include "lib/util/collection/ArrayList.h"


#ifndef EOF 
#define EOF -1
#endif

namespace Util::Io {

/**
 * A stream that can read from and write to a file.
 * Exposes libc compatible functions (fflush, fputc, ungetc, etc.)
 */
class FileStream : public InputStream, public OutputStream {

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

	explicit FileStream(const char* filename, FileMode mode);
	
	explicit FileStream(int32_t fileDescriptor, bool allowRead, bool allowWrite);
	
	FileStream(const FileStream &copy) = delete;

    FileStream &operator=(const FileStream &copy) = delete;
	
	~FileStream() override;

	void write(uint8_t c) override;

    void write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) override;
	
	int fputc(int c);

    void flush() override;
	
	int fflush();
	
	int16_t read() override;
	
	int16_t peek() override;
	
	bool isReadyToRead() override;

    int32_t read(uint8_t *targetBuffer, uint32_t offset, uint32_t length) override;
	
	int ungetChar(int ch);
	
	[[nodiscard]] bool isReadAllowed() const;

	[[nodiscard]] bool isWriteAllowed() const;

	[[nodiscard]] bool isError() const;

	[[nodiscard]] bool isEOF() const;

	[[nodiscard]] bool isOpen() const;
	
	[[nodiscard]] uint32_t getPos() const;

	void setPos(uint32_t newPos, SeekMode mode);
	
	int setBuffer(char* newBuffer, BufferMode mode, size_t size);
	
	void clearError();
	
	bool setAccessMode(File::AccessMode accessMode) const; 
	
private:

	int32_t fileDescriptor;
	uint32_t pos = 0; // next position to be written to, or start of buffer
	
	bool readAllowed = false;
	bool writeAllowed = false;
	bool error = false;
	bool eof = false;
	
	BufferMode bufferMode;
	bool bufferChangeAllowed = true;
	bool freeBufferOnDelete = false;

	uint32_t bufferPos; // current position inside buffer
	uint32_t bufferSize;
	uint8_t *buffer = nullptr;
	
	Util::ArrayList<int> ungottenChars;
	
};

}

#endif