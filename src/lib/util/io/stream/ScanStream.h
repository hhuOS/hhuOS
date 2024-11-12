#ifndef HHUOS_SCANSTREAM_H
#define HHUOS_SCANSTREAM_H


#include <stdint.h>
#include <stdarg.h>

#include "InputStream.h"

namespace Util::Io {

class ScanStream : public InputStream {

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
	
	void setReadLimit(int64_t limit); //limit is compared to readBytes, -1 = no limit
	
	long long readLong(int base=0);
	
	int32_t readInt(int base=0);
	
	uint32_t readUint(int base=0);
	
	double readDouble();
	
	int vscanf(const char* format, va_list vlist);
	int scanf(const char* format, ...);

private:

	uint32_t readChars = 0;
	int64_t readLimit = -1; // -1 = no limit
	
    InputStream &stream;
};

}


#endif