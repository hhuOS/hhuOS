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
	
	FileStream(const OutputStream &copy) = delete;

    FileStream &operator=(const OutputStream &copy) = delete;
	
	~FileStream();

	virtual void write(uint8_t c);

    virtual void write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length);
	
	int fputc(int c);

    virtual void flush();
	
	int fflush();
	
	virtual int16_t read(); 
	
	virtual int16_t peek();
	
	virtual bool isReadyToRead();

    virtual int32_t read(uint8_t *targetBuffer, uint32_t offset, uint32_t length);
	
	int ungetChar(int ch);
	
	bool readAllowed();
	bool writeAllowed();
	bool isError();
	bool isEOF();
	bool isOpen();
	
	uint32_t getPos();
	void setPos(uint32_t newPos, SeekMode mode);
	
	int setBuffer(char* newBuffer, BufferMode mode, size_t size);
	
	void clearError();
	
	bool setAccessMode(File::AccessMode accessMode) const; 
	
private:

	int32_t fileDescriptor;
	uint32_t pos = 0; //next position to be written to, or start of buffer 
	
	bool _readAllowed = false;
	bool _writeAllowed = false;
	bool _error = false;
	bool _eof = false;
	
	BufferMode bufferMode;
	bool _bufferChangeAllowed = true;
	bool _freeBufferOnDelete = false;
	
	
	uint32_t bufferPos; //current position inside buffer
	uint32_t bufferSize;
	char* buffer = NULL;
	
	Util::ArrayList<int> ungottenChars;
	
};

}

#endif