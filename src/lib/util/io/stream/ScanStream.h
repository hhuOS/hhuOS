#ifndef HHUOS_SCANSTREAM_H
#define HHUOS_SCANSTREAM_H


#include <stdint.h>
#include <stdarg.h>

#include "FilterInputStream.h"

namespace Util::Io {

class ScanStream : public FilterInputStream {

public:

    explicit ScanStream(InputStream &stream);

    ScanStream(const ScanStream &copy) = delete;

    ScanStream &operator=(const ScanStream &copy) = delete;

    ~ScanStream() override = default;

    int16_t read() override;
	
	int16_t peek() override;
	
	bool isReadyToRead() override;

    int32_t read(uint8_t *targetBuffer, uint32_t offset, uint32_t length) override;
	
	[[nodiscard]] uint32_t getReadBytes() const;
	
	long long readLong(int32_t base = 0);
	
	int readInt(int32_t base = 0);
	
	unsigned int readUnsignedint(int32_t base = 0);
	
	double readDouble();

	wchar_t readWideChar();

	int32_t scan(const char* format, ...);
	
	int32_t scan(const char* format, va_list vlist);

private:

	void setReadLimit(int64_t limit);

	static int32_t charToInt(int16_t c, uint32_t base);

	uint32_t readBytes = 0;
	int64_t readLimit = -1;
};

}


#endif