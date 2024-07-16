#ifndef LIBC_STDIO_FILE_STREAM
#define LIBC_STDIO_FILE_STREAM

#include <cstdint>

#include "lib/util/io/stream/InputStream.h"
#include "lib/util/io/stream/OutputStream.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/libc/stdio_def.h"



namespace Libc {

	
class StdioFileStream : public Util::Io::InputStream, public Util::Io::OutputStream {
public:
	explicit StdioFileStream(const char* filename, FileMode mode);
	
	explicit StdioFileStream(int32_t fileDescriptor, bool allowRead, bool allowWrite);
	
	StdioFileStream(const OutputStream &copy) = delete;

    StdioFileStream &operator=(const OutputStream &copy) = delete;
	
	~StdioFileStream();

	virtual void write(uint8_t c);

    virtual void write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length);
	
	int fputc(int c);

    virtual void flush();
	
	int fflush();
	
	virtual int16_t read(); 
	
	virtual int16_t peek();

    virtual int32_t read(uint8_t *targetBuffer, uint32_t offset, uint32_t length);
	
	int ungetChar(int ch);
	
	bool readAllowed();
	bool writeAllowed();
	bool isError();
	bool isEOF();
	bool isOpen();
	
	uint32_t getPos();
	void setPos(uint32_t newPos, int mode);
	
	int setBuffer(char* newBuffer, int mode, size_t size);
	
	void clearError();
	
	
private:

	int32_t fileDescriptor;
	uint32_t pos = 0; //next position to be written to, or start of buffer 
	
	bool _readAllowed = false;
	bool _writeAllowed = false;
	bool _error = false;
	bool _eof = false;
	
	int bufferMode;
	bool _bufferChangeAllowed = true;
	bool _freeBufferOnDelete = false;
	
	
	uint32_t bufferPos; //current position inside buffer
	uint32_t bufferSize;
	char* buffer = NULL;
	
	Util::ArrayList<int> ungottenChars;
	
};

}

#endif